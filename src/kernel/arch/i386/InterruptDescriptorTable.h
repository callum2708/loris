#pragma once

#define IDT_NUM_ENTRIES 256

namespace Kernel
{
    class InterruptDescriptorTable
    {
    public:
        InterruptDescriptorTable();

    private:
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

        IDTGate entries[IDT_NUM_ENTRIES];

        void CreateGate(unsigned char num, unsigned int base, unsigned char sel, unsigned char flags);

    };
}