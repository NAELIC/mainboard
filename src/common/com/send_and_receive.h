#pragma once

#include <swo.h>
#include <shell.h>
#include <nRF24L01P_PTX.h>
#include <nRF24L01P_PRX.h>
#include <common/define/hardware.h>

#include <common/com/network.h>
#include <common/define/common.h>

namespace send {
    /**
     * @brief send robot packet on PTX mast in robot -> master communication
     * 
     * @param PTX 
     * @param packet 
     * @return Returns 0 on success or -1 if the packet wasn't successfully acknowledged.
     */
    int status(nRF24L01P_PTX PTX, packet_status &packet);
}

namespace receive {
    /**
     * @brief wait and receive a packet_master on PRX mast
     * 
     * @param PRX 
     * @param buffer_receive 
     * @return size of packet receive
     */
    int robot(nRF24L01P_PRX PRX, packet_master &buffer_receive);

    /**
     * @brief wait and receive a packet robot on PRX mast
     * 
     * @param PRX 
     * @param buffer_receive 
     * @return size of packet receive
     */
    int status(nRF24L01P_PRX PRX, packet_status &buffer_receive); 
}