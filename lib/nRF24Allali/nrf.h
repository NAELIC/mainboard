#pragma once 

//#include "odometry.h"
#include <stdint.h>

#define SPEED_250k 0x20
#define SPEED_1M   0x00
#define SPEED_2M   0x08
#define POW_18db 0x00
#define POW_12db 0x01
#define POW_6db  0x02
#define POW_0db  0x03

void com_init();

enum ComState{
    OFF,
    RX,
    TX
};

ComState com_get_state(int card);
void com_set_state(int card, ComState state);
uint8_t com_read_reg(int index, uint8_t reg);

//void com_tick();

void print_addr(uint8_t a[5]);
void print_byte_as_hex(uint8_t v);

int com_has_data(int card);
void com_receive(int card, uint8_t *payload, int size);
bool com_send(int card, uint8_t *payload, int size);

void com_copy_addr(uint8_t dst[5],uint8_t src[5]);

void com_start_listening(int card);

void com_stop_listening(int card);

bool com_is_ok(int card);

bool com_is_all_ok();

void com_full_diag(float cards[3]);

int com_get_channel(int card);
void com_set_channel(int index, int chan);

int com_get_lost_count(int card);
int com_get_retransmitted_count(int card);

void com_clear_status(int card);

int com_get_pipe_payload(int card,int pipe);
void com_set_pipe_payload(int card,int pipe,uint8_t pl);

int com_get_ack(int card);
void com_set_ack(int card, bool v);
void com_set_ack(int card,int pipe, bool v);

/*!
 * \brief com_set_crc
 * \param card
 * \param crc 0: no CRC, 1: 1 byte CRC, 2: 2 bytes CRC
 */
void com_set_crc(int card,int crc);

//void com_set_reg(int index, uint8_t reg, uint8_t value);
//void com_set_reg5(int index, uint8_t reg, uint8_t value[5]);
void com_flush_rx(int index);
void com_flush_tx(int index);
int com_get_retransmission_delay(int card);
int com_get_retransmission_count(int card);

/*!
 * \brief com_set_retransmission
 * \param card
 * \param delay : n, delay will be 250us
 * \param count : between 0 and 15
 */
void com_set_retransmission(int card, int delay, int count);

void com_power(int card,bool up);

/*!
 * \brief com_set_rf
 * \param card
 * \param speed SPEED_250k, SPEED_1M  or  SPEED_2M
 * \param pow POW_18db, POW_12db, POW_6db, POW_0db
 */
void com_set_rf(int card, uint8_t speed, uint8_t pow);
uint8_t com_get_config(int card);
void com_set_config(int card,uint8_t);
uint8_t com_get_status(int card);
uint8_t com_get_rf_setup(int card);
uint8_t com_get_fifo_status(int card);

void com_get_tx_addr(int index,uint8_t add[5]);
void com_set_tx_addr(int index, uint8_t add[5]);
void com_set_rx_addr(int index,int pipe, uint8_t add[5]);
void com_get_rx_addr(int index,int pipe, uint8_t add[5]);


void com_ce_enable(int index);
void com_ce_disable(int index);

int com_cmp_addr(uint8_t dst[5], uint8_t src[5]);
