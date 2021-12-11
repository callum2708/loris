#include "arch/i386/InterruptRequestManager.h"

namespace Kernel
{
    class Timer
    {
        public:
            Timer();
            static void TickHandler(Registers* registers);
        private:
            static int ticks;
    };
}