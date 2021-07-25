#include <mbed.h>
#include <USBSerial.h>
#include <swo.h>
#include <shell.h>

#include <common/buzzer.h>
#include <common/version.h>

// #include "com.h"

#include "RTT/SEGGER_RTT.h"

// uint32_t TIMEOUT_WATCHDOG_MS = 2000;
DigitalOut led(LED1);
static naelic::SWO swo;

int main()
{
  SEGGER_RTT_SetTerminal(0);
  SEGGER_RTT_ConfigUpBuffer(0, NULL, NULL, 0, SEGGER_RTT_MODE_BLOCK_IF_FIFO_FULL);

  SEGGER_RTT_WriteString(0, "###### Testing SEGGER_printf() on Terminal 0 ######\r\n");

  SEGGER_RTT_printf(0, "printf Test: %%c,         'S' : %c.\r\n", 'V');
  
  SEGGER_RTT_SetTerminal(13);
  
  SEGGER_RTT_printf(0, "%sTime:%s%s %.7d\n",
                      RTT_CTRL_RESET,
                      RTT_CTRL_BG_BRIGHT_RED,
                      RTT_CTRL_TEXT_BRIGHT_WHITE,
                      1111111
                      );
  // SEGGER_RTT_ConfigUpBuffer(1, NULL, NULL, 0, SEGGER_RTT_MODE_BLOCK_IF_FIFO_FULL);
  // SEGGER_RTT_WriteString(1, "###### Testing SEGGER_printf() on Terminal 1 ######\r\n");

  SEGGER_RTT_printf(0, "printf Test: %%c,         'T' : %c.\r\n", 'T');

  Thread buzzer_th;
  buzzer_th.start(buzzer::launch);
  shell_init_usb();
  // Watchdog &watchdog = Watchdog::get_instance();
  // watchdog.start(TIMEOUT_WATCHDOG_MS);
  // com_init();

  led = 1;
  while (true)
  {
    ThisThread::sleep_for(100ms);
    // test_radio();
  }
}
