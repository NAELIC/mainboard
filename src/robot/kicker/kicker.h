#pragma once

#include <common/buzzer/buzzer.h>
#include <common/define/hardware.h>
#include <common/mux.h>
#include <USBSerial.h>
#include <stdlib.h>
#include <mbed.h>
#include <shell.h>
#include "errors.h"

#include <swo.h>

namespace kicker
{
    typedef struct
    {
        int id;
        float power;
    } kicker_msg;

    static Queue<kicker_msg, 2> queue_drivers;

    void launch();
}