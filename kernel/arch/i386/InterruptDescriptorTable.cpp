#include "InterruptDescriptorTable.h"
#include <stdio.h>
#include <string.h>

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

extern "C" void load_idt(unsigned int idtPointer);

extern "C" void isr_handler(Kernel::Registers* regs)
{
    printf("recieved interrupt");
    while(true);
}

Kernel::IDTPointer idtPointer;
Kernel::IDTGate entries[IDT_NUM_ENTRIES];

namespace Kernel
{
    InterruptDescriptorTable::InterruptDescriptorTable()
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
