#pragma once

#include <mbed.h>
#include <shell.h>

#include "hardware.h"
#include "melody.h"

#define BUZZER_POWER 0.5

namespace buzzer
{
    typedef struct
    {
        uint8_t nb; /* Melody number */
    } message_t;

    static Queue<message_t, 16> queue;

    void launch();
}