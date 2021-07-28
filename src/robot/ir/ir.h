#pragma once

#include <mbed.h>
#include <shell.h>
#include <swo.h>

#include <common/hardware.h>

namespace ir
{
    extern float value;
    void compute();
    bool present();
}