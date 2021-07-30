#pragma once

#include <mbed.h>
#include <Stream.h>
#include <RTT/SEGGER_RTT.h>

namespace naelic
{
    class RTT : public Stream
    {
    protected:
        int _putc(int c);
        int _getc();
        void unlock() override;
    };

} // namespace naelic