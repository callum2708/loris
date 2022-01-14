#include <stdio.h>

extern "C"  [[noreturn]] void __cxa_pure_virtual()
{
    printf("cxa_reached!!");
    while(true);
}
