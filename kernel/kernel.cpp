#include <stdio.h>

#include "arch/i386/tty.h"
#include "arch/i386/GlobalDescriptorTable.h"
#include "arch/i386/InterruptDescriptorTable.h"
#include "Timer.h"
#include "Keyboard.h"
#include "kmalloc.h"
#include "paging.h"
#include "multiboot.h"
#include "FIleSystem/FileSystem.h"
#include "FIleSystem/initrd.h"

extern "C" void kernel_main(multiboot *mboot_ptr)
{
    Kernel::InitGDT();
    Kernel::Init_IDT();

    uint32_t initrd_location = *((uint32_t *)mboot_ptr->mods_addr);
    uint32_t initrd_end = *(uint32_t *)(mboot_ptr->mods_addr + 4);

    SetPlacementAddress(initrd_end);

    Kernel::InstallHandler(1, keyboard_handler);

    // Kernel::Timer timer(100);

    initialise_paging();
    printf("Hello, kernel World!\n");
    asm volatile("sti");

    // Initialise the initial ramdisk, and set it as the filesystem root.
   fs_root = initialise_initrd(initrd_location);




   int i = 0;
struct dirent *node = 0;
while ( (node = readdir_fs(fs_root, i)) != 0)
{
  printf("Found file ");
  printf(node->name);
  fs_node *fsnode = finddir_fs(fs_root, node->name);

  if ((fsnode->flags&0x7) == FS_DIRECTORY)
    printf("\n\t(directory)\n");
  else
  {
    printf("\n\t contents: \"");
    char buf[256];
    uint32_t sz = read_fs(fsnode, 0, 256, buf);
    int j;
    for (j = 0; j < sz; j++)
      printf("%c", buf[j]);

    printf("\"\n");
  }
  i++;
}
}
