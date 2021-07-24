#include <mbed.h>
#include <USBSerial.h>
#include <swo.h>
#include <shell.h>

#include <common/voltage.h>
#include <common/buzzer.h>
#include <common/version.h>

#include "drivers.h"
#include "ir.h"
// #include "infos.h"
#include "com.h"


// uint32_t TIMEOUT_WATCHDOG_MS = 2000;
DigitalOut led(LED1);
static naelic::SWO swo;

int main()
{
  Thread buzzer_th;
  buzzer_th.start(buzzer::launch);

  shell_init_usb();
  // Watchdog &watchdog = Watchdog::get_instance();
  // watchdog.start(TIMEOUT_WATCHDOG_MS);
  //   infos_init();
  //buzzer_init();
  // drivers_init();
  com_init();
  // ir_init();

  // buzzer_play(11);
  // buzzer_wait_play();
  // wait_us(2000000);
  led = 1;
  while (true)
  {
    // swo.println(6.6);
    // buzzer_tick();
    // voltage_tick();
    // ir_tick();
    ThisThread::sleep_for(100ms);
    // test_radio();
  }
}