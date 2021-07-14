#pragma once

struct packet_master {
    // #define ACTION_ON      (1<<0)   // The robot should be on (else everything is stopped)
    //                                 // Kicking, transition from 0 to 1 trigger kick if IR is present
    // #define ACTION_KICK1   (1<<1)   // Kick on kicker 1 - chip
    // #define ACTION_KICK2   (1<<2)   // Kick on kicker 2 - normal
    // #define ACTION_DRIBBLE (1<<3)   // Enable/disable the dribbler
    // #define ACTION_CHARGE  (1<<5)   // Enable/disable the capacitor charge
    // #define ACTION_TARE_ODOM (1<<7)   // Tare Odometry
    // uint8_t actions;

    int16_t x_speed;                // Kinematic orders [mm/s]
    // int16_t y_speed;
    // int16_t t_speed;                // Rotation in [mrad/s]

    // uint8_t kickPower;             // Kick power (this is a duration in [x25 uS])
} __attribute__((packed));

struct packet_robot {
    uint8_t id;

    // #define STATUS_OK           (1<<0)  // The robot is alive and ok
    // #define STATUS_DRIVER_ERR   (1<<1)  // Error with drivers
    // #define STATUS_IR           (1<<2)  // The infrared barrier detects the ball
    // uint8_t status;

    // uint8_t cap_volt;                  // Kick capacitor voltage [V]

    // uint8_t voltage;                  // Battery voltage [8th of V]

    int16_t xpos;                     // Data planned by odometry
    // int16_t ypos;                     // In mm
    // int16_t ang;                      // In rad/10000

} __attribute__((packed));

// #define INSTRUCTION_PARAMS          0x01
// struct packet_params {
//     float kp, ki, kd;               // Servo parameter
// } __attribute__((packed));
