#include <stdio.h>

#include "arch/i386/tty.h"
#include "arch/i386/GlobalDescriptorTable.h"
#include "arch/i386/InterruptDescriptorTable.h"
#include "Timer.h"
#include "Keyboard.h"

extern "C" void kernel_main(void)
{
	Kernel::InitGDT();
	Kernel::Init_IDT();

	Kernel::InstallHandler(1, keyboard_handler);


	// Kernel::Timer timer(100);
	asm volatile("sti");

	printf("Hello, kernel World!\n");
}
