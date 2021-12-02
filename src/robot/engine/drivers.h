#pragma once

#include <common/define/hardware.h>
#include <mbed.h>

#define NUM_DRIVERS 5

namespace drivers {
    extern SPI drivers;
    extern DigitalOut drivers_out[NUM_DRIVERS];

    void init();
}  // namespace drivers