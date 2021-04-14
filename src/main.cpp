#include <mbed.h>
#include <USBSerial.h>
#include "shell.h"
#include "ir.h"

uint32_t TIMEOUT_WATCHDOG_MS = 2000;
DigitalOut led(LED1);

int main()
{
    shell_init_usb();
    Watchdog &watchdog = Watchdog::get_instance();
    watchdog.start(TIMEOUT_WATCHDOG_MS);

    // IR
    ir_init();

    while (true) {
        // IR
        ir_tick();
        led = 0;
        ThisThread::sleep_for(500);
        led = 1;
        ThisThread::sleep_for(500);
    }
}
