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

/**
 * @brief play a melody
 * 
 * @param melody_num 
 */
    void play(uint8_t melody_num);

/**
 * @brief plays a beep of a certain frequency for a specified time
 * 
 * @param freq 
 * @param duration 
 */
    void beep(unsigned int freq, unsigned int duration);

/**
 * @brief main process of buzzer thread 
 * 
 */
    void launch();
}  // namespace buzzer