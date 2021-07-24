#include "drivers.h"
#include <common/buzzer.h>
#include <common/hardware.h>
#include <USBSerial.h>
#include <mbed.h>
#include <shell.h>
// #include "kicker.h"
#include "errors.h"

SPI drivers(DRV_MOSI, DRV_MISO, DRV_CLK); // Declaration of SPI

static bool drivers_is_error = false;
static bool drivers_present[5] = {false};
struct driver_packet_ans driver_answers[5];

static DigitalOut drivers_pins[5] = {DRIVERS_CS1, DRIVERS_CS2, DRIVERS_CS3,
                              DRIVERS_CS4, DRIVERS_CS5};

void drivers_init() // initialisation of the drivers
{
  // Initializing SPI
  drivers.frequency(250000); // set the frequency of the drivers

  // Initializing CS pins
  for (int k = 0; k < 5; k++) { // loop for to select all the drivers
    drivers_pins[k] = 1;        // Set the driver_pins in high mode
  }

  for (int k = 0; k < 5; k++) {
    bool ret = false;
    // BOARD_LED_PIN = 1;  // I don't fuckin know what is BOARD_LED_PIN, never
    // define
    ret = drivers_ping(k);
    wait_us(10);
    //   watchdog_feed();      // for now we don't use the watchdog
    // BOARD_LED_PIN = 1;
    drivers_present[k] = ret; // make the driver_pins ping
  }
}

uint8_t drivers_status(int index) {
  drivers_pins[index] = 0; // Set low
  wait_us(35);             // pause needed
  drivers.write(0); // send oly one bit to test the mosi of the spi
  uint8_t answer = drivers.write(0); // write the one bit return of the miso
  wait_us(5);              // pause needed
  drivers_pins[index] = 1; // Set high

  return answer; //if answer != {} : the driver ping
}

int drivers_ping(int index) {   //test to make ping the drivers
  uint8_t status = drivers_status(index); //ping the drivers
  return (status == 0x55 || ((status & 0xf0) == 0x80)); 
}

static void drivers_send(int index, uint8_t instruction, uint8_t *data,
                         size_t len, uint8_t *answer) { //send somthing to the drivers
  drivers_pins[index] = 0; //turn on the pins of the drivers
  wait_us(35); //wait needed and imortant

  drivers.write(instruction);
  drivers.write((char*)data, len, (char*)answer, answer == NULL ? 0 : len);

  wait_us(5);//wait needed and imortant
  drivers_pins[index] = 1; //turn off the pins of the drivers
}

#define REVERSE_TURN
struct driver_packet_ans drivers_set(int index, bool enable, float target,
                                     int16_t pwm) { // set the speed of the drivers
  driver_packet_set packet;

  packet.enable = enable;
#ifdef REVERSE_TURN
  packet.targetSpeed = -target;
#else
  packet.targetSpeed = target;
#endif
  packet.pwm = pwm;
  packet.padding1 = 0;
  packet.padding2 = 0;

  driver_packet_ans ans;

  drivers_send(index, DRIVER_PACKET_SET, (uint8_t *)&packet,
               sizeof(driver_packet_set), (uint8_t *)&ans);

  return ans;
}

void drivers_set_params(float kp, float ki, float kd) { //set the param of the drivers
  for (int index = 0; index < 5; index++) {
    struct driver_packet_params packet;
    packet.kp = kp;
    packet.ki = ki;
    packet.kd = kd;

    drivers_send(index, DRIVER_PACKET_PARAMS, (uint8_t *)&packet,
                 sizeof(struct driver_packet_params), NULL);
  }
}

void drivers_set_safe(int index, bool enable, float target, int16_t pwm) {
  if (!drivers_is_error && drivers_present[index]) {
    struct driver_packet_ans tmp = drivers_set(index, enable, target, pwm);

    if ((tmp.status & 0xf0) == 0x80) {
      driver_answers[index] = tmp;
      for (int k = 0; k < 5; k++) {
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

void drivers_tick() {
  if (drivers_is_error) {
    if (buzzer_is_playing()) {
      for (int k = 0; k < 5; k++) {
        drivers_set(k, false, 0.0);
      }
    } else {
      drivers_is_error = false;
    }
  }
}

bool drivers_is_all_ok() { // check if all is ok
  for (int k = 0; k < 4; k++) {
    if (!drivers_present[k]) {
      return false;
    }
  }

  return true;
}

void drivers_diagnostic() { //detect if the drivers is prsent or is in error
  for (int k = 0; k < 5; k++) {
    shell_print("* Driver #");
    shell_print(k);
    if (!drivers_present[k]) {
      shell_println(" MISSING");
    } else {
      uint8_t status = drivers_status(k);
      if (status == 0x55) {
        shell_println(" OK");
      } else {
        shell_println(" ERR: ");
        shell_println(driver_error(status & 0xf));
      }
    }
  }
}

SHELL_COMMAND(err, "Error") { //detect the error in debug shell
  if (drivers_is_error) {
    shell_println("Drivers are in error mode");
  } else {
    shell_println("Drivers are OK");
  }
}

SHELL_COMMAND(ddb, "") {
  struct driver_packet_ans ans = drivers_set(2, true, 0, 1);

  shell_println(ans.speed);
  shell_println(" ");
  shell_println(ans.pwm);

  uint8_t *ptr = (uint8_t *)&ans;
  for (size_t k = 0; k < sizeof(ans); k++) {
    shell_println((int)ptr[k]);
  }
}

SHELL_COMMAND(blink, "Blink the drivers") {
  for (int k = 0; k < 5; k++) {
    shell_println("Blinking ");
    shell_println(k);

    for (int n = 0; n < 50; n++) {
      wait_us(1000);
      drivers_set(k, true, 0);
      // watchdog_feed();
    }
  }
}

SHELL_COMMAND(pid, "PID") {
  if (argc != 3) {
    shell_println("Usage: pid [p] [i] [d]");
  } else {
    drivers_set_params(atof(argv[0]), atof(argv[1]), atof(argv[2]));
  }
}

SHELL_COMMAND(scan, "Scan for drivers") {
  for (int k = 0; k < 5; k++) {
    shell_print("Driver #");
    shell_print(k);
    shell_print(": ");
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
        while (!shell_available()) {
            drivers_set_safe(atoi(argv[0]), true, atof(argv[1]));
            drivers_tick();
            buzzer_tick();
            wait_us(500);
        }
    }
}