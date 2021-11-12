#pragma once

#define TX_FULL         (1<<5)
#define TX_EMPTY        (1<<4)
#define RX_FULL         (1<<1)
#define RX_EMPTY        (1<<0)

// Operations
#define OP_READ         0x00
#define OP_WRITE        0x20
#define OP_RX           0x61
#define OP_TX           0xa0
#define OP_TX_NOACK     0xb0
#define OP_FLUSH_RX     0xE2
#define OP_FLUSH_TX     0xE1
#define OP_NOP          0xff

// CONFIG FLAGS
#define CONFIG_MASK_RX_DR  (0x01<<6)
#define CONFIG_MASK_TX_DS  (0x01<<5)
#define CONFIG_MASK_MAX_RT (0x01<<4)
#define CONFIG_EN_CRC      (0x01<<3)
#define CONFIG_CRCO        (0x01<<2)
#define CONFIG_PWR_UP      (0x01<<1)
#define CONFIG_PRIM_RX     (0x01<<0)

// Auto ACK for all pipes:
#define REG_EN_AA_ALL 0x3F

// Enable PIPEs:
#define REG_EN_RXADDR_P0 0x1
#define REG_EN_RXADDR_P1 (0x1<<1)
#define REG_EN_RXADDR_P2 (0x1<<2)
#define REG_EN_RXADDR_P3 (0x1<<3)
#define REG_EN_RXADDR_P4 (0x1<<4)
#define REG_EN_RXADDR_P5 (0x1<<5)

// STATUS
#define REG_STATUS_ZERO     0x80 // ???
#define REG_STATUS_RX_DR    0x40
#define REG_STATUS_TX_DS    0x20
#define REG_STATUS_MAX_RT   0x10
#define REG_STATUS_RX_P_NO  0x0E
#define REG_STATUS_TX_FULL  0x01

// FIFO STATUS
#define REG_FSTAT_TX_REUSE   0x40
#define REG_FSTAT_TX_FULL    0x20
#define REG_FSTAT_TX_EMPTY   0x10
#define REG_FSTAT_RX_FULL    0x02
#define REG_FSTAT_RX_EMPTY   0x01

// RFSETUP
#define REG_RF_CONT_WAVE 0x80
#define REG_RF_DR_LOW    0x20
#define REG_RF_PLL       0x10
#define REG_RF_DR_HIGH   0x08
#define REG_RF_PWR       0x06

// Registers
#define REG_CONFIG      0x00
#define REG_EN_AA       0x01
#define REG_EN_RXADDR   0x02
#define REG_SETUP_AW    0x03
#define REG_SETUP_RETR  0x04
#define REG_RF_CH       0x05
#define REG_RF_SETUP    0x06
#define REG_STATUS      0x07
#define REG_OBSERVE_TX  0x08
#define REG_RPD         0x09
#define REG_RX_ADDR_P0  0x0a  // 5 bytes
#define REG_RX_ADDR_P1  0x0b  // 5 bytes
#define REG_RX_ADDR_P2  0x0c
#define REG_RX_ADDR_P3  0x0d
#define REG_RX_ADDR_P4  0x0e
#define REG_RX_ADDR_P5  0x0f
#define REG_TX_ADDR     0x10 // 5 bytes
#define REG_RX_PW_P0    0x11
#define REG_RX_PW_P1    0x12
#define REG_RX_PW_P2    0x13
#define REG_RX_PW_P3    0x14
#define REG_RX_PW_P4    0x15
#define REG_RX_PW_P5    0x16
#define REG_FIFO_STATUS 0x17
#define REG_DYNPD       0x1c
#define REG_FEATURE     0x1d
