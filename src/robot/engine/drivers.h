#pragma once

#include <common/buzzer/buzzer.h>
#include <common/define/hardware.h>
#include <USBSerial.h>
#include <mbed.h>
#include <shell.h>
#include "errors.h"

#include <swo.h>

#define DRIVER_PACKET_SET 0x00
struct driver_packet_set
{
    bool enable;
    float targetSpeed;
    int16_t pwm;
    uint32_t padding1;
    uint8_t padding2;
} __attribute__((packed));

struct driver_packet_ans
{
    uint8_t status;
    float speed;
    int16_t pwm;
    uint32_t enc_cnt;
} __attribute__((packed));

extern struct driver_packet_ans driver_answers[5];

namespace drivers
{
    typedef struct
    {
        float x_speed;
        float y_speed;
        float t_speed;
    } engine_msg;

    static Queue<engine_msg, 2> queue_drivers;

    void launch();
}