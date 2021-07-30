#pragma once

#include <mbed.h>
#include <shell.h>
#include <swo.h>

#include <common/define/hardware.h>
#include <common/mux.h>

namespace voltage {
    void compute();
    float value();
}