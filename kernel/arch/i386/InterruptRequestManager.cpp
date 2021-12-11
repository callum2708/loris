#include "InterruptRequestManager.h"
#include "InterruptDescriptorTable.h"

#include "../../io.h"
#include <stdio.h>

extern "C" void irq0();
extern "C" void irq1();
extern "C" void irq2();
extern "C" void irq3();
extern "C" void irq4();
extern "C" void irq5();
extern "C" void irq6();
extern "C" void irq7();
extern "C" void irq8();
extern "C" void irq9();
extern "C" void irq10();
extern "C" void irq11();
extern "C" void irq12();
extern "C" void irq13();
extern "C" void irq14();
extern "C" void irq15();

namespace Kernel
{
    void *irq_routines[16] = {0};
    InterruptDescriptorTable* InterruptRequestManager::m_idt = nullptr;

    extern "C" void irq_handler(Registers *registers)
    {
        printf("irq!!!!");
        /* This is a blank function pointer */
        void (*handler)(struct Registers * r);

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

    void InterruptRequestManager::Initialise(InterruptDescriptorTable* idt)
    {
        m_idt = idt;
        InterruptRequestManager::RemapPIC();

        m_idt->CreateGate(32, (unsigned int)irq0, 0x08, 0x8E);
        m_idt->CreateGate(33, (unsigned int)irq1, 0x08, 0x8E);
        m_idt->CreateGate(34, (unsigned int)irq2, 0x08, 0x8E);
        m_idt->CreateGate(35, (unsigned int)irq3, 0x08, 0x8E);
        m_idt->CreateGate(36, (unsigned int)irq4, 0x08, 0x8E);
        m_idt->CreateGate(37, (unsigned int)irq5, 0x08, 0x8E);
        m_idt->CreateGate(38, (unsigned int)irq6, 0x08, 0x8E);
        m_idt->CreateGate(39, (unsigned int)irq7, 0x08, 0x8E);
        m_idt->CreateGate(40, (unsigned int)irq8, 0x08, 0x8E);
        m_idt->CreateGate(41, (unsigned int)irq9, 0x08, 0x8E);
        m_idt->CreateGate(42, (unsigned int)irq10, 0x08, 0x8E);
        m_idt->CreateGate(43, (unsigned int)irq11, 0x08, 0x8E);
        m_idt->CreateGate(44, (unsigned int)irq12, 0x08, 0x8E);
        m_idt->CreateGate(45, (unsigned int)irq13, 0x08, 0x8E);
        m_idt->CreateGate(46, (unsigned int)irq14, 0x08, 0x8E);
        m_idt->CreateGate(47, (unsigned int)irq15, 0x08, 0x8E);
    }

    void InterruptRequestManager::InstallHandler(int interruptNumber, void (*handler)(struct registers *r))
    {
        irq_routines[interruptNumber] = handler;
    }

    void InterruptRequestManager::UninstallHandler(int interruptNumber)
    {
        irq_routines[interruptNumber] = 0;
    }

    void InterruptRequestManager::RemapPIC()
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
}