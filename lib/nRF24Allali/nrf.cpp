#include "nrf.h"

#include <assert.h>
#include <mbed.h>
#include <shell.h>  // usb shell
#include <stdint.h>
#include <stdlib.h>
#include <swo.h>  // io JTAG

#include "nrf24l01.h"

// #define DEBUG_COM

Timer timer;
naelic::SWO swo;

// Communication pins
#define COM_MOSI PA_7
#define COM_MISO PA_6
#define COM_CLK PA_5

#define COM_CE1 PC_12
#define COM_CE2 PC_2
#define COM_CE3 PC_3

#define COM_CS1 PB_5
#define COM_CS2 PB_4
#define COM_CS3 PD_2

#define HIGH 1
#define LOW 0

SPI com(COM_MOSI, COM_MISO, COM_CLK);

DigitalOut cs[3] = {{COM_CS1, HIGH}, {COM_CS2, HIGH}, {COM_CS3, HIGH}};

DigitalOut ce[3] = {{COM_CE1, LOW}, {COM_CE2, LOW}, {COM_CE3, LOW}};

ComState com_current_state[3] = {OFF, OFF, OFF};

ComState com_get_state(int card) {
    assert(card >= 0 && card < 3);
    return com_current_state[card];
}
bool com_check_state(int card) {
    assert(card >= 0 && card < 3);
    uint8_t c = com_read_reg(card, REG_CONFIG);
    if (com_current_state[card] == OFF) return (c & CONFIG_PWR_UP) == 0;
    if (com_current_state[card] == RX) {
        return ((c & CONFIG_PRIM_RX) != 0) && ((c & CONFIG_PWR_UP) != 0) &&
               (ce[card] == LOW);
    }
    // TX:
    return (ce[card] == HIGH) && ((c & CONFIG_PRIM_RX) == 0) &&
           ((c & CONFIG_PWR_UP) != 0);
}

int state_change[3] = {0, 0, 0};

void com_set_state(int card, ComState state) {
    assert(card >= 0 && card < 3);
    if (state == com_current_state[card]) return;
    state_change[card] += 1;
    if (state == OFF) {  // turn off the card
        com_ce_disable(card);
        com_power(card, false);
        com_current_state[card] = OFF;
        return;
    }
#ifdef DEBUG_COM
    shell_print("state for card ");
    shell_print(card);
    shell_print(" change from ");
    shell_print(com_current_state[card]);

    shell_print(" to ");
    shell_println(state);
#endif
    if (com_current_state[card] == OFF) {
        com_power(card, true);
        // need to wait 5ms, should be 1.5ms but according
        // to RF24 library can be up to 5ms
        for (int w = 0; w < 5; w++) {
            wait_us(1000);
            // watchdog_feed();
        }
    }
    // We are going from tx to rx or from rx to tx
    com_ce_disable(card);
    wait_us(130);
    // we are in standby I mode
    if (state == TX) {
        com_stop_listening(card);
        com_ce_enable(card);
        wait_us(10 + 130);  // we should be in standby II mode now
        com_current_state[card] = TX;
    } else {  // RX
        com_start_listening(card);
        com_ce_enable(card);
        wait_us(130);  // in rx mode until ce pin goes down
        com_current_state[card] = RX;
    }
}

SHELL_PARAMETER_INT(irqed, "", 0);

void print_byte_as_hex(uint8_t v) {
    uint8_t a = (v >> 4);
    if (a < 10)
        shell_print(a);
    else
        shell_print((char)('A' + a - 10));
    a = v & 0x0F;
    if (a < 10)
        shell_print(a);
    else
        shell_print((char)('A' + a - 10));
}

void print_addr(uint8_t a[5]) {
    for (int i = 0; i < 5; ++i) {
        print_byte_as_hex(a[i]);
        if (i < 4) shell_print("-");
    }
    shell_print("  ");
    for (int i = 0; i < 5; ++i) {
        shell_print(a[i]);
        if (i < 4) shell_print(".");
    }
}

const char *reg_to_str(uint8_t reg) {
    static char buffer[5];
    switch (reg) {
        case 0x00:
            return "CONFIG";
            break;
        case 0x01:
            return "EN_AA";
            break;
        case 0x02:
            return "EN_RXADDR";
            break;
        case 0X03:
            return "SETUP_AW";
            break;
        case 0x04:
            return "SETUP_RETR";
            break;
        case 0x05:
            return "RF_CH";
            break;
        case 0x06:
            return "RF_SETUP";
            break;
        case 0x07:
            return "STATUS";
            break;
        case 0x08:
            return "OBSERVE_TX";
            break;
        case 0x09:
            return "RPD";
            break;
        case 0x0A:
            return "RX_ADDR_P0";
            break;
        case 0x0B:
            return "RX_ADDR_P1";
            break;
        case 0x0C:
            return "RX_ADDR_P2";
            break;
        case 0x0D:
            return "RX_ADDR_P3";
            break;
        case 0x0E:
            return "RX_ADDR_P4";
            break;
        case 0x0F:
            return "RX_ADDR_P5";
            break;
        case 0x10:
            return "TX_ADDR";
            break;
        case 0x11:
            return "RX_PW_P0";
            break;
        case 0x12:
            return "RX_PW_P1";
            break;
        case 0x13:
            return "RX_PW_P2";
            break;
        case 0x14:
            return "RX_PW_P3";
            break;
        case 0x15:
            return "RX_PW_P4";
            break;
        case 0x16:
            return "RX_PW_P5";
            break;
        case 0x17:
            return "FIFO_STATUS";
            break;
        case 0x1C:
            return "DYN_PL";
            break;
        case 0x1D:
            return "FEATURE";
            break;
        default:
            buffer[0] = (reg >> 4) + '0';
            buffer[1] = (reg & 0x0f) + '0';
            buffer[2] = 0;
            //    snprintf(buffer,5,"%02x",reg);
            break;
    }
    return buffer;
}

static void com_spi_send(int card, uint8_t *packet, size_t n) {
    assert(card >= 0 && card < 3);
    cs[card] = LOW;
    //  wait_us(100);
    for (int k = 0; k < n; k++) {
        uint8_t reply = com.write(packet[k]);
        packet[k] = reply;
    }
    //    wait_us(100);
    cs[card] = HIGH;
}

void com_set_reg(int card, uint8_t reg, uint8_t value) {
    assert(card >= 0 && card < 3);
#ifdef DEBUG_COM
    shell_print("set reg: ");
    shell_print(reg_to_str(reg));
    shell_print(" ");
    shell_println(value);
#endif
    reg |= OP_WRITE;

    uint8_t packet[2] = {reg, value};

    com_spi_send(card, packet, 2);
}

void com_set_reg5(int card, uint8_t reg, uint8_t value[5]) {
    assert(card >= 0 && card < 3);
#ifdef DEBUG_COM
    shell_print("set reg5:");
    shell_print(reg_to_str(reg));
    shell_print(" ");
    shell_print(value[0]);
    shell_print("-");
    shell_print(value[1]);
    shell_print("-");
    shell_print(value[2]);
    shell_print("-");
    shell_print(value[3]);
    shell_print("-");
    shell_println(value[4]);
#endif

    reg |= OP_WRITE;

    uint8_t packet[6] = {
        reg, value[0], value[1], value[2], value[3], value[4],
    };

    com_spi_send(card, packet, 6);
}

uint8_t com_read_reg(int card, uint8_t reg) {
    assert(card >= 0 && card < 3);
#ifdef DEBUG_COM
    shell_print("read reg:");
    shell_print(reg_to_str(reg));
#endif

    reg |= OP_READ;

    uint8_t packet[2] = {reg, 0x00};

    com_spi_send(card, packet, 2);

#ifdef DEBUG_COM
    shell_print(" ");
    shell_println(packet[1]);
#endif

    return packet[1];
}

int com_get_channel(int card) { return com_read_reg(card, REG_RF_CH); }

void com_set_channel(int card, int chan) { com_set_reg(card, REG_RF_CH, chan); }

int com_get_ack(int card) { return com_read_reg(card, REG_EN_AA); }
void com_set_ack(int card, bool v) {
    if (v)
        com_set_reg(card, REG_EN_AA, REG_EN_AA_ALL);
    else
        com_set_reg(card, REG_EN_AA, 0x00);
}
void com_set_ack(int card, int pipe, bool v) {
    if (v)
        com_set_reg(card, REG_EN_AA,
                    com_read_reg(card, REG_EN_AA) | (0x01 << pipe));
    else
        com_set_reg(card, REG_EN_AA,
                    com_read_reg(card, REG_EN_AA) & (~(0x01 << pipe)));
}

int com_get_retransmission_delay(int card) {
    return (com_read_reg(card, REG_SETUP_RETR) >> 4);
}
int com_get_retransmission_count(int card) {
    return (com_read_reg(card, REG_SETUP_RETR) & 0x0F);
}
void com_set_retransmission(int card, int delay, int count) {
    uint8_t v = (delay << 0x04) | (count);
    com_set_reg(card, REG_SETUP_RETR, v);
}

uint8_t com_get_config(int card) { return com_read_reg(card, REG_CONFIG); }

uint8_t com_get_fifo_status(int card) {
    return com_read_reg(card, REG_FIFO_STATUS);
}

uint8_t com_get_status(int card) { return com_read_reg(card, REG_STATUS); }
uint8_t com_get_rf_setup(int card) { return com_read_reg(card, REG_RF_SETUP); }

int com_get_pipe_payload(int card, int pipe) {
    uint8_t en = com_read_reg(card, REG_EN_RXADDR);
    switch (pipe) {
        case 0:
            return en & REG_EN_RXADDR_P0 ? com_read_reg(card, REG_RX_PW_P0)
                                         : -1;
            break;
        case 1:
            return en & REG_EN_RXADDR_P1 ? com_read_reg(card, REG_RX_PW_P1)
                                         : -1;
            break;
        case 2:
            return en & REG_EN_RXADDR_P2 ? com_read_reg(card, REG_RX_PW_P2)
                                         : -1;
            break;
        case 3:
            return en & REG_EN_RXADDR_P3 ? com_read_reg(card, REG_RX_PW_P3)
                                         : -1;
            break;
        case 4:
            return en & REG_EN_RXADDR_P4 ? com_read_reg(card, REG_RX_PW_P4)
                                         : -1;
            break;
        case 5:
            return en & REG_EN_RXADDR_P5 ? com_read_reg(card, REG_RX_PW_P5)
                                         : -1;
            break;
    }
    return 0;
}
void com_set_pipe_payload(int card, int pipe, uint8_t pl) {
    if (pl > 32) return;
    uint8_t en = com_read_reg(card, REG_EN_RXADDR);

    switch (pipe) {
        case 0:
            if (pl == 0)
                en = en & ~REG_EN_RXADDR_P0;
            else
                en |= REG_EN_RXADDR_P0;
            com_set_reg(card, REG_RX_PW_P0, pl);
            break;
        case 1:
            if (pl == 0)
                en = en & ~REG_EN_RXADDR_P1;
            else
                en |= REG_EN_RXADDR_P1;
            com_set_reg(card, REG_RX_PW_P1, pl);
            break;
        case 2:
            if (pl == 0)
                en = en & ~REG_EN_RXADDR_P2;
            else
                en |= REG_EN_RXADDR_P2;
            com_set_reg(card, REG_RX_PW_P2, pl);
            break;
        case 3:
            if (pl == 0)
                en = en & ~REG_EN_RXADDR_P3;
            else
                en |= REG_EN_RXADDR_P3;
            com_set_reg(card, REG_RX_PW_P3, pl);
            break;
        case 4:
            if (pl == 0)
                en = en & ~REG_EN_RXADDR_P4;
            else
                en |= REG_EN_RXADDR_P4;
            com_set_reg(card, REG_RX_PW_P4, pl);
            break;
        case 5:
            if (pl == 0)
                en = en & ~REG_EN_RXADDR_P5;
            else
                en |= REG_EN_RXADDR_P5;
            com_set_reg(card, REG_RX_PW_P5, pl);
            break;
    }
    com_set_reg(card, REG_EN_RXADDR, en);
}

uint8_t com_read_status(int card) {
    uint8_t packet[1] = {OP_NOP};
    com_spi_send(card, packet, 1);

    return packet[0];
}

void com_read_reg5(int card, uint8_t reg, uint8_t result[5]) {
    reg |= OP_READ;

    uint8_t packet[6] = {reg, 0x00, 0x00, 0x00, 0x00, 0x00};

    com_spi_send(card, packet, 6);

    for (int k = 1; k < 6; k++) {
        result[k - 1] = packet[k];
    }
}

static void com_mode(int card, bool power, bool rx) {
    uint8_t value = (1 << 3);  // Enable CRC

    if (power) {
        value |= (1 << 1);
    }
    if (rx) {
        value |= (1);
    }

    com_set_reg(card, REG_CONFIG, value);
}

static void com_tx(int card, uint8_t *payload, size_t n) {
    // com_txing[index] = micros();

    uint8_t packet[n + 1];
    packet[0] = OP_TX;
    for (uint8_t k = 0; k < n; k++) {
        packet[k + 1] = payload[k];
    }

    com_spi_send(card, packet, n + 1);
}

static void com_rx(int card, uint8_t *payload, size_t n) {
    uint8_t packet[n + 1] = {0};

    packet[0] = OP_RX;

    com_spi_send(card, packet, n + 1);

    for (uint8_t k = 1; k < n + 1; k++) {
        payload[k - 1] = packet[k];
    }
}

void com_flush_rx(int card) {
    uint8_t packet[1] = {OP_FLUSH_RX};
    com_spi_send(card, packet, 1);
}

void com_flush_tx(int card) {
    uint8_t packet[1] = {OP_FLUSH_TX};
    com_spi_send(card, packet, 1);
}

void com_clear_status(int card) {
    com_set_reg(card, REG_STATUS,
                REG_STATUS_MAX_RT | REG_STATUS_RX_DR | REG_STATUS_TX_DS);
}
int com_get_lost_count(int card) {
    return com_read_reg(card, REG_OBSERVE_TX) >> 4;
}
int com_get_retransmitted_count(int card) {
    return com_read_reg(card, REG_OBSERVE_TX) & 0x0F;
}

void com_start_listening(int card) {
    com_set_reg(
        card, REG_CONFIG,
        com_read_reg(card, REG_CONFIG) | CONFIG_PWR_UP | CONFIG_PRIM_RX);
}

void com_stop_listening(int card) {
    com_set_reg(
        card, REG_CONFIG,
        (com_read_reg(card, REG_CONFIG) | CONFIG_PWR_UP) & ~CONFIG_PRIM_RX);
}

int com_has_data(int card) {
    com_set_state(card, RX);
    uint8_t s = com_read_reg(card, REG_STATUS);
    uint8_t v = (s & REG_STATUS_RX_P_NO) >> 1;
    if (v == 7) return -1;
    return v;
}

void com_receive(int card, uint8_t *payload, int size) {
    com_set_state(card, RX);

    if (com_has_data(card) == -1) return;

    uint8_t packet[size + 1] = {0};

    packet[0] = OP_RX;

    com_spi_send(card, packet, size + 1);

    for (uint8_t k = 1; k < size + 1; k++) {
        payload[k - 1] = packet[k];
    }

    com_clear_status(card);
}

bool com_send(int card, uint8_t *payload, int size) {
    com_set_state(card, TX);  // we are suppose to be in standy II mode
    com_clear_status(card);
    com_tx(card, payload, size);

    uint8_t s, f;
    timer.reset();
    timer.start();
    
    do {
        s = com_read_reg(card, REG_STATUS);
        // watchdog_feed();
        f = com_read_reg(card, REG_FIFO_STATUS);
        //        shell_print("status is: ");
        //        shell_println(s);
    } while (
        ((timer.read_us()) < 15000) &&
        (((f & REG_FSTAT_TX_EMPTY) != 0) ||
         (((s & REG_STATUS_TX_DS) == 0) && ((s & REG_STATUS_MAX_RT) == 0))));
    
    timer.stop();

    if ((f & REG_FSTAT_TX_EMPTY) != 0) {
        com_flush_tx(card);
    }
    //    shell_print("status is: ");
    //    print_byte_as_hex(s);
    //    shell_println();
    com_clear_status(card);
    if ((s & REG_STATUS_TX_DS) != 0) return true;
    return false;
}

/*
 * Actually com_irq is used in com_poll() and not call in an irq !
 *
 * BE CAREFULL ! If you activate the IRQ MODE, you have to manage concurrency
 * with resume_boost() and pause_boost(). See kicker.cpp for more details.
 *
 */
// bool com_irq(int index)
//{
//    if ((micros()-com_txing[index]) < 300) {
//        return true;
//    }

//    int fifo = com_read_reg(index, REG_FIFO_STATUS);

//    // Checking that the module is present (bit 7 should be always 0)
//    if (fifo != 0xff) {
//        if ((fifo & RX_EMPTY) == 0) { // RX
//            com_available[index] = true;

//            if (com_master) {
//                // Receiving a status packet from a robot
//                struct packet_robot packet;
//                com_rx(index, (uint8_t*)&packet, sizeof(struct packet_robot));

//                if (packet.id < MAX_ROBOTS) {
//                    com_has_status[packet.id] = true;
//                    com_statuses[packet.id] = packet;
//                    com_robot_reception[packet.id] = millis();
//                }
//            } else {
//                // Receiving an instruction packet from the master
//                com_master_reception = millis();
//                com_master_new = true;
//                com_master_packets++;
//                com_master_channel_packets[index]++;
//                com_rx(index, com_master_frame, PACKET_SIZE);
//            }
//        }
//        if (com_txing[index] && ((fifo & TX_EMPTY) != 0)) { // TX is over
//            com_txing[index] = 0;
//            com_mode(index, true, true);
//        }

//        return true;
//    }

//    return false;
//}

void com_get_tx_addr(int index, uint8_t addr[5]) {
    com_read_reg5(index, REG_TX_ADDR, addr);
}

void com_set_tx_addr(int index, uint8_t add[5]) {
    com_set_reg5(index, REG_TX_ADDR, add);
}

void com_set_rx_addr(int index, int pipe, uint8_t addr[5]) {
    switch (pipe) {
        case 0:
            com_set_reg5(index, REG_RX_ADDR_P0, addr);
            break;
        case 1:
            com_set_reg5(index, REG_RX_ADDR_P1, addr);
            break;
        case 2:
            com_set_reg(index, REG_RX_ADDR_P2, addr[0]);
            break;
        case 3:
            com_set_reg(index, REG_RX_ADDR_P3, addr[0]);
            break;
        case 4:
            com_set_reg(index, REG_RX_ADDR_P4, addr[0]);
            break;
        case 5:
            com_set_reg(index, REG_RX_ADDR_P5, addr[0]);
            break;
    }
}

void com_get_rx_addr(int index, int pipe, uint8_t addr[5]) {
    if (pipe == 0)
        com_read_reg5(index, REG_RX_ADDR_P0, addr);
    else if (pipe >= 1) {
        com_read_reg5(index, REG_RX_ADDR_P1, addr);
        uint8_t last = addr[0];
        if (pipe == 2) last = com_read_reg(index, REG_RX_ADDR_P2);
        if (pipe == 3) last = com_read_reg(index, REG_RX_ADDR_P3);
        if (pipe == 4) last = com_read_reg(index, REG_RX_ADDR_P4);
        if (pipe == 5) last = com_read_reg(index, REG_RX_ADDR_P5);
        addr[0] = last;
    }
}
void com_ce_enable(int index) {
    //    shell_println("enable ce");
    ce[index] = HIGH;
    wait_us(10);
}

void com_ce_disable(int index) {
    ce[index] = LOW;
    wait_us(10);
    // We are not suppose to disable CE, this is only for power reduction
    // keep com cards in standby II mode

    //    shell_println("disable ce");
}

bool com_is_ok(int card) {
    // we store actual address
    uint8_t old[5];
    com_get_tx_addr(card, old);
    uint8_t test[5] = {0x01, 0xBB, 0x4C, 0x5D, 0x33};
    com_set_tx_addr(card, test);
    uint8_t res[5];
    com_get_tx_addr(card, res);
    // Checking only address prefix
    for (int k = 0; k < 4; k++) {
        if (test[k] != res[k]) {
            return false;
        }
    }
    // restore tx addr
    com_set_tx_addr(card, old);

    return true;
}

bool com_is_all_ok() { return com_is_ok(0) && com_is_ok(1) && com_is_ok(2); }

void com_init() {
    com.format(8,0);
    com.frequency(2250000);  // com.begin(SPI_2_25MHZ, MSBFIRST, 0);

    // set default state for all cards:
    // this setup is a copy from arduino rf24l01+ library
    for (uint8_t k = 0; k < 3; k++) {
        com_ce_enable(k);
        // 2 bytes CRC and disable all IRQ
        com_set_config(k, CONFIG_MASK_MAX_RT | CONFIG_MASK_RX_DR |
                              CONFIG_MASK_TX_DS | CONFIG_EN_CRC | CONFIG_CRCO);
        com_set_retransmission(k, 0, 15);
        // com_set_retransmission(k,0,1);
        com_set_reg(k, REG_RF_SETUP, 0x21);
        com_set_reg(k, REG_RF_SETUP, 0x01);
        com_set_reg(k, REG_FEATURE, 0x00);
        com_set_reg(k, REG_SETUP_AW, 0x03);   // set address on 5 bytes
        com_set_reg(k, REG_EN_RXADDR, 0x01);  // disable all pipe except 0
        com_set_reg(k, REG_DYNPD, 0x00);
        com_clear_status(k);
        com_set_channel(k, 76);
        com_flush_rx(k);
        com_flush_tx(k);
        com_ce_disable(k);  // leave down for standby II mode
        // com_set_rf(k,SPEED_250k,POW_18db); // low speed, low power
        com_set_rf(k, SPEED_2M, POW_0db);
        com_power(k, false);  // turn off the card
        com_current_state[k] = OFF;
    }
}

void com_power(int card, bool up) {
    if (up)
        com_set_config(card, com_get_config(card) | CONFIG_PWR_UP);
    else
        com_set_config(card, com_get_config(card) & ~CONFIG_PWR_UP);
}

void com_set_rf(int card, uint8_t speed, uint8_t pow) {
    com_set_reg(card, REG_RF_SETUP, speed | pow);
}

void com_set_config(int card, uint8_t v) { com_set_reg(card, REG_CONFIG, v); }
void com_set_crc(int card, int crc) {
    if (crc == 0)
        com_set_config(card, com_get_config(card) & ~CONFIG_EN_CRC);
    else if (crc == 1)
        com_set_config(card,
                       (com_get_config(card) | CONFIG_EN_CRC) & ~CONFIG_CRCO);
    else
        com_set_config(card,
                       com_get_config(card) | CONFIG_EN_CRC | CONFIG_CRCO);
}

SHELL_COMMAND(ci, "CI") { com_init(); }

void print_config(int k) {
    shell_print(k);
    shell_print(":");
    uint8_t conf = com_get_config(k);
    if (conf & CONFIG_PWR_UP)
        shell_print(" UP   ");
    else
        shell_print(" down ");

    if (conf & CONFIG_EN_CRC)
        shell_print(" CRC ");
    else
        shell_print(" crc ");
    if (conf & CONFIG_CRCO)
        shell_print(" CRCO ");
    else
        shell_print(" crco ");
    if (conf & CONFIG_MASK_MAX_RT)
        shell_print(" MAXRT ");
    else
        shell_print(" maxrt ");
    if (conf & CONFIG_MASK_RX_DR)
        shell_print(" RXDR ");
    else
        shell_print(" rxdr ");
    if (conf & CONFIG_MASK_TX_DS)
        shell_print(" TXDS ");
    else
        shell_print(" txds ");
    if (conf & CONFIG_PRIM_RX)
        shell_print(" PRIMRX ");
    else
        shell_print(" primrx ");
    shell_println("");
}

SHELL_COMMAND(config, "Display ack support") {
    if (argc == 1)
        print_config(atoi(argv[0]));
    else
        for (int k = 0; k < 3; ++k) print_config(k);
}

void print_addr_term(int k) {
    shell_print(k);
    shell_print(" : width ");
    shell_println(com_read_reg(k, REG_SETUP_AW) + 2);
    shell_print("  tx:   ");
    uint8_t a[5];
    com_get_tx_addr(k, a);
    print_addr(a);
    shell_println();
    for (int pipe = 0; pipe < 6; pipe++) {
        shell_print("  rx(");
        shell_print(pipe);
        shell_print(") - ");
        shell_print(com_get_pipe_payload(k, pipe));
        shell_print(" : ");
        uint8_t a[5];
        com_get_rx_addr(k, pipe, a);
        print_addr(a);
        shell_println();
    }
}

SHELL_COMMAND(addr, "display or set com address: addr [id] nb.nb.nb.nb.nb") {
    if (argc == 0) {
        for (int k = 0; k < 3; ++k) print_addr_term(k);
    } else if (argc == 1) {
        print_addr_term(atoi(argv[0]));
    } else if (argc != 2) {
        shell_println("invalid number of args");
    } else {
        int id = atoi(argv[0]);
        uint8_t a[5];
        int l = 0;
        int s = 0;
        int k = 0;
        while ((k < 5) && (argv[1][l] != '\0')) {
            if (argv[1][l] == '.') {
                argv[1][l] = '\0';
                a[k] = atoi(argv[1] + s);
                s = l + 1;
                k += 1;
            }
            l += 1;
        }
        if (k != 4) {
            shell_println("invalid address");
        } else {
            a[k] = atoi(argv[1] + s);
            com_set_tx_addr(id, a);
            com_set_rx_addr(id, 0, a);
        }
    }
}

void print_feature(int k) {
    shell_print(k);
    shell_print(":");
    uint8_t fea = com_read_reg(k, REG_FEATURE);
    print_byte_as_hex(fea);
    shell_println();
}
SHELL_COMMAND(fea, "display feature reg") {
    if (argc == 1)
        print_feature(atoi(argv[0]));
    else
        for (int k = 0; k < 3; ++k) print_feature(k);
}

void print_dynp(int k) {
    shell_print(k);
    shell_print(":");
    uint8_t fea = com_read_reg(k, REG_DYNPD);
    print_byte_as_hex(fea);
    shell_println();
}
SHELL_COMMAND(dynp, "display dynp reg") {
    if (argc == 1)
        print_dynp(atoi(argv[0]));
    else
        for (int k = 0; k < 3; ++k) print_dynp(k);
}
void print_rpd(int k) {
    shell_print(k);
    shell_print(":");
    uint8_t fea = com_read_reg(k, REG_RPD);
    print_byte_as_hex(fea);
    shell_println();
}
SHELL_COMMAND(rpd, "display rpd reg") {
    if (argc == 1)
        print_rpd(atoi(argv[0]));
    else
        for (int k = 0; k < 3; ++k) print_rpd(k);
}

SHELL_COMMAND(rst, "reset status rst [card]") {
    if (argc == 0) {
        for (int k = 0; k < 3; ++k) com_clear_status(k);
    } else {
        int c = atoi(argv[1]);
        com_clear_status(c);
    }
}

void print_setup(int k) {
    shell_print(k);
    shell_print(":");
    uint8_t s = com_get_rf_setup(k);

    if (s & REG_RF_CONT_WAVE)
        shell_print(" CONT ");
    else
        shell_print(" cont ");
    if (s & REG_RF_DR_LOW)
        shell_print(" DRLOW ");
    else
        shell_print(" drlow ");
    if (s & REG_RF_DR_LOW)
        shell_print(" PLL ");
    else
        shell_print(" pll ");
    if (s & REG_RF_DR_HIGH)
        shell_print(" DRHIG ");
    else
        shell_print(" drhig ");
    shell_print(s & REG_RF_PWR >> 1);
    if (s & REG_RF_DR_LOW) {
        if (s & REG_RF_DR_HIGH)
            shell_print(" error in data rate ");
        else
            shell_print(" 250kbps ");
    } else {
        if (s & REG_RF_DR_HIGH)
            shell_print(" 2Mbps ");
        else
            shell_print(" 1Mbps ");
    }
    if ((s & REG_RF_PWR >> 1) == 0)
        shell_print(" -18dBm ");
    else if ((s & REG_RF_PWR >> 1) == 1)
        shell_print(" -12dBm ");
    else if ((s & REG_RF_PWR >> 1) == 2)
        shell_print(" -6dBm ");
    else if ((s & REG_RF_PWR >> 1) == 3)
        shell_print(" 0dBm ");
    shell_println();
}

SHELL_COMMAND(setup, "display rf setup") {
    if (argc == 1)
        print_setup(atoi(argv[0]));
    else
        for (int k = 0; k < 3; ++k) print_setup(k);
}

void print_status(int k) {
    shell_print(k);
    shell_print(": [STAT] ");
    uint8_t c = com_get_status(k);
    if (c & REG_STATUS_RX_DR)
        shell_print(" RXDR ");
    else
        shell_print(" rxdr ");
    if (c & REG_STATUS_TX_DS)
        shell_print(" TXDS ");
    else
        shell_print(" txds ");
    if (c & REG_STATUS_MAX_RT)
        shell_print(" MRT ");
    else
        shell_print(" mrt ");
    shell_print((c & REG_STATUS_RX_P_NO) >> 1);
    if (c & REG_STATUS_TX_FULL)
        shell_print(" FULL ");
    else
        shell_print(" full ");
    shell_println();
    shell_print(" : [FIFO] ");
    c = com_get_fifo_status(k);
    if (c & REG_FSTAT_TX_REUSE)
        shell_print(" TXR ");
    else
        shell_print(" txr ");
    if (c & REG_FSTAT_TX_FULL)
        shell_print(" TXFULL ");
    else
        shell_print(" txfull ");
    if (c & REG_FSTAT_TX_EMPTY)
        shell_print(" TXEMPTY ");
    else
        shell_print(" txempty ");
    if (c & REG_FSTAT_RX_FULL)
        shell_print(" RXFULL ");
    else
        shell_print(" rxfull ");
    if (c & REG_FSTAT_RX_EMPTY)
        shell_print(" RXEMPTY ");
    else
        shell_print(" rxempty ");
    shell_println();
}

SHELL_COMMAND(status, "status") {
    if (argc == 0)
        for (int k = 0; k < 3; ++k) print_status(k);
    else
        print_status(atoi(argv[0]));
}

void print_chan(int k) {
    shell_print(k);
    shell_print(":");
    shell_println(com_get_channel(k));
}

SHELL_COMMAND(chan, "Display or set used channel: chan [id] [chan]") {
    if (argc == 0) {
        for (int k = 0; k < 3; ++k) print_chan(k);
    } else if (argc == 1) {
        print_chan(atoi(argv[0]));
    } else {
        int id = atoi(argv[0]);
        uint8_t chan = atoi(argv[1]);
        com_set_channel(id, chan);
    }
}

void print_ack(int k) {
    shell_print(k);
    shell_print(":");
    print_byte_as_hex(com_get_ack(k));
    shell_println();
}

SHELL_COMMAND(ack, "Display or set ack support: ack card [O|1]") {
    if (argc == 0) {
        for (int k = 0; k < 3; ++k) print_ack(k);
    } else if (argc == 1) {
        print_ack(atoi(argv[0]));
    } else if (argc != 2) {
        shell_println("wrong arg");
    } else {
        int c = atoi(argv[0]);
        bool b = atoi(argv[1]);
        com_set_ack(c, b);
    }
}

SHELL_COMMAND(recv, "check if data received") {
    for (int k = 0; k < 3; ++k) {
        shell_print(k);
        shell_print(": ");
        int data = com_has_data(k);
        shell_print(data);
        if (data >= 0) {
            char pl[32];
            com_receive(k, (uint8_t *)pl, 32);
            shell_print(" data[");
            shell_print(pl);
            shell_print("]");
            com_flush_rx(k);
            com_clear_status(k);
        }
        com_clear_status(k);
        shell_println();
    }
}

SHELL_COMMAND(send, "send card msg") {
    if (argc != 2)
        shell_println("wrong number of args");
    else {
        int card = atoi(argv[0]);
        int s = com_get_pipe_payload(card, 0);
        shell_print("payload is ");
        shell_println(s);
        char payload[32] = {'\0'};
        for (int i = 0; i < 32 && argv[1][i] != '\0'; ++i)
            payload[i] = argv[1][i];
        com_send(card, (uint8_t *)payload, s);
    }
}

void print_retr(int card) {
    shell_print(card);
    shell_print(": ");
    int d = com_get_retransmission_delay(card);
    shell_print(d);
    shell_print(" (");
    shell_print(250 + 250 * d);
    shell_print(") ");
    shell_println(com_get_retransmission_count(card));
}

SHELL_COMMAND(retr, "Display or set retransmission: retr card delay count") {
    if (argc == 0) {
        for (int k = 0; k < 3; ++k) {
            print_retr(k);
        }
    } else if (argc == 1) {
        print_retr(atoi(argv[0]));
    } else if (argc != 3) {
        shell_println("wrong arg");
    } else {
        int c = atoi(argv[0]);
        int b = atoi(argv[1]);
        int f = atoi(argv[2]);
        com_set_retransmission(c, b, f);
    }
}

void print_csnce(int card) {
    shell_print("card ");
    shell_print(card);
    shell_print(" csn: ");
    if (cs[card] == LOW)
        shell_print(" low ");
    else
        shell_print(" high ");
    shell_print(" ce: ");
    if (ce[card] == LOW)
        shell_print(" low ");
    else
        shell_print(" high ");

    shell_println();
}

SHELL_COMMAND(csnce, "get csn pin and ce pin status") {
    if (argc == 1)
        print_csnce(atoi(argv[0]));
    else if (argc == 2) {
        int c = atoi(argv[0]);
        int s = atoi(argv[1]);
        if (s == 0)
            com_ce_disable(c);
        else
            com_ce_enable(c);
    } else
        for (int i = 0; i < 3; ++i) print_csnce(i);
}

void print_obs(int card) {
    shell_print(card);
    shell_print(": ");
    shell_print(com_get_lost_count(card));
    shell_print(" lost ");
    shell_print(com_get_retransmitted_count(card));
    shell_println(" retransmitted ");
}

SHELL_COMMAND(obs, "display transmission stats") {
    if (argc == 1)
        print_obs(atoi(argv[0]));
    else
        for (int k = 0; k < 3; ++k) print_obs(k);
}

static void print_rxp(int card) {
    shell_print(card);
    shell_print(": ");
    for (int pipe = 0; pipe < 6; pipe++) {
        shell_print(pipe);
        int pl = com_get_pipe_payload(card, pipe);
        if (pl == 0)
            shell_print("[disabled] ");
        else {
            shell_print("[");
            shell_print(pl);
            shell_print("] ");
        }
    }
    shell_println();
}

SHELL_COMMAND(rxp, "display or set pipe payload, 0 means not active") {
    if (argc == 0) {
        for (int k = 0; k < 3; ++k) print_rxp(k);
    } else if (argc == 1) {
        print_rxp(atoi(argv[0]));
    } else if (argc != 3) {
        shell_println("wrong number of arguments: card pipe payloadsize");
    } else {
        int c = atoi(argv[0]);
        int p = atoi(argv[1]);
        int pl = atoi(argv[2]);
        com_set_pipe_payload(c, p, pl);
    }
}

SHELL_COMMAND(flush, "flush rx and tx for all cards") {
    for (int i = 0; i < 3; ++i) {
        com_flush_rx(i);
        com_flush_tx(i);
    }
}

SHELL_COMMAND(st, "get or set card state: 0 off , 1 rx, 2 tx") {
    if (argc == 0)
        for (int k = 0; k < 3; ++k) {
            shell_print(k);
            shell_print(" : ");
            shell_print(state_change[k]);
            if (com_current_state[k] == OFF)
                shell_println(" OFF ");
            else if (com_current_state[k] == RX)
                shell_println(" RX ");
            else
                shell_println(" TX ");
        }
    else {
        int c = atoi(argv[0]);
        int s = atoi(argv[1]);
        if (s == 0)
            com_set_state(c, OFF);
        else if (s == 1)
            com_set_state(c, RX);
        else if (s == 2)
            com_set_state(c, TX);
    }
}

SHELL_COMMAND(dbg, "dump all informations") {
    int n = 0;
    char *arg[] = {0, 0};
    if (argc == 1) {
        n += 1;
        arg[0] = argv[0];
    }

    shell_println("csnce:");
    shell_command_csnce(n, arg);
    shell_println("config:");
    shell_command_config(n, arg);
    shell_println("ack:");
    shell_command_ack(n, arg);
    shell_println("addr:");
    shell_command_addr(n, arg);
    shell_println("retr:");
    shell_command_retr(n, arg);
    shell_println("channel:");
    shell_command_chan(n, arg);
    shell_println("setup:");
    shell_command_setup(n, arg);
    shell_println("status:");
    shell_command_status(n, arg);
    shell_println("observ:");
    shell_command_obs(n, arg);
    shell_println("pipe pyload:");
    shell_command_rxp(n, arg);
    shell_println("feature:");
    shell_command_fea(n, arg);
    shell_println("dynp:");
    shell_command_dynp(n, arg);
    shell_println("rpd:");
    shell_command_rpd(n, arg);
}

void com_copy_addr(uint8_t dst[5], uint8_t src[5]) {
    for (int i = 0; i < 5; ++i) dst[i] = src[i];
}

int com_cmp_addr(uint8_t dst[5], uint8_t src[5]) {
    for (int i = 0; i < 5; ++i)
        if (dst[i] != src[i]) return 1;
    return 0;
}

void com_self_diag(int cardA, int cardB, int &acked, int &not_acked,
                   int &received) {
    uint8_t addr[5] = {0xAF, 0x1E, 0x2B, 0x38, 0x88};
    com_set_state(0, OFF);
    com_set_state(1, OFF);
    com_set_state(2, OFF);
    com_set_tx_addr(cardA, addr);
    com_set_rx_addr(cardA, 0, addr);
    com_set_tx_addr(cardB, addr);
    com_set_rx_addr(cardB, 0, addr);
    com_set_pipe_payload(cardA, 0, 32);
    com_set_pipe_payload(cardB, 0, 32);
    com_set_ack(cardA, 0, true);
    com_set_ack(cardB, 0, true);
    com_set_channel(cardA, 105);
    com_set_channel(cardB, 105);
    com_set_state(cardA, TX);
    com_set_state(cardB, RX);
    uint8_t msg[32];
    for (int i = 0; i < 32; ++i) {
        msg[i] = i;
    }
    acked = 0;
    not_acked = 0;
    received = 0;
    for (int i = 0; i < 50; ++i) {
        if (com_send(cardA, msg, 32)) {
            acked += 1;
        } else {
            not_acked += 1;
        }
        if (com_has_data(cardB) > -1) {
            com_receive(cardB, msg, 32);
            received += 1;
        }
    }

    com_set_state(0, OFF);
    com_set_state(1, OFF);
    com_set_state(2, OFF);
}

void com_full_diag(float cards[3]) {
    // perform a full diag on card, two by two...
    // card status after diag is undefined, dont forget
    // to restore cards status.
    // return value is between 0 and 1, 0 means faulty card, 1 ok
    float work[3] = {0, 0, 0};
    int acked, not_acked, received;

    for (int cardA = 0; cardA < 3; cardA++)
        for (int cardB = 0; cardB < 3; cardB++) {
            if (cardA != cardB) {
                // check A => B connection with ACK support
                com_self_diag(cardA, cardB, acked, not_acked, received);
                // possible cases:
                // not_acked=0 acked=received=max   => everything ok
                // not_acked=received=max acked=0   => cardA send data, receive
                // by cardB,
                //                                     either cardB do not
                //                                     reply, either cardA does
                //                                     not receive (possibly
                //                                     both)
                // acked=0 received=0 not_acked=max => nothing work, can be one
                // or both of cards that are broken
                float sent = acked + not_acked;
                if ((not_acked < (0.1 * sent)) && (received > (0.9 * sent))) {
                    work[cardA] += 1.0;
                    work[cardB] += 1.0;
                }
            }
        }

    for (int card = 0; card < 3; card++) {
        // analysis card perf:
        // card is in fault if err is above 0.1
        cards[card] = work[card] / 4.0;
    }
}

SHELL_COMMAND(com_diag, "diag com cards") {
    int acked, not_acked, received;
    if (argc == 2) {
        shell_print("diag from card");

            int cardA = atoi(argv[0]);
        int cardB = atoi(argv[1]);
        com_self_diag(cardA, cardB, acked, not_acked, received);
        shell_print("diag from card ");
        shell_print(cardA);
        shell_print(" => ");
        shell_print(cardB);
        shell_print(" acked: ");
        shell_print(acked);
        shell_print(" not acked: ");
        shell_print(not_acked);
        shell_print(" received: ");
        shell_println(received);
    } else
        for (int cardA = 0; cardA < 3; cardA++)
            for (int cardB = 0; cardB < 3; cardB++) {
                if (cardA != cardB) {
                    acked = not_acked = received = 0;
                    com_self_diag(cardA, cardB, acked, not_acked, received);
                    shell_print("diag from card ");
                    shell_print(cardA);
                    shell_print(" => ");
                    shell_print(cardB);
                    shell_print(" acked: ");
                    shell_print(acked);
                    shell_print(" not acked: ");
                    shell_print(not_acked);
                    shell_print(" received: ");
                    shell_println(received);
                }
            }
}