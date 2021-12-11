#include "Timer.h"
#include <stdio.h>
#include "arch/i386/InterruptRequestManager.h"

namespace Kernel
{
    int Timer::ticks = 0;
    Timer::Timer()
    {
        InterruptRequestManager::InstallHandler(0, TickHandler);
    }

    void Timer::TickHandler(Registers *registers)
    {
        /* Increment our 'tick count' */
        ticks++;

        /* Every 18 clocks (approximately 1 second), we will
        *  display a message on the screen */
        if (ticks % 18 == 0)
        {
            printf("One second has passed\n");
        }
    }

}