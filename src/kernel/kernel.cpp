#include <stdio.h>

#include "arch/i386/tty.h"
#include "arch/i386/GlobalDescriptorTable.h"
#include "arch/i386/InterruptDescriptorTable.h"


extern "C" void kernel_main(void) {
	Kernel::GlobalDescriptorTable gdt();
	Kernel::InterruptDescriptorTable idt();
	printf("Hello, kernel World!\n");

	asm volatile ("int $0x3");
	// asm volatile ("int $0x4");	
}