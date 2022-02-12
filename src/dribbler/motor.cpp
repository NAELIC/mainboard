#include "motor.h"

#define PWM_MIN_PERCENT 0
#define PWM_MAX_PERCENT 90

// Target PWM speed [0-3000]
static int motor_pwm = 0;
static bool motor_on = false;

#define PWM_HALL_SUPREMUM 3000
#define PWM_SUPREMUM PWM_HALL_SUPREMUM
#define PWM_MIN ((PWM_MIN_PERCENT * PWM_SUPREMUM) / 100)
#define PWM_MAX ((PWM_MAX_PERCENT * PWM_SUPREMUM) / 100)

// Hall current phase
static int hall_current_phase = -2;
static int hall_last_change = 0;
static int hall_last_change_moving = 0;
static int encoder_last_ok = 0;
static bool safe_mode = true;

#include <swo.h>

static naelic::SWO swo;
// Consecutive phases
static int motor_phases[6][3] = {
    //    U   V   W
    {0, 1, -1},
    {1, 0, -1},
    {1, -1, 0},
    {0, -1, 1},
    {-1, 0, 1},
    {-1, 1, 0},
};

// Estimating current phase (see above) depending on hall
// sensor value
static int hall_phases[8] = {
    -1, // 0b000 (impossible)
    0,  // 0b001
    2,  // 0b010
    1,  // 0b011
    4,  // 0b100
    5,  // 0b101
    3,  // 0b110
    -1, // 0b111 (impossible)
};

namespace motor
{
    InterruptIn hallu(HALLU, PullUp);
    InterruptIn hallv(HALLV, PullUp);
    InterruptIn hallw(HALLW, PullUp);

    DigitalOut u_sd(U_SD, 0);
    DigitalOut v_sd(V_SD, 0);
    DigitalOut w_sd(W_SD, 0);

    PwmOut u_pwm(U_IN);
    PwmOut v_pwm(V_IN);
    PwmOut w_pwm(W_IN);

    void init()
    {
        // HALL INIT
        hallu.fall(hall_irq);
        hallu.rise(hall_irq);
        hallv.fall(hall_irq);
        hallv.rise(hall_irq);
        hallw.fall(hall_irq);
        hallw.rise(hall_irq);

        u_pwm.write(0);
        v_pwm.write(0);
        w_pwm.write(0);

        u_pwm.period_us(50);

        v_pwm.period_us(50);

        w_pwm.period_us(50);
    }

    int hall_value()
    {
        uint8_t hall = 0;

        hall = ((hallu.read() & 1) << 2);
        hall |= ((hallv.read() & 1) << 1);
        hall |= ((hallw.read() & 1) << 0);

        return hall;
    }

    void motor_hall_set(bool enable, int value)
    {
        motor_on = enable;

        if (value > 0)
            value += PWM_MIN;
        if (value < 0)
            value -= PWM_MIN;

        if (value < -PWM_MAX)
            value = -PWM_MAX;
        if (value > PWM_MAX)
            value = PWM_MAX;

        motor_pwm = value;
    }

    static void set_phases(int u, int v, int w, int phase)
    {
        bool stopped = false;
        if (!motor_on || (u != 0 && v != 0 && w != 0))
        {
            u = v = w = 0;
        }
        if (u == 0 && v == 0 && w == 0)
        {
            stopped = true;
        }

        static int last_phase = -8;
        static bool last_stopped = true;
        bool update = false;

        // Checking if we should update the mos phase, and generate
        // a deadtime
        update = (last_phase != phase) || (stopped != last_stopped);
        last_phase = phase;
        last_stopped = stopped;
        if (!motor_on)
        {
            last_phase = -2;
        }

        if (update)
        {
            // Setting every output to low
            u_sd.write(0);
            v_sd.write(0);
            w_sd.write(0);

            u_pwm.write(0);
            v_pwm.write(0);
            w_pwm.write(0);
        }

        // swo.print("u");
        // swo.println(u);
        // swo.print("v");
        // swo.println(v);
        // swo.print("w");
        // swo.println(w);
        // u_pwm.write(0.5f);

        if (motor_on)
        {
            
            // if (u >= 0)
            if (u > 0)
            {
                // u_pwm.write(u);
                u_pwm.write(0.0f);
            }

            if (stopped || u != 0)
            {
                if (update)
                    u_sd.write(1);
            }

            // if (v >= 0)
            if (v > 0)
            {
                // v_pwm.write(0.5f);
                v_pwm.write(0.0f);
            }

            if (stopped || v != 0)
            {
                if (update)
                    v_sd.write(1);
            }

            // if (w >= 0)
            if (w > 0)
            {
                w_pwm.write(0.0f);
                // w_pwm.write(w);
            }

            if (stopped || w != 0)
            {
                if (update)
                    w_sd.write(1);
            }
        }
    }

    void hall_irq()
    {
        int phase = hall_phases[hall_value()];

        if (phase < 0 || phase >= 6)
        {
            // XXX: This is not a normal state, not sure what should be done
            // in this situation
            set_phases(0, 0, 0, -1);
        }

        phase += ((motor_pwm > 0) ? 1 : -1);
        if (phase >= 6)
        {
            phase = 0;
        }
        if (phase < 0)
        {
            phase = 5;
        }
        if (phase >= 0 && phase < 6)
        {
            set_phases(
                motor_phases[phase][0] * abs(motor_pwm),
                motor_phases[phase][1] * abs(motor_pwm),
                motor_phases[phase][2] * abs(motor_pwm),
                phase);
        }
        else
        {
            // XXX: This is not a normal state, not sure what should be done
            // in this situation
            set_phases(0, 0, 0, -1);
        }
    }

    void tick()
    {
        hall_irq();
    }
}

// TERMINAL_COMMAND(hall, "Test the hall sensors")
// {
//     terminal_io()->print(digitalRead(HALLU_PIN));
//     terminal_io()->print(" ");
//     terminal_io()->print(digitalRead(HALLV_PIN));
//     terminal_io()->print(" ");
//     terminal_io()->print(digitalRead(HALLW_PIN));
//     terminal_io()->println();
// }

// TERMINAL_COMMAND(bdw, "Bdw")
// {
//     int start = micros();
//     for (int k=0; k<10000; k++) {
//         digitalWrite(U_SD_PIN, LOW);
//     }
//     terminal_io()->println(micros()-start);
// }

// TERMINAL_COMMAND(safe, "Safe mode")
// {
//     if (argc)
//     {
//         safe_mode = (atoi(argv[0]) != 0);
//     }
//     else
//     {
//         terminal_io()->println("Usage: safe [0|1]");
//     }
// }

// TERMINAL_COMMAND(pwm, "Motor set PWM")
// {
//     if (argc > 0)
//     {
//         motor_set(true, atoi(argv[0]));
//     }
//     else
//     {
//         terminal_io()->print("usage: pwm [0-3000] (current: ");
//         terminal_io()->print(abs(motor_pwm));
//         terminal_io()->print(")");
//         terminal_io()->println();
//     }
// }

// TERMINAL_COMMAND(enable_hall, "Enable hall")
// {
//     if (argc == 1)
//     {
//         int val = atoi(argv[0]);
//         if (val == 0)
//         {
//             enable_motor_hall(false);
//         }
//         else
//         {
//             enable_motor_hall(true);
//         }
//     }
// }
