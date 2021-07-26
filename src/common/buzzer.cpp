#include "buzzer.h"

#include <swo.h>
#include <vector>

static naelic::SWO swo;

namespace buzzer
{
  PwmOut pin(BUZZER_PIN);

  std::vector<buzzer_note> find_melody(uint8_t melody_num)
  {
    switch (melody_num)
    {
    case MELODY_BOOT:
      return melody::boot;
    case MELODY_ALERT:
      return melody::alert;
    case MELODY_ALERT_FAST:
      return melody::alert_fast;
    case MELODY_WARNING:
      return melody::warning;
    case MELODY_BEGIN:
      return melody::begin_motor;
    case MELODY_END:
      return melody::end_motor;
    case MELODY_CUSTOM:
      return melody::custom;
    case MELODY_BOOT_DEV:
      return melody::boot_dev;
    case MELODY_ASSERT:
      return melody::assert;
    default:
      return melody::boot;
    }
  }

  void launch()
  {
    while (true)
    {
      osEvent evt = queue.get();
      if (evt.status == osEventMessage)
      {
        message_t *msg = (message_t *)evt.value.p;
        std::vector<buzzer_note> melody = find_melody(msg->nb);

        for (buzzer_note note : melody)
        {
          if (note.freq == 0)
          {
            continue;
          }
          pin.period(float(1.0 / note.freq));
          pin = 0.5;
          ThisThread::sleep_for(chrono::milliseconds(note.duration));
        }
        pin = 0.0;
      }
    }
  }
}

SHELL_COMMAND(play, "Play a melody")
{
  int melnum = atoi(argv[0]);
  shell_println("Playing melody ");
  shell_println(melnum);
  MemoryPool<buzzer::message_t, 16> mpool;
  buzzer::message_t *message = mpool.try_alloc();
  message->nb = melnum;
  buzzer::queue.try_put(message);
}

void buzzer_beep(unsigned int freq, unsigned int duration)
{
  melody::custom[0].freq = freq;
  melody::custom[0].duration = duration;

  MemoryPool<buzzer::message_t, 16> mpool;
  buzzer::message_t *message = mpool.alloc();
  message->nb = MELODY_CUSTOM;
  buzzer::queue.try_put(message);
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