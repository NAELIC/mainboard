#pragma once

#include <swo.h>
#include <shell.h>
#include <nRF24L01P_PTX.h>
#include <nRF24L01P_PRX.h>
#include <common/hardware.h>
#include <common/network.h>

namespace sr {
    int send_packet_robot(nRF24L01P_PTX PTX, packet_robot &packet);
    int send_packet_status(nRF24L01P_PTX PTX, packet_status &packet);
    int receive_packet_robot(nRF24L01P_PRX PRX, packet_robot &buffer_receive);
    int receive_packet_status(nRF24L01P_PRX PRX, packet_status &buffer_receive);
}