#include "serial.h"

#include <swo.h>

namespace serial
{
    static naelic::SWO swo;

    USBSerial serial;

    void launch()
    {
        serial.init();
        wireless::init();

        while (true)
        {
            if(serial.available() >= sizeof(packet_master)) {
                packet_master receive;
                serial.read(&receive, sizeof(packet_master));

                wireless::send(receive);
            }

            ThisThread::sleep_for(100ms);
        }
    }

}
