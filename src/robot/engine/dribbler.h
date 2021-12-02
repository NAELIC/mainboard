#pragma once

#include <common/define/hardware.h>
#include <mbed.h>
#include <shell.h>
#include <swo.h>

// #define DRIBBLER_PACKET_SET 0x00
typedef struct {
    unsigned int targetSpeed;
} __attribute__((packed)) dribbler_packet;

typedef struct {
    uint8_t status;
} __attribute__((packed)) master_packet;

namespace dribbler {
    void set_speed(int speed);
}