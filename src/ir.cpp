#include <stdlib.h>
#include <wirish/wirish.h>
#include <terminal.h>
#include "ir.h"
#include "hardware.h"
#include <watchdog.h>
#include "drivers.h"

volatile bool ir_detected = false;
volatile int ir_value = 0;
volatile int ir_tampon = 0;

extern bool developer_mode;

void ir_init()
{
    pinMode(IR_EMIT, OUTPUT);
    digitalWrite(IR_EMIT, LOW);
    pinMode(IR_RECEIVE, INPUT_ANALOG);

    ir_value = 0;
}


bool ir_present()
{
  return (ir_tampon > 10);
}

bool ir_present_now()
{
  return ir_detected;
}


void ir_tick()
{
    static int lastSample = 0;


    if (millis() - lastSample > 1) {
        lastSample = millis();
        digitalWrite(IR_EMIT, HIGH);
        delay_us(30); // TO IMPROVE (do we reduce the time ?)
        ir_value = analogRead(IR_RECEIVE);
        digitalWrite(IR_EMIT, LOW);
        

        if (ir_value > IR_THRESHOLD) {
          ir_detected = false;
          ir_tampon = 0;
        } else {
          ir_detected = true;
          ir_tampon += 1;
        }
    }
}

void ir_diagnostic()
{
  if (ir_present()) {
        terminal_io()->println("* IR: ERROR OR SOMETHING PRESENT");
  } else {
        terminal_io()->println("* IR: OK");
    }
}

TERMINAL_COMMAND(ir, "Test IR")
{
    while (!SerialUSB.available()) {

      static int lastSample = 0;

      if (millis() - lastSample > 1) {
          lastSample = millis();
          digitalWrite(IR_EMIT, HIGH);
          delay_us(30); // TO IMPROVE (do we reduce the time ?)
          ir_value = analogRead(IR_RECEIVE);
          digitalWrite(IR_EMIT, LOW);

          if (ir_value > IR_THRESHOLD) {
              ir_detected = false;
              ir_tampon = 0;
          } else {
              ir_detected = true;
              ir_tampon += 1;
          }
        terminal_io()->println(ir_value);
        terminal_io()->println(ir_present());
      }
        //same as ir tick
      watchdog_feed();
    }

}

TERMINAL_COMMAND(irp, "Ir present?")
{
  terminal_io()->println(ir_present() ? 1 : 0);
  terminal_io()->println(ir_present_now() ? 1 : 0);
}

TERMINAL_COMMAND(val, "Ir value")
{
  terminal_io()->println(ir_value);
  terminal_io()->println(ir_present() ? 1 : 0);
  terminal_io()->println(ir_present_now() ? 1 : 0);
}
