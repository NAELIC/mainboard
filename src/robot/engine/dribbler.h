#pragma once

#include <mbed.h>
#include <swo.h>

#include "drivers.h"

typedef struct
{
    uint32_t targetSpeed;
    // int16_t pwm;
    uint32_t padding1;
    uint8_t padding2;
} __attribute__((packed)) dribbler_packet_set;

typedef struct {
    uint8_t status;
    uint32_t speed;
    // int16_t pwm;
    // uint32_t enc_cnt;
} __attribute__((packed)) dribbler_packet_ans;

namespace dribbler
{
    void init();
    void set_speed(uint32_t speed);

//     void launch();
}