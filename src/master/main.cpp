#include <mbed.h>
#include "rtt.h"
// #include "swo.h"

naelic::RTT rtt;

FileHandle *mbed::mbed_override_console(int)
{
  return &rtt;
}
DigitalOut led(LED1);

int main(void)
{

  int i = 0;
  do
  {
    i++;
    // uint64_t later = Kernel::get_ms_count();
    // uint32_t dhcsr = CoreDebug->DHCSR;
    // bool debuggerAttached = dhcsr & CoreDebug_DHCSR_C_DEBUGEN_Msk;
    // naelic::SWO swo;
    printf("%d\n", i);
    // if (debuggerAttached)
    // {
    led = !led;
    // }
    // DeepSleepLock lock;
    // int j = 0;
    // while (SEGGER_RTT_HasDataUp(0) != 0)
    // {
    //   j++;
    //   ;
    // }
    // printf("Hello World from SEGGER!%d \n", i);
    // uint64_t elapsed_ms = Kernel::get_ms_count() - later;
    // printf("Elapsed ms: %u \n", (uint32_t)elapsed_ms);
    // printf("j :%d \n", j);
    // _Delay(1);
    ThisThread::sleep_for(1s);
  } while (true);
  return 0;
}

// static void _Delay(int period)
// {
//   int i = 1000 * period;
//   do
//   {
//     printf("%d \n", i);
//   } while (i--);
// }

// int main(void)
// {
//   SEGGER_RTT_Init();
//   SEGGER_RTT_ConfigUpBuffer(0, NULL, NULL, 0, SEGGER_RTT_MODE_BLOCK_IF_FIFO_FULL);
//   SEGGER_RTT_printf(0, "test\n");
//   int i = 0;
//   do
//   {
//     i++;
//     SEGGER_RTT_printf(0, "Hello World from SEGGER!%d \n", i);
//     // uint64_t later = Kernel::get_ms_count();
//     // DeepSleepLock lock;
//     int j = 0;
//     while (SEGGER_RTT_HasDataUp(0) != 0)
//     {
//       j++;
//       ;
//     }
//     // uint64_t elapsed_ms = Kernel::get_ms_count() - later;
//     // printf("Elapsed ms: %u \n", (uint32_t)elapsed_ms);
//     printf("j :%d \n", j);
//     // _Delay(1);
//     ThisThread::sleep_for(1s);
//   } while (true);
//   return 0;
// }