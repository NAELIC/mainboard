#include "nRF24L01P_PRX.h"
#include "nRF24L01P_PTX.h"
#include "shell.h"
#include <USBSerial.h>
#include <mbed.h>
#include <hardware.h>


DigitalOut led(PB_1);
DigitalOut usbDisconnect(PB_9);

// D4
#define SPI1_MOSI PA_7
// D5
#define SPI1_MISO PA_6
// D6
#define SPI1_SCK PA_5
// D7 "CSn" du transmetteur
#define SPI1_CS1 PA_4
// D8 "CSn" de l'émetteur
#define SPI1_CS2 PA_3
// D9 "CE" du transmetteur
#define CE1 PA_2
// D10 "CE" de l'émetteur
#define CE2 PA_1
// D11
#define INT PA_0

//define mast
nRF24L01P CH1(SPI1_MOSI, SPI1_MISO, SPI1_SCK, SPI1_CS1);
nRF24L01P CH2_1(SPI1_MOSI, SPI1_MISO, SPI1_SCK, SPI1_CS2);
nRF24L01P CH2_2(SPI1_MOSI, SPI1_MISO, SPI1_SCK, SPI1_CS2);

//pin allocation and Transmission or Reception mode
nRF24L01P_PTX PTX(CH1, CE1, INT);
nRF24L01P_PRX PRX_1(CH2_1, CE2, INT);
nRF24L01P_PRX PRX_2(CH2_2, CE2, INT);

SHELL_COMMAND(send, "emission") {
    PTX.TransmitPacket(&argv[0],sizeof(argv[0]));
}

SHELL_COMMAND(receive, "reception") {
  if (PRX.IsPacketReady()) {
    char buffer;
    int r = PRX_1.ReadPacket(&buffer);
  }
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
