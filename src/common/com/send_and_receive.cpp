#include "send_and_receive.h"

int send::status(nRF24L01P_PTX PTX, packet_status &packet)
{
    return PTX.TransmitPacket((char *)&packet, sizeof(packet_status));
}

int receive::robot(nRF24L01P_PRX PRX, packet_robot &buffer_receive)
{
    int size_packet = -1;
    if (PRX.IsPacketReady())
    {
        size_packet = PRX.ReadPacket((char *)&buffer_receive);
        common::swo.println(buffer_receive.id);
    }
    else
    {
        common::swo.println("Got nothing : PRX is not ready");
    }
    common::swo.printf("size_packet : %d\n", size_packet);
    return size_packet;
}

int receive::status(nRF24L01P_PRX PRX, packet_status &buffer_receive)
{
    int size_packet = -1;
    if (PRX.IsPacketReady())
    {
        size_packet = PRX.ReadPacket((char *)&buffer_receive);
        common::swo.println(buffer_receive.id);
    }
    else
    {
        common::swo.println("Got nothing : PRX is not ready");
    }
    common::swo.printf("size_packet : %d\n", size_packet);
    return size_packet;
}