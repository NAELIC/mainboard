#pragma once

#define ACTION_ON (1 << 0)
#define ACTION_KICK1 (1 << 1)
#define ACTION_KICK2 (1 << 2)
#define ACTION_DRIBBLE (1 << 3)
#define ACTION_CHARGE (1 << 5)
#define ACTION_TARE_ODOM (1 << 7)

typedef struct
{
    uint8_t id;
    uint8_t actions;

    int16_t x_speed; // Kinematic orders [mm/s]
    int16_t y_speed;
    int16_t t_speed; // Rotation in [mrad/s]

    uint8_t kickPower; // Kick power (this is a duration in [x25 uS])
} __attribute__((packed)) packet_robot;

#define STATUS_OK (1 << 0)         // The robot is alive and ok
#define STATUS_DRIVER_ERR (1 << 1) // Error with drivers
#define STATUS_IR (1 << 2)         // The infrared barrier detects the ball

// robot -> master
typedef struct
{
    uint8_t id;
    uint8_t status;
    
    uint8_t cap_volt; // Kick capacitor voltage [V]
    uint8_t voltage; // Battery voltage [8th of V]

    int16_t xpos; // Data planned by odometry
    int16_t ypos; // In mm
    int16_t ang;  // In rad/10000

} __attribute__((packed)) packet_status;
