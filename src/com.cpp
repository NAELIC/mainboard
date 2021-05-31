#include "nRF24L01P_PRX.h"
#include "nRF24L01P_PTX.h"
#include "shell.h"
#include <USBSerial.h>
#include <mbed.h>
#include <hardware.h>


DigitalOut led(PB_1);
DigitalOut usbDisconnect(PB_9);


nRF24L01P Device1(SPI1_MOSI, SPI1_MISO, SPI1_SCK, SPI1_CS1);
nRF24L01P Device2(SPI1_MOSI, SPI1_MISO, SPI1_SCK, SPI1_CS2);

nRF24L01P_PTX PTX(Device1, CE1, INT);
nRF24L01P_PRX PRX(Device2, CE2, INT);

SHELL_COMMAND(test, "") {
  char c = 'a';
  shell_println("Transmit");
  int r = PTX.TransmitPacket(&c, 1);
  shell_println(r);

  shell_println(Device1.num_lost_packets());
  shell_println(Device2.num_lost_packets());

  if (PRX.IsPacketReady()) {
    char d;
    int r = PRX.ReadPacket(&d);
    shell_print("Read: ");
    shell_print(r);
    shell_print(" ");
    shell_println(d);
  }

  shell_println((int)Device1.debug_read());

  wait_us(1000);
}

int main() {
  shell_init_usb();
  usbDisconnect = 0;

  PTX.Initialize();
  PTX.SetChannel(0);
  PTX.SetDataRate(2000);
  PTX.PowerUp();

  PRX.Initialize();
  PRX.SetChannel(0);
  PRX.SetDataRate(2000);
  PRX.SetPayloadSize(1);
  PRX.PowerUp();
  PRX.StartReceive();

  while (1) {
    ThisThread::sleep_for(100);
  }
}
