#include <mbed.h>
#include <USBSerial.h>
#include <swo.h>
#include <shell.h>

#include <common/buzzer/buzzer.h>
#include <common/utils/version.h>

#include "com.h"

#include <terminal/logging.h>
#include <RTT/SEGGER_RTT.h>

// uint32_t TIMEOUT_WATCHDOG_MS = 2000;
DigitalOut led(LED1);
static naelic::SWO swo;

naelic::RTT rtt;

FileHandle *mbed::mbed_override_console(int)
{
  return &rtt;
}

// ssize_t write(const void *buffer, size_t size)
// {
//   // write the buffer content to the terminal
//   return SEGGER_RTT_Write(1, buffer, size);
// }

// int main()
// {
//   // Thread buzzer_th;
//   // buzzer_th.start(buzzer::launch);

//   // Thread com_th;
//   // com_th.start(com::launch);

//   shell_init_usb();
//   // Watchdog &watchdog = Watchdog::get_instance();
//   // watchdog.start(TIMEOUT_WATCHDOG_MS);

//   led = 1;
//   while (true)
//   {
//     ThisThread::sleep_for(100ms);
//     // test_radio();
//   }
// }

ssize_t read_test(void *buffer, size_t size)
{
  // wait until a character becomes available
  while (!SEGGER_RTT_HasKey())
  {
  }

  // read the characters from the terminal into the buffer
  return SEGGER_RTT_Read(0, buffer, size);
}

// short poll(short events)
// {
//   short revents = 0;
//   if ((events & POLLIN) && SEGGER_RTT_HasKey())
//   {
//     revents |= POLLIN;
//   }
//   if (events & POLLOUT)
//   {
//     revents |= POLLOUT;
//   }
//   return revents;
// }

int main(void)
{
  int i = 0;
  do
  {
    i++;
    // printf("%d\n", i);
    led = !led;
    char c[4];
    bool b = SEGGER_RTT_HasKey();
    // printf("%d \n", b);
    // printf("%d \n",i);
    if (b)
    {
      printf("%c", SEGGER_RTT_GetKey());
    }
    // read_test(c, 4);
    // wait_us(1000000);
    // ThisThread::sleep_for(500ms);
  } while (true);
  return 0;
}