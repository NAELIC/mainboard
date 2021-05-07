#ifndef _DRIVERS_H
#define _DRIVERS_H

#include <stdint.h>
#include <assert.h>
#define cstatic_assert _Static_assert

#define DRIVER_PACKET_SET   0x00
struct driver_packet_set {
    bool enable;
    float targetSpeed;
    int16_t pwm;
    uint32_t padding1; // See the next static_assert
    uint8_t padding2; // See the next static_assert
} __attribute__((packed));

struct driver_packet_ans {
    uint8_t status;
    float speed;
    int16_t pwm;
    uint32_t enc_cnt;
} __attribute__((packed));

// TODO : DEPRECATED: TO REMOVE
#define DRIVER_PACKET_PARAMS   0x01
struct driver_packet_params {
    float kp;
    float ki;
    float kd;
} __attribute__((packed));



extern struct driver_packet_ans driver_answers[5];

/**
 * Initializes the drivers
 */
void drivers_init();

/**
 * Set the speed of the nth wheel [turn/s]
 */
struct driver_packet_ans drivers_set(int index, bool enable, float target, int16_t pwm=0);
void drivers_set_safe(int index, bool enable, float target, int16_t pwm=0);
void drivers_set_params(float kp, float ki, float kd);

/**
 * Tick the drivers
 */
void drivers_tick();

/**
 * Is the driver alive ?
 */
int drivers_ping(int index);
bool drivers_is_all_ok();
void drivers_diagnostic();

#endif
