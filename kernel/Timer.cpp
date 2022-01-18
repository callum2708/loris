#include "Timer.h"
#include <stdio.h>
#include "io.h"
#include <Task.h>

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
        switch_task();
    }

    void Timer::TimerPhase(int frequency)
    {
        int divisor = 1193180 / frequency;     /* Calculate our divisor */
        outb(0x43, 0x36);           /* Set our command byte 0x36 */
        outb(0x40, divisor & 0xFF); /* Set low byte of divisor */
        outb(0x40, divisor >> 8);   /* Set high byte of divisor */
    }

}