#include "serial.h"

#include <swo.h>

namespace serial
{
    static naelic::SWO swo;

    USBSerial serial;

    void launch()
    {
        serial.init();

        while (true)
        {
            if(serial.available() >= sizeof(packet_robot)) {
                packet_robot receive;
                serial.read(&receive, sizeof(packet_robot));

                swo.println("TODO");
                swo.println(receive.id);
                swo.println(receive.x_speed);
            }
        }
    }

}
