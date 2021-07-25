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

// void com_init();
// void test_radio();

// int send(nRF24L01P_PTX PTX);

// int receive(nRF24L01P_PRX PRX);

namespace com {
    void launch();

    void diagnostic();
}