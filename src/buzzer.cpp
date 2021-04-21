
#include "buzzer.h"
#include "mbed.h"
#include "shell.h"
#include "hardware.h"



struct buzzer_note {    //definition of the structure who had for name "buzzer_note"
    unsigned int freq;
    unsigned int duration;
};

// Config
PwmOut buzzer_pin(BUZZER_PIN); // Set the PIN we name BUZZER_PIN in hardware.h as a Pwmout


// Partitions


// static struct buzzer_note beethoven_boot[] = {
//     {440, 200},
//     {0, 10},
//     {440, 200},
//     {0, 10},
//     {440, 200},
//     {0, 10},
//     {392, 600},
//     {0, 10},
// //    {415, 300/2},
// //    {415, 300/2},
// //    {415, 300/2},
// //    {370, 160/2},
//     {0, 0}
// };


static struct buzzer_note rickroll[] = {

{B4b, 100},
{0, 20},
{C5, 100},
{0, 20},
{D5b, 100},
{0, 20},
{B4b, 100},
{0, 20},
{F5, 212},
{0, 20},
{F5, 212},
{0, 20},
{E5b, 312},
{0, 20},

{B4b, 100},
{0, 20},
{C5, 100},
{0, 20},
{D5b, 100},
{0, 20},
{B4b, 100},
{0, 20},
{E5b, 212},
{0, 20},
{E5b, 212},
{0, 20},
{D5b, 312},
{0, 20},


{B4b, 100},
{0, 20},
{C5, 100},
{0, 20},
{D5b, 100},
{0, 20},
{B4b, 100},
{0, 20},

{D5b, 212},
{0, 20},
{E5b, 212},
{0, 20},
{C5, 212},
{0, 212},
{A5b, 112},
{0, 20},

{E5b, 312},
{0, 20},
{D5b, 624},
{0, 20},
{0, 0}
};

static struct buzzer_note chord_boot[] = {
{C5, 50},
{E5, 50},
{G5, 50},
{C6, 200},
{0, 0}
};

// static struct buzzer_note chord_boot_dev[] = {
//   {C5, 50},
//   {E5b, 50},
//   {G5, 50},
//   {C6, 200},
//   {0, 0}
// };


static struct buzzer_note chord_boot_dev[] = {
{C6, 50},
{G5, 50},
{E5, 50},
{C5, 200},
{0, 0}
};


// static struct buzzer_note melody_boot[] = {
//     {523, 200/2},
//     {659, 350/2},
//     {523, 200/2},
//     {698, 300/2},
//     {659, 160/2},
//     {0, 0}
// };

static struct buzzer_note melody_alert[] = {
{2000, 200},
{200, 200},
{2000, 200},
{200, 200},
{0, 0}
};

static struct buzzer_note melody_alert_fast[] = {
{2000, 100},
{200, 100},
{2000, 100},
{200, 100},
{2000, 100},
{200, 100},
{0, 0}
};

static struct buzzer_note melody_warning[] = {
{800, 200},
{400, 200},
{200, 0},
{200, 400},
{0, 0}
};

static struct buzzer_note melody_begin[] = {
{800, 200},
{1000, 200},
{0, 0},
};

static struct buzzer_note melody_end[] = {
{1000, 200},
{800, 200},
{0, 0},
};

static struct buzzer_note melody_custom[] = {
{0, 0},
{0, 0}
};

static struct buzzer_note melody_assert[] = {
{2000, 200},
{1700, 200},
{1400, 200},
{1100, 200},
{800, 200},
{500, 200},
{200, 200},
{0,0}
};

// Status
static struct buzzer_note *note_now;

static struct buzzer_note *melody;
static struct buzzer_note *melody_repeat;
static int melody_st;

void buzzer_init()
{
    melody = NULL;
    buzzer_pin = 0.0; //No sound

}

void buzzer_play_note(unsigned int note)
{
    buzzer_pin.period( float(1.0 / note)); //set the period on the buzzer

    if (note == 0) { //if the melody is end

        buzzer_pin = 0.0; //No sound
    } 
    else {
        buzzer_pin.period(float(1.0 / note)); //set the eriod on the buzzer
        buzzer_pin = 0.5 ;// Activation of soud
    }
}

static void buzzer_enter(struct buzzer_note *note)
{
    buzzer_play_note(note->freq);
    melody = note;
    melody_st = Kernel::Clock::now().time_since_epoch().count();

    if (note->freq == 0 && note->duration == 0) {
        if (melody_repeat != NULL) {
          buzzer_enter(melody_repeat);
        } else {
          melody = NULL;
        }
    }

       shell_println("Coucou ");
}

void buzzer_play(unsigned int melody_num, bool repeat)
{  
  struct buzzer_note *to_play = NULL;

  if (melody_num == MELODY_BOOT) {
    // to_play = &melody_boot[0];
    to_play = &chord_boot[0];

  } else if (melody_num == MELODY_ALERT) {
    to_play = &melody_alert[0];
  } else if (melody_num == MELODY_ALERT_FAST) {
    to_play = &melody_alert_fast[0];
  } else if (melody_num == MELODY_WARNING) {
    to_play = &melody_warning[0];
  } else if (melody_num == MELODY_BEETHOVEN) {
    to_play = &chord_boot[0];
  } else if (melody_num == MELODY_BEGIN) {
    to_play = &melody_begin[0];
  } else if (melody_num == MELODY_END) {
    to_play = &melody_end[0];
  } else if (melody_num == MELODY_CUSTOM) {
    to_play = &melody_custom[0];
  }else if (melody_num == MELODY_BOOT_DEV) {
    to_play = &chord_boot_dev[0];
  }else if (melody_num == RICKROLL) {
    to_play = &rickroll[0];

  } else {
    melody = NULL;
  }

  if (to_play) {
    melody_repeat = repeat ? to_play : NULL;
    buzzer_enter(to_play);
  }
}

void buzzer_tick()
{
    if (melody != NULL) {
        if (Kernel::Clock::now().time_since_epoch().count()-melody_st > melody->duration) {
            buzzer_enter(melody+1);
        }
    }
}

void buzzer_stop()
{
    buzzer_play_note(0);
    melody = NULL;
    melody_repeat = NULL;
}

bool buzzer_is_playing()
{
    return melody != NULL;
}

void buzzer_wait_play()
{
    while (buzzer_is_playing()) {
        buzzer_tick();
        Watchdog::get_instance().kick();
    }
}

void buzzer_beep(unsigned int freq, unsigned int duration)
{
    melody_custom[0].freq = freq;
    melody_custom[0].duration = duration;
    buzzer_play(MELODY_CUSTOM);
}



SHELL_COMMAND(play, "Play a melody")
{
    int melnum = atoi(argv[0]);
    shell_println("Playing melody ");
    shell_println(melnum);
    shell_println();
    buzzer_play(melnum);
}

SHELL_COMMAND(beep, "Plays a beep")
{
    if (argc == 2) {
        buzzer_beep(atoi(argv[0]), atoi(argv[1]));
    } else if (argc == 1) {
        buzzer_beep(atoi(argv[0]), 1000);
    } else {
        shell_println("Usage: beep freq [duration]");
        buzzer_beep(494, 1000);
    }
}

