#include "paging.h"
#include <stdio.h>
#include <string.h>

#include "kmalloc.h"
#include "arch/i386/InterruptDescriptorTable.h"
#include "Heap.h"

extern uint32_t placement_address;
uint32_t nframes;
uint32_t *frames;

PageDirectory *kernel_directory = 0;
PageDirectory *current_directory = 0;

extern Heap *kheap;

void set_frame(uint32_t frame_addr)
{
    uint32_t frame = frame_addr / 0x1000;
    uint32_t idx = INDEX_FROM_BIT(frame);
    uint32_t off = OFFSET_FROM_BIT(frame);
    frames[idx] |= (0x1 << off);
}

void clear_frame(uint32_t frame_addr)
{
    uint32_t frame = frame_addr / 0x1000;
    uint32_t idx = INDEX_FROM_BIT(frame);
    uint32_t off = OFFSET_FROM_BIT(frame);
    frames[idx] &= ~(0x1 << off);
}

uint32_t test_frame(uint32_t frame_addr)
{
    uint32_t frame = frame_addr / 0x1000;
    uint32_t idx = INDEX_FROM_BIT(frame);
    uint32_t off = OFFSET_FROM_BIT(frame);
    return (frames[idx] & (0x1 << off));
}

uint32_t first_frame()
{
    uint32_t i, j;
    for (i = 0; i < INDEX_FROM_BIT(nframes); i++)
    {
        if (frames[i] != 0xFFFFFFFF) // nothing free, exit early.
        {
            // at least one bit is free here.
            for (j = 0; j < 32; j++)
            {
                uint32_t toTest = 0x1 << j;
                if (!(frames[i] & toTest))
                {
                    return i * 4 * 8 + j;
                }
            }
        }
    }
}

void alloc_frame(Page *page, bool isKernel, bool isWritable)
{
    if (page->Frame != 0)
    {
        return; // Frame was already allocated, return straight away.
    }
    else
    {
        uint32_t idx = first_frame(); // idx is now the index of the first free frame.
        if (idx == (uint32_t)-1)
        {
            //TODO: create a panic macro
            printf("No free frames!");
            while (true)
                ;
        }
        set_frame(idx * 0x1000);                // this frame is now ours!
        page->Present = 1;                      // Mark it as present.
        page->ReadWrite = (isWritable) ? 1 : 0; // Should the page be writeable?
        page->User = (isKernel) ? 0 : 1;        // Should the page be user-mode?
        page->Frame = idx;
    }
}

// Function to deallocate a frame.
void free_frame(Page *page)
{
    uint32_t frame;
    if (!(frame = page->Frame))
    {
        return; // The given page didn't actually have an allocated frame!
    }
    else
    {
        clear_frame(frame); // Frame is now free again.
        page->Frame = 0x0;  // Page now doesn't have a frame.
    }
}

void switch_page_directory(PageDirectory *dir)
{
    current_directory = dir;
    asm volatile("mov %0, %%cr3" ::"r"(&dir->TablesPhysical));
    uint32_t cr0;
    asm volatile("mov %%cr0, %0"
                 : "=r"(cr0));
    cr0 |= 0x80000000; // Enable paging!
    asm volatile("mov %0, %%cr0" ::"r"(cr0));
}

Page *get_page(uint32_t address, int make, PageDirectory *dir)
{
    // Turn the address into an index.
    address /= 0x1000;
    // Find the page table containing this address.
    uint32_t table_idx = address / 1024;
    if (dir->Tables[table_idx]) // If this table is already assigned
    {
        return &dir->Tables[table_idx]->Pages[address % 1024];
    }
    else if (make)
    {
        uint32_t tmp;
        dir->Tables[table_idx] = (PageTable *)kmalloc_ap(sizeof(PageTable), &tmp);
        memset(dir->Tables[table_idx], 0, 0x1000);
        dir->TablesPhysical[table_idx] = tmp | 0x7; // PRESENT, RW, US.
        return &dir->Tables[table_idx]->Pages[address % 1024];
    }
    else
    {
        return 0;
    }
}

void initialise_paging()
{
    // The size of physical memory. For the moment we
    // assume it is 16MB big.
    uint32_t mem_end_page = 0x1000000;

    nframes = mem_end_page / 0x1000;
    frames = (uint32_t *)kmalloc(INDEX_FROM_BIT(nframes), false, nullptr);
    memset(frames, 0, INDEX_FROM_BIT(nframes));

    // Let's make a page directory.
    kernel_directory = (PageDirectory *)kmalloc_a(sizeof(PageDirectory));
    memset(kernel_directory, 0, sizeof(PageDirectory));
    current_directory = kernel_directory;

    int i = 0;
    for (i = KHEAP_START; i < KHEAP_START + KHEAP_INITIAL_SIZE; i += 0x1000)
        get_page(i, 1, kernel_directory);

    // We need to identity map (phys addr = virt addr) from
    // 0x0 to the end of used memory, so we can access this
    // transparently, as if paging wasn't enabled.
    // NOTE that we use a while loop here deliberately.
    // inside the loop body we actually change placement_address
    // by calling kmalloc(). A while loop causes this to be
    // computed on-the-fly rather than once at the start.
    i = 0;
    while (i < placement_address+0x1000)
    {
        // Kernel code is readable but not writeable from userspace.
        alloc_frame(get_page(i, true, kernel_directory), false, false);
        i += 0x1000;
    }

    // Now allocate those pages we mapped earlier.
    for (i = KHEAP_START; i < KHEAP_START + KHEAP_INITIAL_SIZE; i += 0x1000)
        alloc_frame(get_page(i, 1, kernel_directory), false, false);

    // Before we enable paging, we must register our page fault handler.
    //TODO: page fault handler
    //    InstallHandler(14, page_fault);

    switch_page_directory(kernel_directory);

    printf("help");

    //do this in two steps as we don't have a heap/operator new yet
    kheap = (Heap *)kmalloc(sizeof(Heap), false, nullptr);
    *kheap = Heap(KHEAP_START, KHEAP_START + KHEAP_INITIAL_SIZE, 0xCFFFF000, false, false);
}