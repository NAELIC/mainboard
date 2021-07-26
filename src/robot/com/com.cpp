#include "com.h"

namespace com
{
  nRF24L01P Device1(COM_MOSI, COM_MISO, COM_CLK, COM_CS1);
  nRF24L01P Device2(COM_MOSI, COM_MISO, COM_CLK, COM_CS2);
  nRF24L01P Device3(COM_MOSI, COM_MISO, COM_CLK, COM_CS3);

  nRF24L01P_PTX PTX(Device1, COM_CE1);
  nRF24L01P_PRX PRX_1(Device2, COM_CE2);
  nRF24L01P_PRX PRX_2(Device3, COM_CE3);

  /** TODO : MOVE IN LIBRARY **/
  void PRX_init(nRF24L01P_PRX &p, int channel, int size)
  {
    p.initialize();
    p.set_channel(channel);
    p.set_data_rate(2000);
    p.set_payload_size(size);
    p.power_up();
    p.start_receive();
  }

  void PTX_init(nRF24L01P_PTX &p, int channel)
  {
    p.initialize();
    p.set_channel(channel);
    p.set_data_rate(2000);
    p.power_up();
  }

  void init()
  {
    PTX_init(PTX, CHANNEL1);
    PRX_init(PRX_1, CHANNEL2, sizeof(packet_robot));
    PRX_init(PRX_2, CHANNEL2, sizeof(packet_robot));
  }

  int receive(nRF24L01P_PRX PRX)
  {
    int size_packet = -1;
    if (PRX.IsPacketReady())
    {
      packet_status receive;
      size_packet = PRX.ReadPacket((char *)&receive);
      common::swo.println(receive.id);
    }
    else
    {
      common::swo.println("Got nothing");
    }
    common::swo.println(size_packet);
    return size_packet;
  }

  bool com_is_ok()
  {
    bool is_ok = true;
    packet_robot packet;
    packet.id = 42;
    int status = send::robot(PTX, packet);
    common::swo.print("status :");
    common::swo.print(status);
    if (status != 0)
      is_ok = false;
    common::swo.println("#PRX1");
    packet_robot packet1;
    if (receive::robot(PRX_1, packet1) != sizeof(packet_robot))
    {
      common::swo.println("ERROR PRX1");
      is_ok = false;
    }

    common::swo.println("#PRX2");
    packet_robot packet2;
    if (receive::robot(PRX_2, packet2) != sizeof(packet_robot))
    {
      common::swo.println("ERROR PRX2");
      is_ok = false;
    }

    return is_ok;
  }

  void diagnostic()
  {
    common::swo.println("* Com module [robot]#");
    if (com_is_ok())
    {
      common::swo.println(" OK");
    }
    else
    {
      common::swo.println(" NOTHING");
    }
  }

  void launch()
  {
    init();

    while (true)
    {
      common::swo.println(common::mode);
      // if (common::mode == Mode::NORMAL)
      // {
      //   common::swo.println("TODO");
      //   ThisThread::sleep_for(1s);
      // }
      // else
      // {
        ThisThread::sleep_for(1s);
      // }
    }
  }

  SHELL_COMMAND(diag, "DIAGNOSTIC MODE")
  {
    diagnostic();
  }
}