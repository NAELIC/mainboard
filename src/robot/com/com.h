#pragma once

#include <shell.h>
#include <nRF24L01P_PTX.h>
#include <nRF24L01P_PRX.h>
#include <common/define/hardware.h>
#include <common/com/network.h>
#include <common/com/send_and_receive.h>

#include <common/define/common.h>

namespace com {
    void launch();

    void diagnostic();
}