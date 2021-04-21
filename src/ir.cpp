#include "ir.h"
#include "mbed.h"
#include "shell.h"
#include "hardware.h"

volatile bool ir_detected = false;
volatile int ir_value = 0;
volatile int ir_tampon = 0;


DigitalOut ir_emit(IR_EMIT);
AnalogIn ir_receive(IR_RECEIVE);

void ir_init() // initialisation for the start of the main
{
    ir_emit = 0;
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
    static int lastSample = Kernel::Clock::now().time_since_epoch().count();
    if ( Kernel::Clock::now().time_since_epoch().count() - lastSample > 1) {
        lastSample = Kernel::Clock::now().time_since_epoch().count();
        ir_emit = 1;
        wait_us(30); // TO IMPROVE (do we reduce the time ?)
        ir_value = ir_receive.read_u16();        
        ir_emit = 0;
        

        if (ir_value > IR_THRESHOLD) {
          ir_detected = false;
          ir_tampon = 0;
        } else {
          ir_detected = true;
          ir_tampon += 1;
        }
    }
    //same as ir tick
    Watchdog::get_instance().kick();
}

void ir_diagnostic()
{
  if (ir_present()) {
      shell_println("* IR: ERROR OR SOMETHING PRESENT");
  } else {
      shell_println("* IR: OK");
  }
}

SHELL_COMMAND(ir, "Test IR")
{
    while (!shell_available()) {
      shell_println(ir_value);
      shell_println(ir_present());      
    }
}

SHELL_COMMAND(irp, "Ir present?")
{
  shell_println(ir_present() ? 1 : 0);
  shell_println(ir_present_now() ? 1 : 0);
}

SHELL_COMMAND(val, "Ir value")
{
  shell_println(ir_value);
  shell_println(ir_present() ? 1 : 0);
  shell_println(ir_present_now() ? 1 : 0);
}
