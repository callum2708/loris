#pragma once

#include <stddef.h>
#include <stdint.h>

namespace Kernel
{
    class TTY
    {
    public:
        TTY();
        void Initialise(void);
        void PutChar(char c);
        void Write(const char *data, size_t size);
        void WriteString(const char* data);
        void SetColor(uint8_t color);
        void PutEntryAt(unsigned char c, uint8_t color, size_t x, size_t y);
    };
}