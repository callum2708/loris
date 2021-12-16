#include "InterruptDescriptorTable.h"
#include <stdio.h>
#include <string.h>
#include "../../io.h"

extern "C" void isr0();
extern "C" void isr1();
extern "C" void isr2();
extern "C" void isr3();
extern "C" void isr4();
extern "C" void isr5();
extern "C" void isr6();
extern "C" void isr7();
extern "C" void isr8();
extern "C" void isr9();
extern "C" void isr10();
extern "C" void isr11();
extern "C" void isr12();
extern "C" void isr13();
extern "C" void isr14();
extern "C" void isr15();
extern "C" void isr16();
extern "C" void isr17();
extern "C" void isr18();
extern "C" void isr19();
extern "C" void isr20();
extern "C" void isr21();
extern "C" void isr22();
extern "C" void isr23();
extern "C" void isr24();
extern "C" void isr25();
extern "C" void isr26();
extern "C" void isr27();
extern "C" void isr28();
extern "C" void isr29();
extern "C" void isr30();
extern "C" void isr31();
extern "C" void irq32();
extern "C" void irq33();
extern "C" void irq34();
extern "C" void irq35();
extern "C" void irq36();
extern "C" void irq37();
extern "C" void irq38();
extern "C" void irq39();
extern "C" void irq40();
extern "C" void irq41();
extern "C" void irq42();
extern "C" void irq43();
extern "C" void irq44();
extern "C" void irq45();
extern "C" void irq46();
extern "C" void irq47();

void *irq_routines[16] = {0};

extern "C" void load_idt(unsigned int idtPointer);

extern "C" void isr_handler(Kernel::Registers *regs)
{
    printf("recieved interrupt");
    while (true);
}

extern "C" void irq_handler(Kernel::Registers *registers)
{
    /* This is a blank function pointer */
    void (*handler)(struct Kernel::Registers * r);

    /* Find out if we have a custom handler to run for this
    *  IRQ, and then finally, run it */
    handler = irq_routines[registers->int_no - 32];
    if (handler)
    {
        handler(registers);
    }

    /* If the IDT entry that was invoked was greater than 40
        *  (meaning IRQ8 - 15), then we need to send an EOI to
        *  the slave controller */
    if (registers->int_no >= 40)
    {
        outb(0xA0, 0x20);
    }

    /* In either case, we need to send an EOI to the master
        *  interrupt controller too */
    outb(0x20, 0x20);
}

Kernel::IDTPointer idtPointer;
Kernel::IDTGate entries[IDT_NUM_ENTRIES];

namespace Kernel
{
    void Init_IDT()
    {
        memset(entries, 0x00, sizeof(entries));
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

        RemapPIC();
        CreateGate(32, (unsigned int)irq32, 0x08, 0x8E);
        CreateGate(33, (unsigned int)irq33, 0x08, 0x8E);
        CreateGate(34, (unsigned int)irq34, 0x08, 0x8E);
        CreateGate(35, (unsigned int)irq35, 0x08, 0x8E);
        CreateGate(36, (unsigned int)irq36, 0x08, 0x8E);
        CreateGate(37, (unsigned int)irq37, 0x08, 0x8E);
        CreateGate(38, (unsigned int)irq38, 0x08, 0x8E);
        CreateGate(39, (unsigned int)irq39, 0x08, 0x8E);
        CreateGate(40, (unsigned int)irq40, 0x08, 0x8E);
        CreateGate(41, (unsigned int)irq41, 0x08, 0x8E);
        CreateGate(42, (unsigned int)irq42, 0x08, 0x8E);
        CreateGate(43, (unsigned int)irq43, 0x08, 0x8E);
        CreateGate(44, (unsigned int)irq44, 0x08, 0x8E);
        CreateGate(45, (unsigned int)irq45, 0x08, 0x8E);
        CreateGate(46, (unsigned int)irq46, 0x08, 0x8E);
        CreateGate(47, (unsigned int)irq47, 0x08, 0x8E);

    }

    void CreateGate(unsigned char num, unsigned int base, unsigned char sel, unsigned char flags)
    {
        entries[num].handler_low = base & 0xFFFF;
        entries[num].handler_high = (base >> 16) & 0xFFFF;

        entries[num].segsel = sel;
        entries[num].zero = 0;
        // We must uncomment the OR below when we get to using user-mode.
        // It sets the interrupt gate's privilege level to 3.
        entries[num].config = flags /* | 0x60 */;
    }

    void RemapPIC()
    {
        outb(0x20, 0x11);
        outb(0xA0, 0x11);
        outb(0x21, 0x20);
        outb(0xA1, 0x28);
        outb(0x21, 0x04);
        outb(0xA1, 0x02);
        outb(0x21, 0x01);
        outb(0xA1, 0x01);
        outb(0x21, 0x0);
        outb(0xA1, 0x0);
    }

    void InstallHandler(int interruptNumber, void (*handler)(Registers *r))
    {
        irq_routines[interruptNumber] = handler;
    }

    void UninstallHandler(int interruptNumber)
    {
        irq_routines[interruptNumber] = 0;
    }

}