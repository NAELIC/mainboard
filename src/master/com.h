#pragma once

#include <swo.h>
#include <shell.h>
#include <nRF24L01P_PTX.h>
#include <nRF24L01P_PRX.h>
#include <common/hardware.h>

void com_init();
void test_radio();

int send(nRF24L01P_PTX PTX, char *buffer, int size);

int receive(nRF24L01P_PRX PRX, char *buffer);
