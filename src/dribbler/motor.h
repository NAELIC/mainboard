#pragma once

#include <mbed.h>
#include "define/hardware.h"


/*
* The project needs to be improved.
* We need to added : 
*/

namespace motor {
    void init();
    void tick();
    void motor_hall_set(bool enable, int value);
    void hall_irq();
}