#pragma once

#include <shell.h>
#include <nRF24L01P_PTX.h>
#include <nRF24L01P_PRX.h>
#include <common/hardware.h>
#include <common/com/network.h>
#include <common/com/send_and_receive.h>

#include <common/utils/common.h>

namespace com {
    void launch();

    void diagnostic();
}