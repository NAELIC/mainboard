#include <mbed.h>
#include <swo.h>
#include "com.h"
#include "motor.h"

static naelic::SWO swo;
// DigitalOut led(LED1);

// Override mbed printf implementation
FileHandle *mbed::mbed_override_console(int)
{
  return &swo;
}

int main()
{
  com::init();
  // motor::init();

  // motor::motor_hall_set(true, 500);

  while (true)
  {
    // motor::tick();
    com::tick();
    ThisThread::sleep_for(100ms);
  }
}