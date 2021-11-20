#pragma once

#include <swo.h>
#include <shell.h>
#include <nRF24L01P_PTX.h>
#include <nRF24L01P_PRX.h>
#include <common/define/hardware.h>

#include <common/com/network.h>
#include <common/define/common.h>

namespace send {
    int status(nRF24L01P_PTX PTX, packet_status &packet);
}

namespace receive {
    int robot(nRF24L01P_PRX PRX, packet_master &buffer_receive);
    int status(nRF24L01P_PRX PRX, packet_status &buffer_receive); 
}