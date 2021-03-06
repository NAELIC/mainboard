#include <mbed.h>
#include <USBSerial.h>
#include <swo.h>
#include <shell.h>

#include <common/buzzer/buzzer.h>
#include <common/utils/version.h>

#include "kicker/kicker.h"

#include "engine/drivers.h"
#include "engine/dribbler.h"
#include "ir/ir.h"
// #include "infos.h"
#include "com/com.h"
#include "voltage/voltage.h"


// uint32_t TIMEOUT_WATCHDOG_MS = 2000;
DigitalOut led(LED1);
static naelic::SWO swo;

EventQueue event_queue;

int main()
{
  kicker_init();
  Thread buzzer_th;
  buzzer_th.start(buzzer::launch);

  // Thread voltage_th(osPriorityNormal);
  // voltage_th.start(voltage::launch);

  event_queue.call_every(1s, voltage::compute);
  event_queue.call_every(1s, ir::compute);
  event_queue.call_every(100, kicker_tick);

  Thread com_th;
  com_th.start(com::launch);
  com_th.set_priority(osPriorityHigh1);

  Thread engine_th;
  engine_th.start(drivers::launch);

  // Thread dribbler_th;
  // dribbler_th.start(dribbler::launch);

  shell_init_usb();
  // Watchdog &watchdog = Watchdog::get_instance();
  // watchdog.start(TIMEOUT_WATCHDOG_MS);
  //   infos_init();
  led = 1;

  event_queue.dispatch_forever();
  // Thread queue_thread;
  // queue_thread.start(&event_queue, &EventQueue::dispatch_forever);

  // while (true)
  // {
  //   // test_radio();
  // }
}