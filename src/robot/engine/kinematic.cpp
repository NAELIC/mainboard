#include "kinematic.h"

#define MAX_ACCELERATION (0.001)

namespace kinematic
{
    time_t enabled_since;
    bool enabled = false;

    void set_target(float target_x, float target_y, float target_t)
    {
        enabled = true;
        x = target_x;
        y = target_y;
        t = target_t;
    }

    void init()
    {
        enabled_since = time(NULL);
    }

    void launch()
    {
        if (enabled)
        {
            return;
        }

        if (time(NULL) - enabled_since > 100)
        {
            enabled = false;
            x = 0;
            y = 0;
            t = 0;
            return;
        }

        float new_front_left = (FRONT_LEFT_X * x + FRONT_LEFT_Y * y +
                                ROBOT_RADIUS * t) /
                               (2 * M_PI * WHEEL_RADIUS);

        float new_front_right = (FRONT_RIGHT_X * x + FRONT_RIGHT_Y * y +
                                 ROBOT_RADIUS * t) /
                                (2 * M_PI * WHEEL_RADIUS);

        float new_rear_left = (REAR_LEFT_X * x + REAR_LEFT_Y * y +
                               ROBOT_RADIUS * t) /
                              (2 * M_PI * WHEEL_RADIUS);

        float new_rear_right = (REAR_RIGHT_X * x + REAR_RIGHT_Y * y +
                                ROBOT_RADIUS * t) /
                               (2 * M_PI * WHEEL_RADIUS);

        float delta1 = fabs(new_front_left - front_left);
        float delta2 = fabs(new_front_right - front_right);
        float delta3 = fabs(new_rear_left - rear_left);
        float delta4 = fabs(new_rear_right - rear_right);

        if (delta2 > delta1)
            delta1 = delta2;
        if (delta3 > delta1)
            delta1 = delta3;
        if (delta4 > delta1)
            delta1 = delta4;

        if (delta1 > MAX_ACCELERATION)
        {
            float alpha = MAX_ACCELERATION / delta1;
            front_left = front_left + alpha * (new_front_left - front_left);
            front_right = front_right + alpha * (new_front_right - front_right);
            rear_left = rear_left + alpha * (new_rear_left - rear_left);
            rear_right = rear_right + alpha * (new_rear_right - rear_right);
        }
        else
        {
            front_left = new_front_left;
            front_right = new_front_right;
            rear_left = new_rear_left;
            rear_right = new_rear_right;
        }
    }
}