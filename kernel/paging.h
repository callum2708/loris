#pragma once

#include <stdint.h>
#include "arch/i386/InterruptDescriptorTable.h"

#define INDEX_FROM_BIT(a) (a / (8 * 4))
#define OFFSET_FROM_BIT(a) (a % (8 * 4))

struct Page
{
   uint32_t Present : 1;  // Page present in memory
   uint32_t ReadWrite: 1;       // Read-only if clear, readwrite if set
   uint32_t User : 1;     // Supervisor level only if clear
   uint32_t Accessed : 1; // Has the page been accessed since last refresh?
   uint32_t Dirty : 1;    // Has the page been written to since last refresh?
   uint32_t Unused : 7;   // Amalgamation of unused and reserved bits
   uint32_t Frame : 20;   // Frame address (shifted right 12 bits)
};

struct PageTable
{
   Page Pages[1024];
};

struct PageDirectory
{
   PageTable *Tables[1024];
   uint32_t TablesPhysical[1024];
   uint32_t PhysicalAddress;
};

void initialise_paging();
void switch_page_directory(PageDirectory *newPageDirectory);
Page *get_page(uint32_t address, int make, PageDirectory *dir);

//TODO: move to frame allocator
void set_frame(uint32_t frame_addr);
void clear_frame(uint32_t frame_addr);
uint32_t test_frame(uint32_t frame_addr);
uint32_t first_frame();
void alloc_frame(Page *page, bool isKernel, bool isWritable);
void free_frame(Page *page);

void page_fault(Kernel::Registers *registers);