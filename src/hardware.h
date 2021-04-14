#ifndef _HARDWARE_H
#define _HARDWARE_H

#define GREG 1
#define CATIE 2

#ifdef CATIE_BOARD
  #define BOARD CATIE
#endif
#ifdef GREG_BOARD
  #define BOARD GREG
#endif
#ifndef BOARD
  static_assert(false);
#endif

// Brushless drivers
#define DRIVERS_CS1 7
#define DRIVERS_CS2 1
#define DRIVERS_CS3 19
#define DRIVERS_CS4 18
#define DRIVERS_CS5 17

#define DRIVERS_SPI 1

// Multiplexer
#define MUX         8

#define ADDR1       14
#define ADDR2       31
#define ADDR3       2

// Communication pins
#define COM_CE1  13
#define COM_CE2  3
#define COM_CE3  9

#define COM_CS1 20
#define COM_CS2 21
#define COM_CS3 22

#define COM_IRQ1 23
#define COM_IRQ2 24
#define COM_IRQ3 0

#define COM_SPI 2

// Communication configuration
#define COM_ADDR   {0x22, 0x87, 0xe8, 0xf9, 0x00}
#define COM_MASTER 0xff

// Buzzer
#define BUZZER_PIN      11
#define BUZZER_TIMER    2

// Booster
#define BOOSTER_PIN     27
#define BOOSTER_TIMER   1
#define CAP_ADDR        4
#define CAP_R1          200000.0
#define CAP_R2          2000.0
#define CHARGE_CUR_ADDR 6
#define KICKER_CHARGING_VALUE 150.0

// Hall sensor
#define HALL1_ADDR      0
#define HALL2_ADDR      1
#define HALL3_ADDR      2
#define HALL4_ADDR      3

#define HALL_THRES     2000

// Kicker
#define KICKER1_PIN     25
#define KICKER2_PIN     26
#define KICKER_TIMER    4

// Voltage
#define BAT_ADDR        5
#define BAT_R1          10000
#define BAT_R2          1000

// Infrared
#define IR_EMIT         12
#define IR_RECEIVE      10
#define IR_THRESHOLD    600

#endif
