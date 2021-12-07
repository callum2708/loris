#include "GlobalDescriptorTable.h"

#define SEGMENT_BASE 0
#define SEGMENT_LIMIT 0xFFFFF

#define CODE_RX_TYPE 0xA
#define DATA_RW_TYPE 0x2

extern "C" void gdt_load_and_set(unsigned int);

namespace Kernel
{

    GlobalDescriptorTable::GlobalDescriptorTable()
    {
        gtdPointer.limit = sizeof(GlobalDescriptorTableEntry) * GDT_NUM_ENTRIES;
        gtdPointer.base = (unsigned int)&gdt_entries;

        CreateEntry(0, 0, 0, 0, 0);                // Null segment
        CreateEntry(1, 0, 0xFFFFFFFF, 0x9A, 0xCF); // Code segment
        CreateEntry(2, 0, 0xFFFFFFFF, 0x92, 0xCF); // Data segment
        CreateEntry(3, 0, 0xFFFFFFFF, 0xFA, 0xCF); // User mode code segment
        CreateEntry(4, 0, 0xFFFFFFFF, 0xF2, 0xCF); // User mode data segment
        gdt_load_and_set((unsigned int)&gtdPointer);
    }

    void GlobalDescriptorTable::CreateEntry(int num, unsigned int base, unsigned int limit, unsigned char access, unsigned char gran)
    {
        gdt_entries[num].base_low = (base & 0xFFFF);
        gdt_entries[num].base_mid = (base >> 16) & 0xFF;
        gdt_entries[num].base_high = (base >> 24) & 0xFF;

        gdt_entries[num].limit_low = (limit & 0xFFFF);
        gdt_entries[num].granularity = (limit >> 16) & 0x0F;

        gdt_entries[num].granularity |= gran & 0xF0;
        gdt_entries[num].access = access;
    }
}