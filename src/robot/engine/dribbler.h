#pragma once

#include <common/buzzer/buzzer.h>
#include <common/define/hardware.h>
#include <USBSerial.h>
#include <mbed.h>
#include <shell.h>
#include "errors.h"

#include <swo.h>

#define DRIBBLER_PACKET_SET 0x00
struct dribbler_packet_set
{
    bool enable;
    float targetSpeed;
    int16_t pwm;
    uint32_t padding1;
    uint8_t padding2;
} __attribute__((packed));

struct drivbbler_packet_ans
{
    uint8_t status;
    float speed;
    int16_t pwm;
    uint32_t enc_cnt;
} __attribute__((packed));

extern struct dribbler_packet_ans dribbler_answers;

namespace dribbler
{
    typedef struct
    {
        float speed;
    } dribbler_msg;

    static Queue<dribbler_msg, 2> queue_dribbler;

    void launch();
}