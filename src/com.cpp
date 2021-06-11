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
nRF24L01P CH2_2(COM_MOSI, COM_MISO, COM_CLK, COM_CS3);

//pin allocation and Transmission or Reception mode
nRF24L01P_PTX PTX(CH1, COM_CE1, COM_CE1);
nRF24L01P_PRX PRX_1(CH2_1, COM_CE2, COM_CE1);
nRF24L01P_PRX PRX_2(CH2_2, COM_CE2, COM_CE1);

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

void com_init() {

  //initialize transmit mast
  PTX.Initialize();
  PTX.SetChannel(0);
  PTX.SetDataRate(2000);
  PTX.PowerUp();

  //initialize receive mast
  PRX_1.Initialize();
  PRX_1.SetChannel(0);
  PRX_1.SetDataRate(2000);
  PRX_1.SetPayloadSize(1);
  PRX_1.PowerUp();
  PRX_1.StartReceive();

  PRX_2.Initialize();
  PRX_2.SetChannel(0);
  PRX_2.SetDataRate(2000);
  PRX_2.SetPayloadSize(1);
  PRX_2.PowerUp();
  PRX_2.StartReceive();
}

SHELL_COMMAND(com_init, "com init()"){
  com_init();
}

SHELL_COMMAND(test, "com test") {
  char c = 'a';
  shell_println("Transmit");
  int r = PTX.TransmitPacket(&c, 1);
  shell_println(r);

  shell_println(CH1.num_lost_packets());
  shell_println(CH2_1.num_lost_packets());

  if (PRX_1.IsPacketReady()) {
    char d;
    int r = PRX_1.ReadPacket(&d);
    shell_print("Read: ");
    shell_print(r);
    shell_print(" ");
    shell_println(d);
  } else {
    shell_println("Got nothing");
  }

  shell_println((int)CH2_1.debug_read());

  ThisThread::sleep_for(1ms);
}


SHELL_COMMAND(send, "emission") {
  //PTX.TransmitPacket(argv[0],sizeof(argv[0]));
  //char c = 'a';
  char* buffer = argv[0];
  int size_packet = send(PTX, buffer, sizeof(char));
  shell_println(size_packet);
}

SHELL_COMMAND(receive, "reception") {
  /*if (PRX_1.IsPacketReady() && PRX_2.IsPacketReady()) {//définir comment faire ici
    char buffer_1;
    int size_packet = PRX_1.ReadPacket(&buffer_1);
    if(size_packet > 1) shell_println("(com.cpp) error: ReadPacket -> buffer_1");
    char buffer_2;
    size_packet = PRX_2.ReadPacket(&buffer_2);
    if(size_packet > 1) shell_println("(com.cpp) error: ReadPacket -> buffer_2");
    if (buffer_1 & buffer_2) shell_println(buffer_1);
  }*/
  char buffer_1;
  char buffer_2;
  if (PRX_1.IsPacketReady() && PRX_2.IsPacketReady()){
    int size_packet_1 = receive(PRX_1, &buffer_1);
    int size_packet_2 = receive(PRX_2, &buffer_2);
  
    if(size_packet_1 != size_packet_2) shell_println("(com.cpp) error : recieve -> size of variables 'size_packet_X' is different");
    if (buffer_1 & buffer_2) shell_println(buffer_1);
  }
  //debug ------
  shell_println("IsPacketReady ?");
  bool debug = PRX_1.IsPacketReady() && PRX_2.IsPacketReady();
  shell_println(debug);
  if(!debug){
    shell_println("    PRX_1 ready ?");
    shell_println(PRX_1.IsPacketReady());
    shell_println("    PRX_2 ready ?");
    shell_println(PRX_2.IsPacketReady());
  }
  //------------

  ThisThread::sleep_for(1ms);
}