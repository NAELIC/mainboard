#include <mbed.h>
#include <USBSerial.h>
#include <swo.h>
// #include <shell.h>

// #include <common/buzzer/buzzer.h>
// #include <common/utils/version.h>

// #include "com.h"

// uint32_t TIMEOUT_WATCHDOG_MS = 2000;
DigitalOut led(LED1);
static naelic::SWO swo;
USBSerial serial(false);

int main()
{
  // Thread buzzer_th;
  // buzzer_th.start(buzzer::launch);

  // Thread com_th;
  // com_th.start(com::launch);

  // Watchdog &watchdog = Watchdog::get_instance();
  // watchdog.start(TIMEOUT_WATCHDOG_MS);

  led = 1;
  while (true)
  {
    swo.println(serial.available());
    if(serial.available()) {
      char t = serial.getc();
      swo.print(t);
      serial.putc(t);
    }
    ThisThread::sleep_for(100ms);
  }
}
