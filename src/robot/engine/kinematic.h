#include <math.h>
#include <mbed.h>

namespace kinematic
{
    void apply_order(float x, float y, float t);

    bool isManualControl();

    void stopRobotAndDisable();
    void enable();

}