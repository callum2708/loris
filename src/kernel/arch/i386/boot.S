# Declare constants for the multiboot header.
.set ALIGN,    1<<0             # align loaded modules on page boundaries
.set MEMINFO,  1<<1             # provide memory map
.set FLAGS,    ALIGN | MEMINFO  # this is the Multiboot 'flag' field
.set MAGIC,    0x1BADB002       # 'magic number' lets bootloader find the header
.set CHECKSUM, -(MAGIC + FLAGS) # checksum of above, to prove we are multiboot

# Declare a header as in the Multiboot Standard.
.section .multiboot
.align 4
.long MAGIC
.long FLAGS
.long CHECKSUM

# Reserve a stack for the initial thread.
.section .bss
.align 16
stack_bottom:
.skip 16384 # 16 KiB
stack_top:

/* Build GDT pointer and load it */
.global gdt_load_and_set

gdt_load_and_set:
	mov %eax, 4(%esp)
    lgdt    (%eax)               /* load gdt table */
    /* load cs segment by doing a far jump */
    pushw $8
    push $reload_segments
    lret
    hlt
	
.global load_idt   
load_idt:
   mov %eax, 4(%esp)  /* Get the pointer to the IDT, passed as a parameter. */
   lidt (%eax)       /* Load the IDT pointer. */
   ret

.macro ISR_NOERRCODE interruptNumber  /* define a macro, taking one parameter */
  .global isr\interruptNumber       
  isr\interruptNumber:
    cli
    push  0
    push \interruptNumber
    jmp isr_common_stub
.endm

.macro ISR_ERRCODE interruptNumber
  .global isr\interruptNumber
  isr\interruptNumber:
    cli
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

isr_common_stub:
   pusha                    /* Pushes edi,esi,ebp,esp,ebx,edx,ecx,eax */

   mov %ax, %ds               /* Lower 16-bits of eax = ds. */
   push %eax                 /* save the data segment descriptor */

   mov %ax, 0x10  /* load the kernel data segment descriptor */
   mov %ds, %ax
   mov %es, %ax
   mov %fs, %ax
   mov %gs, %ax

   call isr_handler

   pop %eax        /* reload the original data segment descriptor */
   mov %ds, %ax
   mov %es, %ax
   mov %fs, %ax
   mov %gs, %ax

   popa                     /* Pops edi,esi,ebp... */
   add %esp, 8     /* Cleans up the pushed error code and pushed ISR number */
   sti
   iret           /* pops 5 things at once: CS, EIP, EFLAGS, SS, and ESP */

.reload_segments:
    /* we only use one segment for data*/
    mov     %ax, 0x10
    mov     %ds, %ax
    mov     %ss, %ax
    mov     %es, %ax
    mov     %gs, %ax
    mov     %fs, %ax
    ret


# The kernel entry point.
.section .text
.global _start
.type _start, @function
_start:
	movl $stack_top, %esp

	# Call the global constructors.
	call _init

	# Transfer control to the main kernel.
	call kernel_main

	# Hang if kernel_main unexpectedly returns.
	cli
1:	hlt
	jmp 1b
.size _start, . - _start
