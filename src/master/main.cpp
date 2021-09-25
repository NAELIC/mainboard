#include <mbed.h>
#include <swo.h>

#include <common/buzzer/buzzer.h>
#include <common/utils/version.h>

#include "com/serial.h"

DigitalOut led(LED1);

// uint32_t TIMEOUT_WATCHDOG_MS = 2000;

int main()
{
  Thread buzzer_th;
  buzzer_th.start(buzzer::launch);

  // Watchdog &watchdog = Watchdog::get_instance();
  // watchdog.start(TIMEOUT_WATCHDOG_MS);
  
  Thread serial_th;
  serial_th.start(serial::launch);

  led = 1;
  while (true)
  {
    led = !led;
    ThisThread::sleep_for(2000ms);
  }
}
