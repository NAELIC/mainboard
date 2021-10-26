#include "serial.h"

#include <swo.h>

namespace serial
{
    static naelic::SWO swo;

    USBSerial serial(false);

    void launch()
    {
        serial.init();
        wireless::init();

        while (true)
        {
            if(serial.available() >= sizeof(packet_robot)) {
                packet_robot receive;
                serial.read(&receive, sizeof(packet_robot));

                wireless::send(receive);
            }

            ThisThread::sleep_for(100ms);
        }
    }

}
