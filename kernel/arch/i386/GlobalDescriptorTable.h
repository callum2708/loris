#pragma once

#define PL3 0x3
#define PL0 0x0

#define GDT_NUM_ENTRIES 3

namespace Kernel
{
    struct GlobalDescriptorTableEntry
    {
        unsigned short limit_low;  /* The lower 16 bits of the limit */
        unsigned short base_low;   /* The lower 16 bits of the base */
        unsigned char base_mid;    /* The next 8 bits of the base */
        unsigned char access;      /* Contains access flags */
        unsigned char granularity; /* Specify granularity, and 4 bits of limit */
        unsigned char base_high;   /* The last 8 bits of the base; */
    } __attribute__((packed));     /* It needs to be packed like this, 64 bits */

    struct GlobalDescriptorTablePointer
    {
        unsigned short limit; /* Size of gdt table in bytes*/
        unsigned int base;    /* Address to the first gdt entry */
    } __attribute__((packed));

    class GlobalDescriptorTable
    {
    private:
        void CreateEntry(int num, unsigned int base, unsigned int limit, unsigned char access, unsigned char gran);
        GlobalDescriptorTableEntry gdt_entries[GDT_NUM_ENTRIES];
    public:
        GlobalDescriptorTable();
    };
}
