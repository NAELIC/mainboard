#include "com.h"
namespace com
{
    static naelic::SWO swo;
    static SPISlave device(COM_MOSI, COM_MISO, COM_CLK, COM_SEL);

    void init()
    {
        device.frequency(500000);
        device.reply(0x01);
    }

    void tick()
    {
        if (device.receive())
        {
            swo.println("receive");
            int v = device.read(); // Read byte from master
            // v = (v + 1) % 0x100;   // Add one to it, modulo 256
            swo.println(v);
            device.reply(0x01); // Make this the next reply
        }
    }
}