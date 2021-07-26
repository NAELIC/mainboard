#pragma once

#include <mbed.h>
#include <shell.h>
#include <swo.h>

#include <common/hardware.h>

namespace ir
{
    static double value;
    void launch();
    bool present();
}