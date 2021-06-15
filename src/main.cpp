#include "buzzer.h"
#include "drivers.h"
#include "hardware.h"
#include "mux.h"
#include "shell.h"
#include "voltage.h"
#include "infos.h"
#include "com.h"
#include <USBSerial.h>
#include <mbed.h>

uint32_t TIMEOUT_WATCHDOG_MS = 2000;
DigitalOut led(LED1);

int main() {
  shell_init_usb();
  // Watchdog &watchdog = Watchdog::get_instance();
  // watchdog.start(TIMEOUT_WATCHDOG_MS);

  // Buzzer
  infos_init();
  mux_init();
  voltage_init();
  buzzer_init();
  drivers_init();
  com_init();
  // ir_init();

  // melody

  // buzzer_play(11);
  // buzzer_wait_play();
  // wait_us(2000000);

  while (true) {
    // ir
    buzzer_tick();
    voltage_tick();

    // ir_tick();

    // led = 0;
    // ThisThread::sleep_for(500ms);
    // led = 1;
    ThisThread::sleep_for(5ms);
  }
}

// afficher valeur millis dans le tickn pour changement.
