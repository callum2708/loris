#pragma once

#include <stdint.h>

void* kmalloc_a(uint32_t size);  // page aligned.
void* kmalloc_p(uint32_t size, uint32_t *phys); // returns a physical address.
void* kmalloc_ap(uint32_t size, uint32_t *phys); // page aligned and returns a physical address.

void* kmalloc(uint32_t sz, bool align, uint32_t *phys);
void kfree(void *ptr);
