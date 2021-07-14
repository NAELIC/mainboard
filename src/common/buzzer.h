#pragma once

#include <mbed.h>
#include <shell.h>
#include "hardware.h"

#define BUZZER_POWER 0.5

// When the robot starts
#define MELODY_BOOT       0

// When the battery is low
#define MELODY_ALERT      1
#define MELODY_ALERT_FAST 2
// When there is a warning
#define MELODY_WARNING    3
// When we start the ID of the motors
#define MELODY_BEGIN      4
// When we start the ID of the motors
#define MELODY_END        5
// A custom melody used by beep
#define MELODY_CUSTOM     6
#define MELODY_BEETHOVEN     7
#define MELODY_BOOT_DEV       8
#define RICKROLL       9
#define MELODY_ASSERT 10
#define SPAROW 11
#define MARIO_INTRO 12


const int A5  =880;
const int A5b =830;
const int C2  =130;
const int C4  =261;

const int B4  =494;
const int B4b =466;
const int C5  =523;//C note in Hz
const int C5d =554;
const int D5  =587;
const int D5b =554;
const int E5  =659;
const int E5b =622;
const int F5  =698;
const int G5  =784;
const int C6  =1047;
const int C7  =2093;
const int B5b =932;


/**
 * Initializes the buzzer
 */
void buzzer_init();

/**
 * Plays a melody
 * @param melody The melody id (MELODY_*)
 * @param repeat Does the melody repeats continuously?
 */
void buzzer_play(unsigned int melody, bool repeat=false);

/**
 * Stops playing any sound
 */
void buzzer_stop();

/**
 * Ticking the buzzer
 */
void buzzer_tick();

/**
 * Is the buzzer plaing?
 */
bool buzzer_is_playing();

/**
 * Wait the end of the play
 */
void buzzer_wait_play();

/**
 * Plays a beep
 * @param freq     The frequency (Hz)
 * @param duration The duration (ms)
 */
void buzzer_beep(unsigned int freq, unsigned int duration);

void buzzer_play_note(int note);