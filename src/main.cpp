#include <mbed.h>
#include <USBSerial.h>
#include "shell.h"

DigitalOut led(LED1);

int main()
{
  shell_init_usb();

    while (true) {
        led = 0;
        ThisThread::sleep_for(500);
        led = 1;
        ThisThread::sleep_for(500);
    }
}
