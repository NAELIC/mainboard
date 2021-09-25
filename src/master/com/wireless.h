#pragma once

#include <swo.h>
#include <nRF24L01P_PTX.h>
#include <nRF24L01P_PRX.h>
#include <common/define/hardware.h>
#include <common/com/network.h>

namespace wireless
{
    void init();
    void send(packet_robot& packet);
}