#pragma once

#include <mbed.h>
#include "drivers.h"

// #include <common/buzzer/buzzer.h>
// #include <common/define/hardware.h>
// #include <USBSerial.h>
// #include <shell.h>
// #include "errors.h"

#include <swo.h>



// #define DRIBBLER_PACKET_SET 0x00
typedef struct
{
    uint32_t targetSpeed;
    // int16_t pwm;
    // uint32_t padding1;
    // uint8_t padding2;
} __attribute__((packed)) dribbler_packet_set;

typedef struct {
    uint8_t status;
    uint32_t speed;
    // int16_t pwm;
    // uint32_t enc_cnt;
} __attribute__((packed)) dribbler_packet_ans;

namespace dribbler
{

    void set_speed(uint32_t speed);

//     void launch();
}