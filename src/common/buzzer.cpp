#include "buzzer.h"

// Config
PwmOut buzzer_pin(BUZZER_PIN);
Timer millis;

// Status
static struct buzzer_note *note_now;
static struct buzzer_note *melody;
static struct buzzer_note *melody_repeat;
static int melody_st;

void buzzer_init()
{
  melody = NULL;
  buzzer_pin = 0.0;
  millis.start(); 
  wait_us(100);
}

void buzzer_play_note(int note)
{
  if (note == 0)
  {
    buzzer_pin = 0.0;
  }
  else
  {
    buzzer_pin.period(float(1.0 / note)); 
    buzzer_pin = BUZZER_POWER;                     
  }
}

static void buzzer_enter(struct buzzer_note *note)
{
  buzzer_play_note(note->freq);
  melody = note;
  melody_st = millis.read_ms(); 
  
  if (note->freq == 0 && note->duration == 0)
  {
    if (melody_repeat != NULL)
    {
      buzzer_enter(melody_repeat);
    }
    else
    {
      melody = NULL;
    }
  }
}

void buzzer_play(unsigned int melody_num, bool repeat)
{
  struct buzzer_note *to_play = NULL;

  if (melody_num == MELODY_BOOT)
  {
    // to_play = &melody_boot[0];
    to_play = &chord_boot[0];
  }
  else if (melody_num == MELODY_ALERT)
  {
    to_play = &melody_alert[0];
  }
  else if (melody_num == MELODY_ALERT_FAST)
  {
    to_play = &melody_alert_fast[0];
  }
  else if (melody_num == MELODY_WARNING)
  {
    to_play = &melody_warning[0];
  }
  else if (melody_num == MELODY_BEGIN)
  {
    to_play = &melody_begin[0];
  }
  else if (melody_num == MELODY_END)
  {
    to_play = &melody_end[0];
  }
  else if (melody_num == MELODY_CUSTOM)
  {
    to_play = &melody_custom[0];
  }
  else if (melody_num == MELODY_BOOT_DEV)
  {
    to_play = &chord_boot_dev[0];
  }
  else
  {
    melody = NULL;
  }

  if (to_play)
  {
    melody_repeat = repeat ? to_play : NULL;
    buzzer_enter(to_play);
  }
}

// int instant_melody ;
void buzzer_tick()
{
  if (melody != NULL)
  {
    if (millis.read_ms() - melody_st > melody->duration)
    {
      buzzer_enter(melody + 1);
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
  while (buzzer_is_playing())
  {
    buzzer_tick();
    // Watchdog::get_instance().kick();
  }
}

void buzzer_beep(unsigned int freq, unsigned int duration)
{
  melody_custom[0].freq = freq;
  melody_custom[0].duration = duration;
  buzzer_play(MELODY_CUSTOM);
}

//SHELL Comand
SHELL_COMMAND(play, "Play a melody")
{
  int melnum = atoi(argv[0]);
  shell_println("Playing melody ");
  shell_println(melnum);
  buzzer_play(melnum);
}

SHELL_COMMAND(beep, "Plays a beep")
{
  if (argc == 2)
  {
    buzzer_beep(atoi(argv[0]), atoi(argv[1]));
  }
  else if (argc == 1)
  {
    buzzer_beep(atoi(argv[0]), 1000);
  }
  else
  {
    shell_println("Usage: beep freq [duration]");
    buzzer_beep(494, 1000);
  }
}
