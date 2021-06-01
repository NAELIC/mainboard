#ifndef _HARDWARE_H
#define _HARDWARE_H

#include "mbed.h"

// Brushless drivers
#define DRIVERS_CS1 SPI1_CS
#define DRIVERS_CS2 PA_10
#define DRIVERS_CS3 PA_9
#define DRIVERS_CS4 PB_0
#define DRIVERS_CS5 PB_1

#define DRV_MOSI SPI1_MOSI
#define DRV_MISO SPI1_MISO
#define DRV_CLK SPI1_SCK

// Multiplexer
#define MUX         DIO9

#define ADDR1       DIO11
#define ADDR2       DIO13
#define ADDR3       DIO14

// Communication pins

#define COM_MOSI DIO8
#define COM_MISO DIO7
#define COM_CLK DAC1


// // D7 "CSn" du transmetteur
// #define SPI1_CS1 PA_4
// // D8 "CSn" de l'émetteur
// #define SPI1_CS2 PA_3
// // D9 "CE" du transmetteur
// #define CE1 PA_2
// // D10 "CE" de l'émetteur
// #define CE2 PA_1
// // D11
// #define INT PA_0

#define COM_CE1  DIO4
#define COM_CE2  ADC1
#define COM_CE3  ADC2

#define COM_CS1 DIO1
#define COM_CS2 DIO2
#define COM_CS3 DIO3

#define COM_IRQ1 ADC4
#define COM_IRQ2 DIO5
#define COM_IRQ3 SPI3_SCK

#define COM_SPI 2

// Communication configuration
// #define COM_ADDR   {0x22, 0x87, 0xe8, 0xf9, 0x00}
// #define COM_MASTER 0xff

// Buzzer
#define BUZZER_PIN      DIO12 // Pin Buzer
#define BUZZER_TIMER    2  // Inutile ?

// Booster
// #define BOOSTER_PIN     27
// #define BOOSTER_TIMER   1
// #define CAP_ADDR        4
// #define CAP_R1          200000.0
// #define CAP_R2          2000.0
// #define CHARGE_CUR_ADDR 6
// #define KICKER_CHARGING_VALUE 150.0

// Hall sensor
// #define HALL1_ADDR      0
// #define HALL2_ADDR      1
// #define HALL3_ADDR      2
// #define HALL4_ADDR      3

#define HALL_THRES     2000

// Kicker
// #define KICKER1_PIN     25
// #define KICKER2_PIN     26
// #define KICKER_TIMER    4

// Voltage
#define BAT_ADDR        5
#define BAT_R1          10000
#define BAT_R2          1000

// Infrared
#define IR_EMIT         DIO10   // IR_LED 
#define IR_RECEIVE      ADC_IN3 //IR_DET_A on datasheet
#define IR_THRESHOLD    700

#endif
