#include <mbed.h>
#include <swo.h>

#include <common/buzzer/buzzer.h>
#include <common/utils/version.h>

#include "com/serial.h"
#include "com.h" // TODO : Rename on wireless !

DigitalOut led(LED1);
static naelic::SWO swo;

// uint32_t TIMEOUT_WATCHDOG_MS = 2000;

int main()
{
  Thread buzzer_th;
  buzzer_th.start(buzzer::launch);

  // Thread com_th;
  // com_th.start(com::launch);

  // Watchdog &watchdog = Watchdog::get_instance();
  // watchdog.start(TIMEOUT_WATCHDOG_MS);
  
  Thread serial_th;
  serial_th.start(serial::launch);

  led = 1;
  while (true)
  {
    // swo.println(serial.available());
    // if(serial.available()) {
    //   char t = serial.getc();
    //   swo.print(t);
    //   serial.putc(t);
    // }
    ThisThread::sleep_for(100ms);
  }
}
