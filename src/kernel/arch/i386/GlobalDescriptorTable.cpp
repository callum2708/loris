#include "GlobalDescriptorTable.h"

#define SEGMENT_BASE 0
#define SEGMENT_LIMIT 0xFFFFF

#define CODE_RX_TYPE 0xA
#define DATA_RW_TYPE 0x2

extern void gdt_load_and_set(unsigned int) __asm__ ("gdt_load_and_set"); 

namespace Kernel
{

    GlobalDescriptorTable::GlobalDescriptorTable()
    {
        GlobalDescriptorTablePointer gdt_ptr;
        gdt_ptr.limit = sizeof(GlobalDescriptorTableEntry) * GDT_NUM_ENTRIES;
        gdt_ptr.base = (unsigned int)&gdt_entries;

        /* the null entry */
        CreateEntry(0, 0, 0);
        /* kernel mode code segment */
        CreateEntry(1, PL0, CODE_RX_TYPE);
        /* kernel mode data segment */
        CreateEntry(2, PL0, DATA_RW_TYPE);
        /* user mode code segment */
        CreateEntry(3, PL3, CODE_RX_TYPE);
        /* user mode data segment */
        CreateEntry(4, PL3, DATA_RW_TYPE);
        gdt_load_and_set((unsigned int)&gdt_ptr);
    }

    void GlobalDescriptorTable::CreateEntry(unsigned int n, unsigned char pl, unsigned char type)
    {
        gdt_entries[n].base_low = (SEGMENT_BASE & 0xFFFF);
        gdt_entries[n].base_mid = (SEGMENT_BASE >> 16) & 0xFF;
        gdt_entries[n].base_high = (SEGMENT_BASE >> 24) & 0xFF;

        gdt_entries[n].limit_low = (SEGMENT_LIMIT & 0xFFFF);

        /*
     * name | value | size | desc
     * ---------------------------
     * G    |     1 |    1 | granularity, size of segment unit, 1 = 4kB
     * D/B  |     1 |    1 | size of operation size, 0 = 16 bits, 1 = 32 bits
     * L    |     0 |    1 | 1 = 64 bit code
     * AVL  |     0 |    1 | "available for use by system software"
     * LIM  |   0xF |    4 | the four highest bits of segment limit
     */
        gdt_entries[n].granularity |= (0x01 << 7) | (0x01 << 6) | 0x0F;

        /*
     * name | value | size | desc
     * ---------------------------
     * P    |     1 |    1 | segment present in memory
     * DPL  |    pl |    2 | privilege level
     * S    |     1 |    1 | descriptor type, 0 = system, 1 = code or data
     * Type |  type |    4 | segment type, how the segment can be accessed
     */
        gdt_entries[n].access =
            (0x01 << 7) | ((pl & 0x03) << 5) | (0x01 << 4) | (type & 0x0F);
    }
}