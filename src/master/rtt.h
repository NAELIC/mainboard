#pragma once

#include <mbed.h>
#include <Stream.h>
#include <mutex>

#ifndef BLOCKING_MODE
#define BLOCKING_MODE TRUE
#endif

namespace naelic
{
    class RTT : public Stream
    {
    protected:
        int _putc(int c);
        int _getc();
        // void lock() override;
        void unlock() override;
        // PlatformMutex mutex;
    };

} // namespace naelic
