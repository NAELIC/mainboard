#include <mbed.h>
#include <USBSerial.h>
#include <swo.h>
#include <shell.h>

#include <common/mux.h>
#include <common/buzzer/buzzer.h>
#include <common/define/hardware.h>
// #include <common/utils/version.h>

// #include "kicker/kicker.h"

#include "engine/drivers.h"
// #include "engine/dribbler.h"
// #include "ir/ir.h"
#include "infos.h"
#include "com/com.h"
#include "voltage/voltage.h"
#include <nrf.h>

// uint32_t TIMEOUT_WATCHDOG_MS = 2000;
DigitalOut led(LED1);
static naelic::SWO swo;
// Override mbed printf implementation
FileHandle *mbed::mbed_override_console(int)
{
    return &swo;
}


EventQueue event_queue;

int main()
{
  // kicker_init();

  // infos_init();

  // Thread buzzer_th;
  // buzzer_th.start(buzzer::launch);

  // bool h1=get_hall(HALL1_ADDR);
  //   // delay_us(1000);
  // bool h2=get_hall(HALL2_ADDR);
  //  // delay_us(1000);
  // bool h3=get_hall(HALL3_ADDR);
  //   // delay_us(1000);
  // bool h4=get_hall(HALL4_ADDR);
  
  // int id=infos_get_id();
  // swo.println(id);

  com::robot_init();

  // Thread voltage_th(osPriorityNormal);
  // voltage_th.start(voltage::launch);

  // event_queue.call_every(1s, voltage::compute);
  // event_queue.call_every(1s, ir::compute);
  // event_queue.call_every(100, kicker_tick);

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
  led = 1;

  while(true) {
    ThisThread::sleep_for(100s);
  }
  // event_queue.dispatch_forever();
  // Thread queue_thread;
  // queue_thread.start(&event_queue, &EventQueue::dispatch_forever);
}