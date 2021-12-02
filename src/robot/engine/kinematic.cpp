#include "kinematic.h"

#include <common/define/common.h>

#include "drivers.h"
#include "trinamics.h"
#include <math.h>

#define MAX_MOTOR_ACCELERATION (0.01)

#define DEG2RAD(deg) (deg*M_PI/180.0)
#define WHEEL_RADIUS (0.06/2.0)
#define ROBOT_RADIUS  (0.17/2.0)
#define ANGLE_REAR   DEG2RAD(45)
#define ANGLE_FRONT  DEG2RAD(120)

#define FRONT_LEFT_X     -sin(ANGLE_FRONT)
#define FRONT_LEFT_Y     -cos(ANGLE_FRONT)
#define FRONT_RIGHT_X    -sin(-ANGLE_FRONT)
#define FRONT_RIGHT_Y    -cos(-ANGLE_FRONT)
#define REAR_LEFT_X      -sin(ANGLE_REAR)
#define REAR_LEFT_Y      -cos(ANGLE_REAR)
#define REAR_RIGHT_X     -sin(-ANGLE_REAR)
#define REAR_RIGHT_Y     -cos(-ANGLE_REAR)

namespace kinematic
{
    bool manual_control = false;

    bool isManualControl() {
      return core_util_atomic_load_bool(&manual_control);
    }

    void disableManualControl() {
       core_util_atomic_store_bool(&manual_control, false);
    }

    void enableManualControl() {
       core_util_atomic_store_bool(&manual_control, true);
    }

    #define FRONT_LEFT_ID  0
    #define REAR_LEFT_ID   1
    #define REAR_RIGHT_ID  2
    #define FRONT_RIGHT_ID 3
    #define NB_MOTORS 4

    constexpr float ROTATION_DRIVE_COEF = ROBOT_RADIUS/(2*M_PI*WHEEL_RADIUS);

    float motors_drive_x[NB_MOTORS] =  { FRONT_LEFT_X, REAR_LEFT_X, REAR_RIGHT_X, FRONT_RIGHT_X };
    float motors_drive_y[NB_MOTORS] =  { FRONT_LEFT_Y, REAR_LEFT_Y, REAR_RIGHT_Y, FRONT_RIGHT_Y };
    float motors_drive_t[NB_MOTORS] =  { ROTATION_DRIVE_COEF, ROTATION_DRIVE_COEF, ROTATION_DRIVE_COEF, ROTATION_DRIVE_COEF };
    float motors_speed_m_s[NB_MOTORS]  =  { 0.0, 0.0, 0.0, 0.0 };

    // speed limitation 
    float last_motors_speed_m_s[NB_MOTORS]  =  { 0.0, 0.0, 0.0, 0.0 };
    uint64_t last_speed_applied_timestamp_ms = Kernel::get_ms_count();
    
    float limitation_speed_factor[NB_MOTORS]  =  { 1.0, 1.0, 1.0, 1.0 };
    float acc_motors_m_s_s[NB_MOTORS]  =  { 0.0, 0.0, 0.0, 0.0 };
    /**
     * @brief Applies a given order directly while preserving the integrity of the robot
     * 
     * (Xavier): for the moment we applying the command directly, 
     *           the timing is direcly related to the communication speed given by the controlling computer.
     *           This is bad but it's interesting to see if the robot can be controlled in such situation.  
     *
     * @param x in m/s
     * @param y in m/s
     * @param t in rad/s
     */
    void apply_order(float x, float y, float t) {

        common::swo.print("x: ");
        common::swo.println(x);
        common::swo.print("y: ");
        common::swo.println(y);
        common::swo.print("t: ");
        common::swo.println(t);
        common::swo.println("----");
        // Apply kinematic
        auto speed_applyed_timestamp_ms = Kernel::get_ms_count();

        float max_motor_acc = 0.0;
        for(uint8_t i = 0; i < NB_MOTORS; i++ ) {
          motors_speed_m_s[i] = motors_drive_x[i] * x + motors_drive_y[i] * y + motors_drive_t[i] * t;
          // get max acc
          acc_motors_m_s_s[i] = motors_speed_m_s[i] - last_motors_speed_m_s[i];
          if ( fabs(max_motor_acc) < fabs(acc_motors_m_s_s[i]))
            max_motor_acc = acc_motors_m_s_s[i];
        }
        
        if ( max_motor_acc > MAX_MOTOR_ACCELERATION) {
            float acc_limitation_ratio = MAX_MOTOR_ACCELERATION / max_motor_acc;
            for(uint8_t i = 0; i < NB_MOTORS; i++ ) {
                motors_speed_m_s[i] =  last_motors_speed_m_s[i] + acc_limitation_ratio *  acc_motors_m_s_s[i];
                last_motors_speed_m_s[i] = motors_speed_m_s[i];
                trinamics::set_speed(i, motors_speed_m_s[i]);
            }
        } else {
            for(uint8_t i = 0; i < NB_MOTORS; i++ ) {
                last_motors_speed_m_s[i] = motors_speed_m_s[i];
             
                trinamics::set_speed(i, motors_speed_m_s[i]);
            }
        }
        last_speed_applied_timestamp_ms =  speed_applyed_timestamp_ms;
    }

    SHELL_COMMAND(manual, "send motor order: x (m/s), y(m/s), t(rad/s)")
    {   
        enableManualControl();
        float x = atof(argv[0]);
        float y = atof(argv[1]);
        float t = atof(argv[2]);
        shell_print("x: ");
        shell_println(x);

        shell_print("y: ");
        shell_println(y);

        shell_print("t: ");
        shell_println(t);
        shell_println("----");

        kinematic::apply_order(x, y, t);

        for(uint8_t i = 0; i < 4; i++) {
            shell_print("Speed motor ");
            shell_print(i);
            shell_print(": ");
            shell_println(motors_speed_m_s[i]);
        }
    }

    SHELL_COMMAND(stop_manual, "active robot manual control")
    {   
        disableManualControl();
    }
}