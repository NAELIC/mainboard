#include <mbed.h>
#include <USBSerial.h>
#include "shell.h"

DigitalOut led(PB_1);
DigitalOut usbDisconnect(PB_9);

int main()
{
  shell_init_usb();
  usbDisconnect = 0;

    while (true) {
        ThisThread::sleep_for(100);
    }
}
