#include "arch/i386/InterruptDescriptorTable.h"

namespace Kernel
{
    class Timer
    {
        public:
            Timer(int frequency);
            static void TickHandler(Registers* registers);
        private:
            static int ticks;

            void TimerPhase(int frequency);
    };
}