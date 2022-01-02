#include "PageFrameAllocator.h"

#include <stdio.h>

uint32_t *frames;
uint32_t nframes;

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
            while (true);
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