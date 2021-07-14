#include <common/buzzer.h>
// #include "drivers.h"
// #include "hardware.h"
// #include "mux.h"
// #include "voltage.h"
// #include "infos.h"
#include "com.h"
#include <USBSerial.h>
#include <mbed.h>
#include <swo.h>
#include <shell.h>

// uint32_t TIMEOUT_WATCHDOG_MS = 2000;
DigitalOut led(LED1);
static naelic::SWO swo;

int main()
{
  shell_init_usb();
  // Watchdog &watchdog = Watchdog::get_instance();
  // watchdog.start(TIMEOUT_WATCHDOG_MS);

  // Buzzer

  //   infos_init();
  //   mux_init();
  //   voltage_init();
  buzzer_init();
  //   drivers_init();
  com_init();
  // ir_init();

  // melody

  // buzzer_play(11);
  // buzzer_wait_play();
  // wait_us(2000000);
  led = 1;
  while (true)
  {
    swo.println("Hello i'm Robot !");
    // ir
    buzzer_tick();
    // voltage_tick();

    // ir_tick();

    ThisThread::sleep_for(100ms);
    test_radio();
  }
}

// afficher valeur millis dans le tickn pour changement.
