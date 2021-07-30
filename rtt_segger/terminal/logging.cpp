#include "logging.h"

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

    void RTT::unlock()
    {
        wait_us(1500);
    }

} // namespace naelic