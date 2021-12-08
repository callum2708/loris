.code32

.section .text
.macro ISR_NOERRCODE interruptNumber  /* define a macro, taking one parameter */
  .global isr\interruptNumber       
  isr\interruptNumber:
    push  0
    push \interruptNumber
    jmp isr_common_stub
.endm

.macro ISR_ERRCODE interruptNumber
  .global isr\interruptNumber
  isr\interruptNumber:
    push \interruptNumber
    jmp isr_common_stub
.endm

ISR_NOERRCODE 0
ISR_NOERRCODE 1
ISR_NOERRCODE 2
ISR_NOERRCODE 3
ISR_NOERRCODE 4
ISR_NOERRCODE 5
ISR_NOERRCODE 6
ISR_NOERRCODE 7
ISR_ERRCODE 8
ISR_NOERRCODE 9
ISR_ERRCODE 10
ISR_ERRCODE 11
ISR_ERRCODE 12
ISR_ERRCODE 13
ISR_ERRCODE 14
ISR_NOERRCODE 15
ISR_NOERRCODE 16
ISR_NOERRCODE 17
ISR_NOERRCODE 18
ISR_NOERRCODE 19
ISR_NOERRCODE 20
ISR_NOERRCODE 21
ISR_NOERRCODE 22
ISR_NOERRCODE 23
ISR_NOERRCODE 24
ISR_NOERRCODE 25
ISR_NOERRCODE 26
ISR_NOERRCODE 27
ISR_NOERRCODE 28
ISR_NOERRCODE 29
ISR_NOERRCODE 30
ISR_NOERRCODE 31

.extern isr_handler
isr_common_stub:
   pusha                    /* Pushes edi,esi,ebp,esp,ebx,edx,ecx,eax */

   push %gs
   push %fs
   push %es
   push %ds

   mov $0x10, %ax /* load the kernel data segment descriptor */
   mov %ax, %ds
   mov %ax, %es
   mov %ax, %fs
   mov %ax, %gs

   push %esp
   call isr_handler
    add $4, %esp

   pop %ds
   pop %es
   pop %fs
   pop %gs

   popa                     /* Pops edi,esi,ebp... */
   add %esp, 8
   iret           /* pops 5 things at once: CS, EIP, EFLAGS, SS, and ESP */