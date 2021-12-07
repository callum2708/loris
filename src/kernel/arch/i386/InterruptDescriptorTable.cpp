#include "InterruptDescriptorTable.h"
#include <stdio.h>

extern void isr0() __asm__ ("isr0");
extern void isr1() __asm__ ("isr1");
extern void isr2() __asm__ ("isr2");
extern void isr3() __asm__ ("isr3");
extern void isr4() __asm__ ("isr4");
extern void isr5() __asm__ ("isr5");
extern void isr6() __asm__ ("isr6");
extern void isr7() __asm__ ("isr7");
extern void isr8() __asm__ ("isr8");
extern void isr9() __asm__ ("isr9");
extern void isr10() __asm__ ("isr10");
extern void isr11() __asm__ ("isr11");
extern void isr12() __asm__ ("isr12");
extern void isr13() __asm__ ("isr13");
extern void isr14() __asm__ ("isr14");
extern void isr15() __asm__ ("isr15");
extern void isr16() __asm__ ("isr16");
extern void isr17() __asm__ ("isr17");
extern void isr18() __asm__ ("isr18");
extern void isr19() __asm__ ("isr19");
extern void isr20() __asm__ ("isr20");
extern void isr21() __asm__ ("isr21");
extern void isr22() __asm__ ("isr22");
extern void isr23() __asm__ ("isr23");
extern void isr24() __asm__ ("isr24");
extern void isr25() __asm__ ("isr25");
extern void isr26() __asm__ ("isr26");
extern void isr27() __asm__ ("isr27");
extern void isr28() __asm__ ("isr28");
extern void isr29() __asm__ ("isr29");
extern void isr30() __asm__ ("isr30");
extern void isr31() __asm__ ("isr31");


extern void load_idt(unsigned int idtPointer) __asm__("load_idt");

typedef struct registers
{
    unsigned int ds;                                     // Data segment selector
    unsigned int edi, esi, ebp, esp, ebx, edx, ecx, eax; // Pushed by pusha.
    unsigned int int_no, err_code;                       // Interrupt number and error code (if applicable)
    unsigned int eip, cs, eflags, useresp, ss;           // Pushed by the processor automatically.
} registers_t;

extern "C" void isr_handler(registers_t regs)
{
    printf("recieved interrupt: ");
    printf("%d", regs.int_no);
    printf("\n");
}

namespace Kernel
{
    InterruptDescriptorTable::InterruptDescriptorTable()
    {
        IDTPointer idtPointer;
        idtPointer.limit = IDT_NUM_ENTRIES * sizeof(IDTGate) - 1;
        idtPointer.base = (unsigned int)&entries;

        CreateGate(0, (unsigned int)isr0, 0x08, 0x8E);
        CreateGate(1, (unsigned int)isr1, 0x08, 0x8E);
        CreateGate(2, (unsigned int)isr2, 0x08, 0x8E);
        CreateGate(3, (unsigned int)isr3, 0x08, 0x8E);
        CreateGate(4, (unsigned int)isr4, 0x08, 0x8E);
        CreateGate(5, (unsigned int)isr5, 0x08, 0x8E);
        CreateGate(6, (unsigned int)isr6, 0x08, 0x8E);
        CreateGate(7, (unsigned int)isr7, 0x08, 0x8E);
        CreateGate(8, (unsigned int)isr8, 0x08, 0x8E);
        CreateGate(9, (unsigned int)isr9, 0x08, 0x8E);
        CreateGate(10, (unsigned int)isr10, 0x08, 0x8E);
        CreateGate(11, (unsigned int)isr11, 0x08, 0x8E);
        CreateGate(12, (unsigned int)isr12, 0x08, 0x8E);
        CreateGate(13, (unsigned int)isr13, 0x08, 0x8E);
        CreateGate(14, (unsigned int)isr14, 0x08, 0x8E);
        CreateGate(15, (unsigned int)isr15, 0x08, 0x8E);
        CreateGate(16, (unsigned int)isr16, 0x08, 0x8E);
        CreateGate(17, (unsigned int)isr17, 0x08, 0x8E);
        CreateGate(18, (unsigned int)isr18, 0x08, 0x8E);
        CreateGate(19, (unsigned int)isr19, 0x08, 0x8E);
        CreateGate(20, (unsigned int)isr20, 0x08, 0x8E);
        CreateGate(21, (unsigned int)isr21, 0x08, 0x8E);
        CreateGate(22, (unsigned int)isr22, 0x08, 0x8E);
        CreateGate(23, (unsigned int)isr23, 0x08, 0x8E);
        CreateGate(24, (unsigned int)isr24, 0x08, 0x8E);
        CreateGate(25, (unsigned int)isr25, 0x08, 0x8E);
        CreateGate(26, (unsigned int)isr26, 0x08, 0x8E);
        CreateGate(27, (unsigned int)isr27, 0x08, 0x8E);
        CreateGate(28, (unsigned int)isr28, 0x08, 0x8E);
        CreateGate(29, (unsigned int)isr29, 0x08, 0x8E);
        CreateGate(30, (unsigned int)isr30, 0x08, 0x8E);
        CreateGate(31, (unsigned int)isr31, 0x08, 0x8E);

        load_idt((unsigned int)&idtPointer);
    }

    void InterruptDescriptorTable::CreateGate(unsigned char num, unsigned int base, unsigned char sel, unsigned char flags)
    {
        entries[num].handler_low = base & 0xFFFF;
        entries[num].handler_high = (base >> 16) & 0xFFFF;

        entries[num].segsel = sel;
        entries[num].zero = 0;
        // We must uncomment the OR below when we get to using user-mode.
        // It sets the interrupt gate's privilege level to 3.
        entries[num].config = flags /* | 0x60 */;
    }
}

