#include "com.h"

nRF24L01P Device1(COM_MOSI, COM_MISO, COM_CLK, COM_CS1);
nRF24L01P Device2(COM_MOSI, COM_MISO, COM_CLK, COM_CS2);
nRF24L01P Device3(COM_MOSI, COM_MISO, COM_CLK, COM_CS3);

nRF24L01P_PTX PTX(Device1, COM_CE1, COM_CE1);
nRF24L01P_PRX PRX(Device2, COM_CE2, COM_CE1);
nRF24L01P_PRX PRX2(Device3, COM_CE3, COM_CE1);
static naelic::SWO swo;

int send(nRF24L01P_PTX PTX)
{
  struct packet_robot packet;
  
  packet.id = 4;
  packet.xpos = 125;
  swo.println("Transmit");
  int r = PTX.TransmitPacket((char *) &packet, sizeof(struct packet_robot));
  swo.println(r);

  swo.println(Device1.num_lost_packets());
  swo.println(Device2.num_lost_packets());
  return r;
}

SHELL_COMMAND(send, "send"){
  shell_println(send(PTX));
}

int receive(nRF24L01P_PRX PRX)
{
  int size_packet = -1;
  if (PRX.IsPacketReady())
  {
    
    struct packet_master receive;
    size_packet = PRX.ReadPacket((char *) &receive);

    swo.print("Read: ");
    swo.print(size_packet);
    swo.print(" ");
    swo.println(receive.x_speed);
  }
  else
  {
    swo.println("Got nothing");
  }

  swo.println((int)Device2.debug_read());

  wait_us(1000);
  return size_packet;
}

SHELL_COMMAND(receive, "receive"){
  shell_println(receive(PRX));
}

void test_radio()
{
  // struct packet_robot packet;
  
  // packet.id = 4;
  // packet.xpos = 125;
  // swo.println("Transmit");
  // int r = PTX.TransmitPacket((char *) &packet, sizeof(struct packet_robot));
  // swo.println(r);

  // swo.println(Device1.num_lost_packets());
  // swo.println(Device2.num_lost_packets());

  // if (PRX.IsPacketReady())
  // {
  //   struct packet_robot receive;
  //   int r = PRX.ReadPacket((char *) &receive);

  //   swo.print("Read: ");
  //   swo.print(r);
  //   swo.print(" ");
  //   swo.println(receive.id);
  //   swo.println(receive.xpos);
  // }
  // else
  // {
  //   swo.println("Got nothing");
  // }

  // swo.println((int)Device2.debug_read());

  // wait_us(1000);
  receive(PRX);
}

// SHELL_COMMAND(test, "")
// { packet_robot packet_receive;
//   char* buffer = "HelloWorld";
//   shell_println("Transmit");
//   int r = PTX.TransmitPacket(buffer, sizeof(packet_receive));
//   shell_println(r);

//   shell_println(Device1.num_lost_packets());
//   shell_println(Device2.num_lost_packets());

//   if (PRX.IsPacketReady())
//   {
//     char d[10];
//     int r = PRX.ReadPacket(d);
//     shell_print("Read: ");
//     shell_print(r);
//     shell_print(" ");
//     shell_println(d);
//   }
//   else
//   {
//     shell_println("Got nothing");
//   }

//   shell_println((int)Device2.debug_read());

//   wait_us(1000);
// }
 

void com_init()
{

  PTX.Initialize();
  PTX.SetChannel(0);
  PTX.SetDataRate(2000);
  PTX.PowerUp();

  PRX.Initialize();
  PRX.SetChannel(0);
  PRX.SetDataRate(2000);
  PRX.SetPayloadSize(sizeof(struct packet_master));
  PRX.PowerUp();
  PRX.StartReceive();

  PRX2.Initialize();
  PRX2.SetChannel(0);
  PRX2.SetDataRate(2000);
  PRX2.SetPayloadSize(sizeof(struct packet_master));
  PRX2.PowerUp();
  PRX2.StartReceive();
}