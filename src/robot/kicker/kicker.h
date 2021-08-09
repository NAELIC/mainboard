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

void kicker_init();

void kicker_boost_enable(bool enable);

/**
 * Run a kick, power is [us]
 */
void kicker_kick(int kicker, int power);

/**
 * Ticks the kicker
 */
void kicker_tick();

/**
 * Gets the kick capcitor voltage [V]
 */
float kicker_cap_voltage();


/**
 * Clear the capacitors
 */
void kicker_clear();

bool kicker_is_charging();

bool is_charging();

void kicker_off();