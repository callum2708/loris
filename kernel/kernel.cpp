#include <stdio.h>

#include <arch/i386/tty.h>
#include "arch/i386/GlobalDescriptorTable.h"
#include "arch/i386/InterruptDescriptorTable.h"
#include "Timer.h"
#include "Keyboard.h"
#include "kmalloc.h"
#include "paging.h"
#include "multiboot.h"
#include "FIleSystem/FileSystem.h"
#include "FIleSystem/initrd.h"
#include <Task.h>

uint32_t initial_esp; // New global variable.

extern "C" void kernel_main(multiboot *mboot_ptr, uint32_t initial_stack)
{
  initial_esp = initial_stack;

  Kernel::InitGDT();
  Kernel::Init_IDT();

  uint32_t initrd_location = *((uint32_t *)mboot_ptr->mods_addr);
  uint32_t initrd_end = *(uint32_t *)(mboot_ptr->mods_addr + 4);

  SetPlacementAddress(initrd_end);

  Kernel::InstallHandler(1, keyboard_handler);

  Kernel::Timer timer(100);
  initialise_paging();
  initialise_tasking();

     // Create a new process in a new address space which is a clone of this.
   int ret = fork();
   int pid = getpid();

  printf("Hello, kernel World!\n");
  asm volatile("sti");

  // Initialise the initial ramdisk, and set it as the filesystem root.
  fs_root = initialise_initrd(initrd_location);
}
