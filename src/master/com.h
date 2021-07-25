#pragma once

#include <swo.h>
#include <shell.h>
#include <nRF24L01P_PTX.h>
#include <nRF24L01P_PRX.h>
#include <common/hardware.h>
#include <common/network.h>
#include <common/com/send_and_receive.h>

enum MODE {
    NORMAL,
    DIAGNOSTIC
};

namespace com {
    void launch();

    void diagnostic();
}