#pragma once

#include <stdint.h>
#include "paging.h"

#define INDEX_FROM_BIT(a) (a / (8 * 4))
#define OFFSET_FROM_BIT(a) (a % (8 * 4))

void set_frame(uint32_t frame_addr);
void clear_frame(uint32_t frame_addr);
uint32_t test_frame(uint32_t frame_addr);
uint32_t first_frame();
void alloc_frame(Page *page, bool isKernel, bool isWritable);
void free_frame(Page *page);
