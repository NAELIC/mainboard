#pragma once

#include <mbed.h>
#include <swo.h>
#include <shell.h>
#include <nRF24L01P_PTX.h>
#include <nRF24L01P_PRX.h>
#include <common/hardware.h>
#include <common/network.h>

void com_init();
void test_radio();

int send(nRF24L01P_PTX PTX);

int receive(nRF24L01P_PRX PRX);
