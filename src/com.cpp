#include "com.h"
#include "nRF24L01P_PRX.h"
#include "nRF24L01P_PTX.h"
#include "shell.h"
#include <USBSerial.h>
#include <mbed.h>
#include <hardware.h>


//define mast
nRF24L01P CH1(COM_MOSI, COM_MISO, COM_CLK, COM_CS1);
nRF24L01P CH2_1(COM_MOSI, COM_MISO, COM_CLK, COM_CS2);
nRF24L01P CH2_2(COM_MOSI, COM_MISO, COM_CLK, COM_CS2);

//pin allocation and Transmission or Reception mode
nRF24L01P_PTX PTX(CH1, COM_CE1, COM_CE1);
nRF24L01P_PRX PRX_1(CH2_1, COM_CE2, COM_CE1);
nRF24L01P_PRX PRX_2(CH2_2, COM_CE2, COM_CE1);

SHELL_COMMAND(send, "emission") {
    //PTX.TransmitPacket(argv[0],sizeof(argv[0]));
  char c = 'a';
  shell_println("Transmit");
  int r = PTX.TransmitPacket(&c, 1);
  shell_println(r);
}

SHELL_COMMAND(receive, "reception") {
  if (PRX_1.IsPacketReady() && PRX_2.IsPacketReady()) {//définir comment faire ici
    char buffer_1;
    int size_packet = PRX_1.ReadPacket(&buffer_1);
    /*//test size_packet
    char buffer_2;
    size_packet = PRX_2.ReadPacket(&buffer_2);
    //test size_packet
    if (buffer_1 & buffer_2) shell_println(buffer_1);
   */ 
  }
  wait_us(1000);
}

void com_init() {

  //initialize transmit mast
  PTX.Initialize();
  PTX.SetChannel(0);
  PTX.SetDataRate(2000);
  PTX.PowerUp();

  //initialize receive mast
  PRX_1.Initialize();
  PRX_1.SetChannel(1);
  PRX_1.SetDataRate(2000);
  PRX_1.SetPayloadSize(1);
  PRX_1.PowerUp();
  PRX_1.StartReceive();

  PRX_2.Initialize();
  PRX_2.SetChannel(1);
  PRX_2.SetDataRate(2000);
  PRX_2.SetPayloadSize(1);
  PRX_2.PowerUp();
  PRX_2.StartReceive();
}