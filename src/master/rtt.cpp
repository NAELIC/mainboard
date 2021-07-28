#include "rtt.h"
#include "SEGGER_RTT.h"

#include "swo.h"

namespace naelic
{

    int RTT::_putc(int c)
    {
        SEGGER_RTT_PutChar(0, c);

        return c;
    }

    int RTT::_getc()
    {
        return -1;
    }

    // void RTT::lock()
    // {
    //     _mutex.lock();
    // }
    void RTT::unlock()
    {
        // uint32_t dhcsr = CoreDebug->DHCSR;
        // bool debuggerAttached = dhcsr & CoreDebug_DHCSR_C_DEBUGEN_Msk;
        // naelic::SWO swo;
        // printf("%d", debuggerAttached);
        //         if (!debuggerAttached || !SEGGER_RTT_HasDataUp(0)) {
        //   am_hal_sysctrl_sleep(AM_HAL_SYSCTRL_SLEEP_DEEP);
        // }
        // while (SEGGER_RTT_HasDataUp(0) != 0 && BLOCKING_MODE)
        // {
        //     ;
        // }
        wait_us(1500);
    }

} // namespace naelic
