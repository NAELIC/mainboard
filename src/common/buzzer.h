#pragma once

#include <mbed.h>
#include <shell.h>

#include "hardware.h"
#include "melody.h"

#define BUZZER_POWER 0.5

namespace buzzer
{
    typedef struct
    {
        uint8_t nb; /* Melody number */
    } message_t;

    static Queue<message_t, 16> queue;

    void launch();
}

// /**
//  * Initializes the buzzer
//  */
// void buzzer_init();

// /**
//  * Plays a melody
//  * @param melody The melody id (MELODY_*)
//  * @param repeat Does the melody repeats continuously?
//  */
// void buzzer_play(unsigned int melody, bool repeat = false);

// /**
//  * Stops playing any sound
//  */
// void buzzer_stop();

// /**
//  * Ticking the buzzer
//  */
// void buzzer_tick();

// /**
//  * Is the buzzer plaing?
//  */
// bool buzzer_is_playing();

// /**
//  * Wait the end of the play
//  */
// void buzzer_wait_play();

// /**
//  * Plays a beep
//  * @param freq     The frequency (Hz)
//  * @param duration The duration (ms)
//  */
// void buzzer_beep(unsigned int freq, unsigned int duration);

// void buzzer_play_note(int note);