#include "send_and_receive.h"

namespace sr {
    static naelic::SWO swo;

    int send_packet_robot(nRF24L01P_PTX PTX, packet_robot &packet)
    {
        return PTX.TransmitPacket((char *)&packet, sizeof(packet_robot));
    }

    int send_packet_status(nRF24L01P_PTX PTX, packet_status &packet)
    {
        return PTX.TransmitPacket((char *)&packet, sizeof(packet_status));
    }

    int receive_packet_robot(nRF24L01P_PRX PRX, packet_robot &buffer_receive)
    {
        int size_packet = -1;
        if (PRX.IsPacketReady())
        {
            size_packet = PRX.ReadPacket((char *)&buffer_receive);
            swo.println(buffer_receive.id);
        }
        else
        {
            swo.println("Got nothing : PRX is not ready");
        }
        swo.printf("size_packet : %d\n", size_packet);
        return size_packet;
    }

    int receive_packet_status(nRF24L01P_PRX PRX, packet_status &buffer_receive)
    {
        int size_packet = -1;
        if (PRX.IsPacketReady())
        {
        size_packet = PRX.ReadPacket((char *)&buffer_receive);
        swo.println(buffer_receive.id);
        }
        else
        {
        swo.println("Got nothing : PRX is not ready");
        }
        swo.printf("size_packet : %d\n", size_packet);
        return size_packet;
    }
}