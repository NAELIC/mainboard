#include "com.h"
#include "nRF24L01P_PRX.h"
#include "nRF24L01P_PTX.h"
#include "shell.h"
#include <USBSerial.h>
#include <mbed.h>
#include <hardware.h>

nRF24L01P Device1(COM_MOSI, COM_MISO, COM_CLK, COM_CS1);
nRF24L01P Device2(COM_MOSI, COM_MISO, COM_CLK, COM_CS2);

nRF24L01P_PTX PTX(Device1, COM_CE1, COM_CE1);
nRF24L01P_PRX PRX(Device2, COM_CE2, COM_CE1);

int send(nRF24L01P_PTX PTX,char *buffer, int size) {
    shell_println("Transmit");
    int r = PTX.TransmitPacket(buffer, size);
    shell_println(r);
    return r;
}

int receive(nRF24L01P_PRX PRX, char *buffer) {
    shell_println("Receive");
    int size_packet = -1;
    if (PRX.IsPacketReady()) {
        size_packet = PRX.ReadPacket(buffer);
    }
    return size_packet;
}

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
  } else {
    shell_println("Got nothing");
  }

  shell_println((int)Device2.debug_read());

  wait_us(1000);
}

SHELL_COMMAND(dbg, "") {}

void com_init() {

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
}