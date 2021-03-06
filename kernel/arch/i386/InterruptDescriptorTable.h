#pragma once

#define IDT_NUM_ENTRIES 256

namespace Kernel
{
    struct IDTGate
    {
        unsigned short handler_low;
        unsigned short segsel;
        unsigned char zero;
        unsigned char config;
        unsigned short handler_high;
    } __attribute__((packed));

    struct IDTPointer
    {
        unsigned short limit;
        unsigned int base;
    } __attribute__((packed));

    struct Registers
    {
        unsigned int gs, fs, es, ds;                         /* pushed the segs last */
        unsigned int edi, esi, ebp, esp, ebx, edx, ecx, eax; // Pushed by pusha.
        unsigned int int_no, err_code;                       // Interrupt number and error code (if applicable)
        unsigned int eip, cs, eflags, useresp, ss;           // Pushed by the processor automatically.
    };

    void Init_IDT();
    void CreateGate(unsigned char num, unsigned int base, unsigned char sel, unsigned char flags);
    void RemapPIC();
    void InstallHandler(int interruptNumber, void (*handler)(Registers *r));
    void UninstallHandler(int interruptNumber);
}