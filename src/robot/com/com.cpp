#include "com.h"
#include <nrf.h>

constexpr long SPI_COM_FREQUENCY = 2250000;

namespace com
{
  nRF24L01P Device1(COM_MOSI, COM_MISO, COM_CLK, COM_CS1, SPI_COM_FREQUENCY); // the SPI frequency
  nRF24L01P Device2(COM_MOSI, COM_MISO, COM_CLK, COM_CS2);
  nRF24L01P card_icmp(COM_MOSI, COM_MISO, COM_CLK, COM_CS3);

  nRF24L01P_PTX PTX(Device1, COM_CE1);
  nRF24L01P_PRX PRX_1(Device2, COM_CE2);

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
    PRX_init(PRX_1, CHANNEL1, sizeof(packet_robot));

    // default state
    card_icmp.set_interrupt_max_rt(true);
    card_icmp.set_address_width(5);
    card_icmp.set_crc_width(2);
    card_icmp.set_air_data_rate(2000);
    card_icmp.set_tx_power(0);
    card_icmp.set_channel(icmp::CHANNEL);
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
    common::swo.print("TODO");

    return true;
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

  int receive(nRF24L01P_PRX PRX, packet_robot &buffer)
  {
    int size_packet = -1;
    if (PRX.IsPacketReady())
    {
      size_packet = PRX.ReadPacket((char *) &buffer);
    }
    else
    {
      // common::swo.println("Got nothing : PRX is not ready");
    }
    // common::swo.printf("size_packet : %d\n", size_packet);
    return size_packet;
  }

  void launch()
  {
    com_init();
    init();
    bool first_order_timeout = true;

    packet_robot packet;

    State state = INIT;
    icmp::Order dhcp_request;

    while (true)
    {
      switch (state)
      {
      case INIT:
        // Send a DHCP Request and wait for answer

        // if (card_icmp.((char *)&dhcp_request, sizeof(icmp::Order) == 0)) {
        //     card_icmp.
        //     com_set_state(card_icmp,RX);
        //     state=WAIT_DHCP;
        //     icmp_stamp=millis();
        //   //  SerialUSB.println("wait for dhcp reply");
        // } else {
        //     dhcp_failure+=1;
        //     com_flush_tx(card_icmp);
        //     state=WAIT_AND_INIT;
        //     wait_init_timeout=millis()+100; // wait 100ms
        // }
        break;
      case WAIT_AND_INIT:
        break;
      case WAIT_DHCP:
        break;
      case RUNNING:
        break;
      }
      int size_receive = receive(PRX_1, packet);

      // common::swo.println(size_receive);

      if (size_receive == sizeof(packet)) {
        common::swo.println(packet.id);
      }

      ThisThread::sleep_for(100ms);
      // common::swo.println(common::mode);
      // if (common::mode == Mode::NORMAL)
      // {
      //   common::swo.println("TODO");
      // }
      // else
      // {
      //   ThisThread::sleep_for(1s);
      // }
    }
  }

  SHELL_COMMAND(diag, "DIAGNOSTIC MODE")
  {
    diagnostic();
  }
}