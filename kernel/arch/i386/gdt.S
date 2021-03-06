.section .text

/* Build GDT pointer and load it */
.global gdt_load_and_set
gdt_load_and_set:
   mov 4(%esp),%eax
   lgdt (%eax)

   mov $0x10, %ax
   mov %ax, %ds
   mov %ax, %es
   mov %ax, %fs
   mov %ax, %gs
   mov %ax, %ss
   jmp  $0x08,$flush

flush:
   ret
