#include <stdio.h>

#include "arch/i386/tty.h"
#include "arch/i386/GlobalDescriptorTable.h"
#include "arch/i386/InterruptDescriptorTable.h"
#include "Timer.h"
#include "Keyboard.h"
#include "kmalloc.h"
#include "paging.h"

extern "C" void kernel_main(void)
{
    Kernel::InitGDT();
    Kernel::Init_IDT();

    Kernel::InstallHandler(1, keyboard_handler);

    // int *ptr = (int*)0xA0000000;
    // int do_page_fault = *ptr;

    // Kernel::Timer timer(100);


    initialise_paging();
    printf("Hello, kernel World!\n");
    asm volatile("sti");
    uint32_t a = (uint32_t)kmalloc(4, false, nullptr);
}
