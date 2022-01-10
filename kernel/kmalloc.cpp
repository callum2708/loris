#include "kmalloc.h"
#include "Heap.h"
#include <stdio.h>

extern uint32_t end;
uint32_t placement_address = (uint32_t)&end;

Heap *kheap = 0;

void *kmalloc(uint32_t size, bool align, uint32_t *phys)
{
    if (kheap)
    {
        return (void *)kheap->Alloc(size, align);
    }

    if (align == true && (placement_address & 0xFFFFF000)) // If the address is not already page-aligned
    {
        // Align it.
        placement_address &= 0xFFFFF000;
        placement_address += 0x1000;
    }
    if (phys)
    {
        *phys = placement_address;
    }
    uint32_t tmp = placement_address;
    placement_address += size;
    return (void *)tmp;
}

void kfree(void *ptr)
{
    if (kheap)
    {
        kheap->Free(ptr);
    }
}

void *kmalloc_a(uint32_t size)
{
    return kmalloc(size, true, nullptr);
}
void *kmalloc_p(uint32_t size, uint32_t *phys)
{
    return kmalloc(size, false, phys);
}
void *kmalloc_ap(uint32_t size, uint32_t *phys)
{
    return kmalloc(size, true, phys);
}

void SetPlacementAddress(uint32_t address)
{
    placement_address = address;
}
