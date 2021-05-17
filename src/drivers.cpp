#include <mbed.h>
#include <shell.h>
#include "hardware.h"
#include "drivers.h"
#include "buzzer.h"
#include <USBSerial.h>
// #include "kicker.h"
#include "errors.h"

SPI drivers(DRV_MOSI, DRV_MISO, DRV_CLK ); //Declaration of SPI
// USBSerial serial; //Declaration of USBSerial

static bool drivers_is_error = false;
static bool drivers_present[5] = {false};
struct driver_packet_ans driver_answers[5];

static int drivers_pins[5] = {
    DRIVERS_CS1, DRIVERS_CS2, DRIVERS_CS3,
    DRIVERS_CS4, DRIVERS_CS5
};

USBSerial serial;

void pause_boost() //recreation de fonction qui est normalement ecrite differament dans kikker.cpp
{
    wait_us(100);
}

void resume_boost()//recreation de fonction qui est normalement ecrite differament dans kikker.cpp
{
    wait_us(100);
}




void drivers_init()
{
  cstatic_assert(
    sizeof(driver_packet_set) > sizeof(driver_packet_ans), //Compare les longueur en bits 
    "In SPI, packet answer have to be strictly smaller than the resquest packet"
    );

  // Initializing SPI
  drivers.frequency(250000); //set the frequency of the drivers


  // Initializing CS pins
  for (int k=0; k<5; k++) {
    drivers_pins[k] = 1; // Set the pins in high mode
  }

  for (int k=0; k<5; k++) {
    bool ret=false;
    while(!ret){
    //   BOARD_LED_PIN = 1;  // I don't fuckin know what is BOARD_LED_PIN, never define
      ret=drivers_ping(k);
      wait_us(10);
    //   watchdog_feed();      // for now we don't use the watchdog
    //   BOARD_LED_PIN = 1;
    }
    drivers_present[k] = ret;
  }
}



uint8_t drivers_status(int index)
{
    pause_boost();   //
    drivers_pins[index] = 0; // Set des pin en low
    wait_us(35);    //pause 
    drivers.write(0);
    uint8_t answer = drivers.write(0); 
    wait_us(5);      //pause
    drivers_pins[index] = 1; // Set des pin en high
    resume_boost();

    return answer;
}

int drivers_ping(int index)
{
    uint8_t status = drivers_status(index);
    return (status == 0x55 || ((status&0xf0) == 0x80));
}

static void drivers_send(int index, uint8_t instruction, uint8_t *data, size_t len, uint8_t *answer)
{
    pause_boost();
    drivers_pins[index] = 0;
    wait_us(35);

    drivers.write(instruction);

    for (uint8_t k=0; k < len; k++) {
      if (answer == NULL) {
        drivers.write(data[k]);
      } else {
        *(answer++) = drivers.write(data[k]);
      }
    }
    wait_us(5);
    drivers_pins[index] = 1;
    resume_boost();
}

#define REVERSE_TURN
struct driver_packet_ans drivers_set(int index, bool enable, float target, int16_t pwm)
{
    struct driver_packet_set packet;
    packet.enable = enable;
#ifdef REVERSE_TURN
    packet.targetSpeed = -target;
#else
    packet.targetSpeed = target;
#endif
    packet.pwm = pwm;
    packet.padding1 = 0;
    packet.padding2 = 0;

    struct driver_packet_ans answer;
    drivers_send(index, DRIVER_PACKET_SET, (uint8_t*)&packet, sizeof(struct driver_packet_set), (uint8_t*)&answer);
    return answer;
}

void drivers_set_params(float kp, float ki, float kd)
{
    for (int index = 0; index < 5; index++) {
        struct driver_packet_params packet;
        packet.kp = kp;
        packet.ki = ki;
        packet.kd = kd;

        drivers_send(index, DRIVER_PACKET_PARAMS, (uint8_t*)&packet, sizeof(struct driver_packet_params), NULL);
    }
}

void drivers_set_safe(int index, bool enable, float target, int16_t pwm)
{
    if (!drivers_is_error && drivers_present[index]) {
        struct driver_packet_ans tmp = drivers_set(index, enable, target, pwm);

        if ((tmp.status & 0xf0) == 0x80) {
            driver_answers[index] = tmp;
            for (int k=0; k<5; k++) {
                drivers_set(k, false, 0.0);
            }

            drivers_is_error = true;
            buzzer_play(MELODY_WARNING);
            // terminal_io()->println("Error on driver:");
            // terminal_io()->println(index);
            // terminal_io()->println(driver_answers[index].status&0xf);
        } else if (tmp.status == 0x55) {
            driver_answers[index] = tmp;
        }
    }
}



void drivers_tick()
{
    if (drivers_is_error) {
        if (buzzer_is_playing()) {
            for (int k=0; k<5; k++) {
                drivers_set(k, false, 0.0);
            }
        } else {
            drivers_is_error = false;
        }
    }
}







bool drivers_is_all_ok()
{
    // XXX: We are only looking for the 4th firsts
    for (int k=0; k<4; k++) {
        if (!drivers_present[k]) {
            return false;
        }
    }

    return true;
}

void drivers_diagnostic()
{
    for (int k=0; k<5; k++) {
        shell_println("* Driver #");
        shell_println(k);
        if (!drivers_present[k]) {
            shell_println(" MISSING");
        } else {
            uint8_t status = drivers_status(k);
            if (status == 0x55) {
                shell_println(" OK");
            } else {
                shell_println(" ERR: ");
                shell_println(driver_error(status&0xf));
            }
        }
    }
}

SHELL_COMMAND(err, "Error")
{
    if (drivers_is_error) {
        shell_println("Drivers are in error mode");
    } else {
        shell_println("Drivers are OK");
    }
}

SHELL_COMMAND(ddb, "")
{
    struct driver_packet_ans ans = drivers_set(2, true, 0, 1);

    shell_println(ans.speed);
    shell_println(" ");
    shell_println(ans.pwm);

    uint8_t *ptr = (uint8_t*)&ans;
    for (size_t k=0; k<sizeof(ans); k++) {
        shell_println((int)ptr[k]);
    }
}

SHELL_COMMAND(blink, "Blink the drivers")
{
    for (int k=0; k<5; k++) {
        shell_println("Blinking ");
        shell_println(k);

        for (int n=0; n<50; n++) {
            wait_us(1000);
            drivers_set(k, true, 0);
            // watchdog_feed();
        }
    }
}

SHELL_COMMAND(pid, "PID")
{
    if (argc != 3) {
        shell_println("Usage: pid [p] [i] [d]");
    } else {
        drivers_set_params(atof(argv[0]), atof(argv[1]), atof(argv[2]));
    }
}

SHELL_COMMAND(scan, "Scan for drivers")
{
    for (int k=0; k<5; k++) {
        shell_println("Driver #");
        shell_println(k);
        shell_println(" ");
        shell_println(drivers_pins[k]);
        shell_println(" ");
        if (drivers_ping(k)) {
            shell_println("Present!");
        } else {
            shell_println("-");
        }
    }
}

SHELL_COMMAND(set, "Set speed for one driver")
{
    if (argc != 2) {
        shell_println("Usage: set [driver] [speed]");
    } else {
        while (!serial.available()) {
            drivers_set_safe(atoi(argv[0]), true, atof(argv[1]));
            drivers_tick();
            buzzer_tick();

// Remove of steve Debug
// //GROS DEBUG
//            digitalWrite(IR_EMIT, HIGH);
//            int value = analogRead(IR_RECEIVE);
//            digitalWrite(IR_EMIT, LOW);
//            terminal_io()->println(value);
//            delay(5);
//            /////////


            // watchdog_feed();
            wait_us(500);
        }
    }
}