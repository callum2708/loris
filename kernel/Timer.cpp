#include "Timer.h"
#include <stdio.h>
#include "io.h"

namespace Kernel
{
    int Timer::ticks = 0;
    Timer::Timer(int frequency)
    {
        TimerPhase(frequency);
        InstallHandler(0, TickHandler);
    }

    void Timer::TickHandler(Registers *registers)
    {
        /* Increment our 'tick count' */
        ticks++;

        /* Every 18 clocks (approximately 1 second), we will
        *  display a message on the screen */
        if (ticks % 100 == 0)
        {
            printf("One second has passed\n");
        }
    }

    void Timer::TimerPhase(int frequency)
    {
        int divisor = 1193180 / frequency;     /* Calculate our divisor */
        outb(0x43, 0x36);           /* Set our command byte 0x36 */
        outb(0x40, divisor & 0xFF); /* Set low byte of divisor */
        outb(0x40, divisor >> 8);   /* Set high byte of divisor */
    }

}