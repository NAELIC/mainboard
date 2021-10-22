#include <math.h>
#include <mbed.h>

#define DEG2RAD(deg) (deg * M_PI / 180.0)
#define WHEEL_RADIUS (0.06 / 2.0)
#define ROBOT_RADIUS (0.17 / 2.0)
#define ANGLE_REAR DEG2RAD(45)
#define ANGLE_FRONT DEG2RAD(120)

#define FRONT_LEFT_X -sin(ANGLE_FRONT)
#define FRONT_LEFT_Y -cos(ANGLE_FRONT)
#define FRONT_RIGHT_X -sin(-ANGLE_FRONT)
#define FRONT_RIGHT_Y -cos(-ANGLE_FRONT)
#define REAR_LEFT_X -sin(ANGLE_REAR)
#define REAR_LEFT_Y -cos(ANGLE_REAR)
#define REAR_RIGHT_X -sin(-ANGLE_REAR)
#define REAR_RIGHT_Y -cos(-ANGLE_REAR)

namespace kinematic
{

    extern float front_left, front_right, rear_left, rear_right;
    extern float x, y, t;

    void launch();
}