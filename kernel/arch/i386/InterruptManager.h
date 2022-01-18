#pragma once

namespace Kernel
{
    class InterruptHandler;
}

extern Kernel::InterruptHandler* irq_routines[16];

void InstallHandler(int interruptNumber, Kernel::InterruptHandler *handler);

void UninstallHandler(int interruptNumber);