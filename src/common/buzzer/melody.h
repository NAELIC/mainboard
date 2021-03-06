#pragma once

#include <vector>

typedef struct
{
    uint16_t freq;
    unsigned int duration;
} buzzer_note;

/**
 * Melodic enumerations.
 */
#define MELODY_BOOT 0 // Robot start
#define MELODY_ALERT 1
#define MELODY_ALERT_FAST 2
#define MELODY_WARNING 3
#define MELODY_BEGIN 4 // Start motors
#define MELODY_END 5   // End motors
#define MELODY_CUSTOM 6
#define MELODY_BOOT_DEV 7
#define MELODY_ASSERT 8

/**
 * Notes constant.
 */
#define A5 880
#define A5b 830
#define C2 130
#define C4 261
#define B4 494
#define B4b 466
#define C5 523
#define C5d 554
#define D5 587
#define D5b 554
#define E5 659
#define E5b 622
#define F5 698
#define G5 784
#define C6 1047
#define C7 2093
#define B5b 932

namespace melody
{
    /**
 * Melody
 */
    static std::vector<buzzer_note> boot = {
        {C5, 50},
        {E5, 50},
        {G5, 50},
        {C6, 200}};

    static std::vector<buzzer_note> boot_dev = {
        {C6, 50},
        {G5, 50},
        {E5, 50},
        {C5, 200}};

    static std::vector<buzzer_note> alert = {
        {2000, 200},
        {200, 200},
        {2000, 200},
        {200, 200}};

    static std::vector<buzzer_note> alert_fast = {
        {2000, 100},
        {200, 100},
        {2000, 100},
        {200, 100},
        {2000, 100},
        {200, 100}};

    static std::vector<buzzer_note> warning = {
        {800, 200},
        {400, 200},
        {200, 400}};

    static std::vector<buzzer_note> begin_motor = {
        {800, 200},
        {1000, 200}};

    static std::vector<buzzer_note> end_motor = {
        {1000, 200},
        {800, 200}};

    static std::vector<buzzer_note> assert = {
        {2000, 200},
        {1700, 200},
        {1400, 200},
        {1100, 200},
        {800, 200},
        {500, 200},
        {200, 200}};

    static std::vector<buzzer_note> custom = {{0, 0}};
}
