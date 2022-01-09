#include "Heap.h"
#include "kmalloc.h"
#include "paging.h"
#include <stdio.h>

extern PageDirectory *kernel_directory;
extern PageDirectory *current_directory;

Heap::Heap(uint32_t start, uint32_t end_addr, uint32_t max, bool m_supervisor, bool m_readonly)
    : m_index((void *)start, HEAP_INDEX_SIZE, &HeaderLessThan)
{
    // Shift the start address forward to resemble where we can start putting data.
    start += sizeof(type_t) * HEAP_INDEX_SIZE;

    // Make sure the start address is page-aligned.
    if (start & 0xFFFFF000 != 0)
    {
        start &= 0xFFFFF000;
        start += 0x1000;
    }
    // Write the start, end and max addresses into the heap structure.
    m_startAddress = start;
    m_endAddress = end_addr;
    m_startAddress = max;
    m_supervisor = m_supervisor;
    m_readonly = m_readonly;

    // We start off with one large hole in the index.
    Header *hole = (Header *)start;
    hole->size = end_addr - start;
    hole->magic = HEAP_MAGIC;
    hole->is_hole = 1;
    m_index.Insert(hole);
}

void *Heap::Alloc(uint32_t size, bool page_align)
{

    // Make sure we take the size of header/footer into account.
    uint32_t new_size = size + sizeof(Header) + sizeof(Footer);
    // Find the smallest hole that will fit.
    uint32_t iterator = FindSmallestHole(new_size, page_align);

    if (iterator == -1) // If we didn't find a suitable hole
    {
        uint32_t old_length = m_endAddress - m_startAddress;
        uint32_t old_m_endAddress = m_endAddress;

        // We need to allocate some more space.
        Expand(old_length + new_size);
        uint32_t new_length = m_endAddress - m_startAddress;

        // Find the endmost header. (Not endmost in size, but in location).
        iterator = 0;
        // Vars to hold the index of, and value of, the endmost header found so far.
        uint32_t idx = -1;
        uint32_t value = 0x0;
        while (iterator < m_index.Size())
        {
            uint32_t tmp = (uint32_t)m_index.GetItemAtIndex(iterator);
            if (tmp > value)
            {
                value = tmp;
                idx = iterator;
            }
            iterator++;
        }

        // If we didn't find ANY headers, we need to add one.
        if (idx == -1)
        {
            Header *header = (Header *)old_m_endAddress;
            header->magic = HEAP_MAGIC;
            header->size = new_length - old_length;
            header->is_hole = 1;
            Footer *footer = (Footer *)(old_m_endAddress + header->size - sizeof(Footer));
            footer->magic = HEAP_MAGIC;
            footer->header = header;
            m_index.Insert(header);
        }
        else
        {
            // The last header needs adjusting.
            Header *header = (Header *)m_index.GetItemAtIndex(idx);
            header->size += new_length - old_length;
            // Rewrite the footer.
            Footer *footer = (Footer *)((uint32_t)header + header->size - sizeof(Footer));
            footer->header = header;
            footer->magic = HEAP_MAGIC;
        }
        // We now have enough space. Recurse, and call the function again.
        return Alloc(size, page_align);
    }

    Header *orig_hole_header = m_index.GetItemAtIndex(iterator);
    uint32_t orig_hole_pos = (uint32_t)orig_hole_header;
    uint32_t orig_hole_size = orig_hole_header->size;
    // Here we work out if we should split the hole we found into two parts.
    // Is the original hole size - requested hole size less than the overhead for adding a new hole?
    if (orig_hole_size - new_size < sizeof(Header) + sizeof(Footer))
    {
        // Then just increase the requested size to the size of the hole we found.
        size += orig_hole_size - new_size;
        new_size = orig_hole_size;
    }

    // If we need to page-align the data, do it now and make a new hole in front of our block.
    if (page_align && orig_hole_pos & 0xFFFFF000)
    {
        uint32_t new_location = orig_hole_pos + 0x1000 /* page size */ - (orig_hole_pos & 0xFFF) - sizeof(Header);
        Header *hole_header = (Header *)orig_hole_pos;
        hole_header->size = 0x1000 /* page size */ - (orig_hole_pos & 0xFFF) - sizeof(Header);
        hole_header->magic = HEAP_MAGIC;
        hole_header->is_hole = 1;
        Footer *hole_footer = (Footer *)((uint32_t)new_location - sizeof(Footer));
        hole_footer->magic = HEAP_MAGIC;
        hole_footer->header = hole_header;
        orig_hole_pos = new_location;
        orig_hole_size = orig_hole_size - hole_header->size;
    }
    else
    {
        // Else we don't need this hole any more, delete it from the index.
        m_index.Remove(iterator);
    }
    // Overwrite the original header...
    Header *block_header = (Header *)orig_hole_pos;
    block_header->magic = HEAP_MAGIC;
    block_header->is_hole = 0;
    block_header->size = new_size;
    // ...And the footer
    Footer *block_footer = (Footer *)(orig_hole_pos + sizeof(Header) + size);
    block_footer->magic = HEAP_MAGIC;
    block_footer->header = block_header;

    // We may need to write a new hole after the allocated block.
    // We do this only if the new hole would have positive size...
    if (orig_hole_size - new_size > 0)
    {
        Header *hole_header = (Header *)(orig_hole_pos + sizeof(Header) + size + sizeof(Footer));
        hole_header->magic = HEAP_MAGIC;
        hole_header->is_hole = 1;
        hole_header->size = orig_hole_size - new_size;
        Footer *hole_footer = (Footer *)((uint32_t)hole_header + orig_hole_size - new_size - sizeof(Footer));
        if ((uint32_t)hole_footer < m_endAddress)
        {
            hole_footer->magic = HEAP_MAGIC;
            hole_footer->header = hole_header;
        }
        m_index.Insert(hole_header);
    }

    return (void *)((uint32_t)block_header + sizeof(Header));
}

void Heap::Free(void *p)
{
    if (p == 0)
        return;

    // Get the header and footer associated with this pointer.
    Header *header = (Header *)((uint32_t)p - sizeof(Header));
    Footer *footer = (Footer *)((uint32_t)header + header->size - sizeof(Footer));

    //TODO: add assert
    //    ASSERT(header->magic == HEAP_MAGIC);
    //    ASSERT(footer->magic == HEAP_MAGIC);
    header->is_hole = 1;

    bool do_add = true;
    // Unify left
    // If the thing immediately to the left of us is a footer...
    Footer *test_footer = (Footer *)((uint32_t)header - sizeof(Footer));
    if (test_footer->magic == HEAP_MAGIC &&
        test_footer->header->is_hole == 1)
    {
        uint32_t cache_size = header->size; // Cache our current size.
        header = test_footer->header;       // Rewrite our header with the new one.
        footer->header = header;            // Rewrite our footer to point to the new header.
        header->size += cache_size;         // Change the size.
        do_add = false;                     // Since this header is already in the index, we don't want to add it again.
    }

    // Unify right
    // If the thing immediately to the right of us is a header...
    Header *test_header = (Header *)((uint32_t)footer + sizeof(Footer));
    if (test_header->magic == HEAP_MAGIC &&
        test_header->is_hole)
    {
        header->size += test_header->size;               // Increase our size.
        test_footer = (Footer *)((uint32_t)test_header + // Rewrite it's footer to point to our header.
                                 test_header->size - sizeof(Footer));
        footer = test_footer;
        // Find and remove this header from the index.
        uint32_t iterator = 0;
        while ((iterator < m_index.Size()) &&
               (m_index.GetItemAtIndex(iterator) != test_header))
            iterator++;

        // Make sure we actually found the item.
        //TODO: Add Assert
        // ASSERT(iterator < index.size);
        m_index.Remove(iterator);
    }
    // If the footer location is the end address, we can contract.
    if ((uint32_t)footer + sizeof(Footer) == m_endAddress)
    {
        uint32_t old_length = m_endAddress - m_startAddress;
        uint32_t new_length = Contract((uint32_t)header - m_startAddress);
        // Check how big we will be after resizing.
        if (header->size - (old_length - new_length) > 0)
        {
            // We will still exist, so resize us.
            header->size -= old_length - new_length;
            footer = (Footer *)((uint32_t)header + header->size - sizeof(Footer));
            footer->magic = HEAP_MAGIC;
            footer->header = header;
        }
        else
        {
            // We will no longer exist :(. Remove us from the index.
            uint32_t iterator = 0;
            while ((iterator < m_index.Size()) &&
                   (m_index.GetItemAtIndex(iterator) != test_header))
                iterator++;
            // If we didn't find ourselves, we have nothing to remove.
            if (iterator < m_index.Size())
                m_index.Remove(iterator);
        }
    }

    if (do_add)
        m_index.Insert(header);
}

uint32_t Heap::FindSmallestHole(uint32_t size, bool pageAlign)
{
    // Find the smallest hole that will fit.
    uint32_t iterator = 0;
    while (iterator < m_index.Size())
    {
        Header *header = m_index.GetItemAtIndex(iterator);
        if (pageAlign)
        {
            uint32_t location = (uint32_t)header;
            uint32_t offset = 0;
            if ((location + sizeof(Header)) & 0xFFFFF000 != 0)
                offset = 0x1000 /* page size */ - (location + sizeof(Header)) % 0x1000;
            uint32_t hole_size = (uint32_t)header->size - offset;
            // Can we fit now?
            if (hole_size >= (uint32_t)size)
                break;
        }
        else if (header->size >= size)
            break;
        iterator++;
    }

    if (iterator == m_index.Size())
        return -1; // We got to the end and didn't find anything.
    else
        return iterator;
}

bool HeaderLessThan(void *a, void *b)
{
    return (((Header *)a)->size < ((Header *)b)->size);
}

void Heap::Expand(uint32_t new_size)
{
    // Get the nearest following page boundary.
    if (new_size & 0xFFFFF000 != 0)
    {
        new_size &= 0xFFFFF000;
        new_size += 0x1000;
    }

    // This should always be on a page boundary.
    uint32_t old_size = m_endAddress - m_startAddress;
    uint32_t i = old_size;
    while (i < new_size)
    {
        alloc_frame(get_page(m_startAddress + i, 1, kernel_directory),
                    (m_supervisor) ? 1 : 0, (m_readonly) ? 0 : 1);
        i += 0x1000 /* page size */;
    }
    m_endAddress = m_startAddress + new_size;
}

uint32_t Heap::Contract(uint32_t new_size)
{
    // Get the nearest following page boundary.
    if (new_size & 0x1000)
    {
        new_size &= 0x1000;
        new_size += 0x1000;
    }
    // Don't contract too far!
    if (new_size < HEAP_MIN_SIZE)
        new_size = HEAP_MIN_SIZE;
    uint32_t old_size = m_endAddress - m_startAddress;
    uint32_t i = old_size - 0x1000;
    while (new_size < i)
    {
        free_frame(get_page(m_startAddress + i, 0, kernel_directory));
        i -= 0x1000;
    }
    m_endAddress = m_startAddress + new_size;
    return new_size;
}