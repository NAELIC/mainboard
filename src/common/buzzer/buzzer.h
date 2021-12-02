#pragma once

#include <common/define/hardware.h>
#include <mbed.h>
#include <shell.h>

#include "melody.h"

#define BUZZER_POWER 0.5

namespace buzzer {
    typedef struct {
        uint8_t nb; /* Melody number */
    } message_t;

    static Queue<message_t, 5> queue;

    void play(uint8_t melody_num);
    void beep(unsigned int freq, unsigned int duration);

    void launch();
}  // namespace buzzer