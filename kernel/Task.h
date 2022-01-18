#pragma once

#include "paging.h"

struct Task
{
    int id;                        // Process ID.
    uint32_t esp, ebp;             // Stack and base pointers.
    uint32_t eip;                  // Instruction pointer.
    PageDirectory *page_directory; // Page directory.
    struct Task *next;             // The next task in a linked list.
};

void initialise_tasking();
void switch_task();
int fork();
void move_stack(void *new_stack_start, uint32_t size);
int getpid();