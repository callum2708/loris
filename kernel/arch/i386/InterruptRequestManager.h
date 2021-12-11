#pragma once

#include "InterruptDescriptorTable.h"

namespace Kernel
{
    class InterruptRequestManager
    {
    private:
        static InterruptDescriptorTable *m_idt;

    public:
        static void Initialise(InterruptDescriptorTable *idt);
        static void InstallHandler(int interruptNumber, void (*handler)(Registers *r));
        static void UninstallHandler(int interruptNumber);
        static void RemapPIC();
    };
}