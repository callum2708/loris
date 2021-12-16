.section .text
.global load_idt   
load_idt:
   mov 4(%esp), %eax /* Get the pointer to the IDT, passed as a parameter. */
   lidt (%eax)       /* Load the IDT pointer. */
   ret

.macro ISR_NOERRCODE interruptNumber  /* define a macro, taking one parameter */
  .global isr\interruptNumber       
  isr\interruptNumber:
    pushl $0
    pushl $\interruptNumber
    jmp isr_common_stub
.endm

.macro ISR_ERRCODE interruptNumber
  .global isr\interruptNumber
  isr\interruptNumber:
    pushl $\interruptNumber
    jmp isr_common_stub
.endm

isr_common_stub:
   pushal                    /* Pushes edi,esi,ebp,esp,ebx,edx,ecx,eax */

   pushl %gs
   pushl %fs
   pushl %es
   pushl %ds

   mov $0x10, %ax /* load the kernel data segment descriptor */
   mov %ax, %ds
   mov %ax, %es
   mov %ax, %fs
   mov %ax, %gs

   pushl %esp
   call isr_handler
   add $4, %esp

   popl %ds
   popl %es
   popl %fs
   popl %gs

   popal                     /* Pops edi,esi,ebp... */
   add $8, %esp
   iret           /* pops 5 things at once: CS, EIP, EFLAGS, SS, and ESP */

.macro IRQ IRQNumber
  .global irq\IRQNumber
  irq\IRQNumber:
    pushl $0    /* Note that these don't pushl an error code on the stack: */
    pushl $\IRQNumber
    jmp irq_common_stub
.endm

irq_common_stub:
  pushal                    /* Pushes edi,esi,ebp,esp,ebx,edx,ecx,eax */

   pushl %gs
   pushl %fs
   pushl %es
   pushl %ds

   mov $0x10, %ax /* load the kernel data segment descriptor */
   mov %ax, %ds
   mov %ax, %es
   mov %ax, %fs
   mov %ax, %gs

   pushl %esp
   call irq_handler
   add $4, %esp

   popl %ds
   popl %es
   popl %fs
   popl %gs

   popal                     /* Pops edi,esi,ebp... */
   add $8, %esp
   iret           /* pops 5 things at once: CS, EIP, EFLAGS, SS, and ESP */

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
IRQ 32
IRQ 33
IRQ 34
IRQ 35
IRQ 36
IRQ 37
IRQ 38
IRQ 39
IRQ 40
IRQ 41
IRQ 42
IRQ 43
IRQ 44
IRQ 45
IRQ 46
IRQ 47

