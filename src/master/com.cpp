#include "com.h"

namespace com
{
  static naelic::SWO swo;

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
    PTX_init(PTX, CHANNEL2);
    PRX_init(PRX_1, CHANNEL1, sizeof(packet_status));
    PRX_init(PRX_2, CHANNEL1, sizeof(packet_status));
  }


  bool com_is_ok()
  {
    bool is_ok = true;
    packet_status packet;
    packet.id = 42;
    int status = send::status(PTX, packet);
    swo.print("status :");
    swo.print(status);
    if (status != 0)
      is_ok = false;
    swo.println("#PRX1");
    packet_status packet1;
    if (receive::status(PRX_1, packet1) != sizeof(packet_status))
    {
      swo.println("ERROR PRX1");
      is_ok = false;
    }

    swo.println("#PRX2");
    packet_status packet2;
    if (receive::status(PRX_2, packet2) != sizeof(packet_status))
    {
      swo.println("ERROR PRX2");
      is_ok = false;
    }

    return is_ok;
  }

  void diagnostic()
  {
    // swo.println("power_down");
    // PTX.PowerDown();

    // swo.println("change channel");
    // PTX.set_channel(CHANNEL2);

    // swo.println("power_up");
    // PTX.power_up();
    swo.println("* Com module [master]#");
    if (com_is_ok())
    {
      swo.println(" OK");
    }
    else
    {
      swo.println(" NOTHING");
    }
  }

  void launch()
  {
    init();

    while (true)
    {
      // swo.println(mode);
      ThisThread::sleep_for(1s);
    }
  }
}