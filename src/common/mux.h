#pragma once

#include <mbed.h>
#include <shell.h>
#include <common/define/hardware.h>

unsigned short mux_sample(int addr);
bool get_hall(int addr);
