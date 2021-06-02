#pragma once
void com_init();

int send(nRF24L01P_PTX PTX,char *buffer, int size);

int receive(nRF24L01P_PRX PRX, char *buffer);