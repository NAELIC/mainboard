#include "voltage.h"

static naelic::SWO swo;
static float bat = 0.0;

namespace voltage
{
    void launch()
    {
        while (true)
        {   
            float v1 = (mux_sample(BAT_ADDR) / (float)(1 << 16)) * 3.3;
            bat = v1 * (BAT_R1 + BAT_R2) / BAT_R2;
            ThisThread::sleep_for(2s);
        }
    }

    float value() {
        return bat;
    }
}

SHELL_COMMAND(voltage, "Voltage value")
{
    shell_println(voltage::value());
}