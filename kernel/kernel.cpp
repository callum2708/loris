#include <stdio.h>

#include "arch/i386/tty.h"
#include "arch/i386/GlobalDescriptorTable.h"
#include "arch/i386/InterruptDescriptorTable.h"
#include "arch/i386/InterruptRequestManager.h"
#include "Timer.h"

extern "C" void kernel_main(void) {
	Kernel::GlobalDescriptorTable gdt;
	Kernel::InterruptDescriptorTable idt;
	Kernel::InterruptRequestManager::Initialise(&idt);


	Kernel::Timer timer(100);
	asm volatile("sti");

	printf("Hello, kernel World!\n");

}
