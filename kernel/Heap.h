#pragma once

#include <stdint.h>
#include "../Util/OrderedList.h"
#include <stdint.h>

#define KHEAP_START 0xC0000000
#define KHEAP_INITIAL_SIZE 0x100000
#define HEAP_INDEX_SIZE 0x20000
#define HEAP_MAGIC 0x12345678
#define HEAP_MIN_SIZE 0x70000

struct Header
{
   uint32_t magic; // Magic number, used for error checking and identification.
   bool is_hole;   // 1 if this is a hole. 0 if this is a block.
   uint32_t size;  // size of the block, including the end footer.
};

struct Footer
{
   uint32_t magic; // Magic number, same as in header_t.
   Header *header; // Pointer to the block header.
};

class Heap
{
public:
   Heap(uint32_t start, uint32_t end, uint32_t max, bool supervisor, bool readonly);
   void *Alloc(uint32_t size, bool page_align);
   void Free(void *p);

private:
   ordered_array_t m_index;
   uint32_t m_startAddress; // The start of our allocated space.
   uint32_t m_endAddress;   // The end of our allocated space. May be expanded up to max_address.
   uint32_t m_maxAddress;   // The maximum address the heap can be expanded to.
   bool m_supervisor;       // Should extra pages requested by us be mapped as supervisor-only?
   bool m_readonly;         // Should extra pages requested by us be mapped as read-only?

   uint32_t FindSmallestHole(uint32_t size, bool page_align);
   void Expand(uint32_t new_size);
   uint32_t Contract(uint32_t new_size);
};

   bool HeaderLessThan(void *a, void *b);