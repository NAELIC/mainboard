#pragma once

#include <USBSerial.h>
#include <common/buzzer/buzzer.h>
#include <common/define/hardware.h>
#include <mbed.h>
#include <shell.h>
#include <swo.h>

#include "drivers.h"

#define DRIVER_PACKET_SET 0x00
#define NUM_OF_TRINAMIC 4

// For debug, max speed is 1m/s
#define DEBUG_MAX_TRINAMIC_SPEED 8928

namespace trinamics {
    void tmc4671_init(uint8_t motor);
    void start_motor(uint8_t motor);
    void stop_motor(uint8_t motor, bool freewheel = false);
    void set_speed(int id_motor, float target);

    void init();

    bool ping(int motor);

}  // namespace trinamic