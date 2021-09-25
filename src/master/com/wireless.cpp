#include "wireless.h"
static naelic::SWO swo;

namespace wireless
{
    // Devices
    nRF24L01P Device1(COM_MOSI, COM_MISO, COM_CLK, COM_CS1);
    nRF24L01P Device2(COM_MOSI, COM_MISO, COM_CLK, COM_CS2);
    nRF24L01P Device3(COM_MOSI, COM_MISO, COM_CLK, COM_CS3);

    //
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
        PRX_init(PRX_1, CHANNEL2, sizeof(packet_status));
        PRX_init(PRX_2, CHANNEL2, sizeof(packet_status));
    }

    void send(packet_robot &packet)
    {
        PTX.TransmitPacket((char *)&packet, sizeof(packet_robot));
    }
}