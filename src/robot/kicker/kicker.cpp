#include "kicker.h"

static naelic::SWO swo;
#include "../engine/kinematic.h"

/*namespace kicker
{
    PwmOut kicker_1(KICKER1_PIN);
    PwmOut kicker_2(KICKER2_PIN);
    void set_kicker_value(int id,float power){
        switch (id)
        {
      40  * // Count the time to tog     case 1:
            kicker_1.write(power);
            swo.printf("kicker_1 -> power : %f\n", power);
            break;
        case 2:
            kicker_2.write(power);
            swo.printf("kicker_1 -> power : %f\n", power);
            break;
        default:
            swo.printf("error : enter in default case, id = %d\n??? -> power : %f\n",id ,power);
            break;
        }
    }
    void launch(){
        kicker_1.period_ms(1);
        kicker_2.period_ms(1);
        while (true){
            ThisThread::sleep_for(100ms);
        }
    }
    SHELL_COMMAND(kicker_set, "Set kicker")
    {
        if (argc != 2)
        {
        shell_println("Usage: set [kicker] [power]");
        }
        else
        {
        while (!shell_available())
        {
            kicker::set_kicker_value(atoi(argv[0]), atof(argv[1]));
            wait_us(500);
        }
        }
    }
}*/
/* -------------------------------------------------------------------*/

static bool charging = false;
static float cap = 0.0;
Timeout kicker_timeout;

DigitalOut booster(BOOSTER_PIN);
DigitalOut kicker_1(KICKER1_PIN);
DigitalOut kicker_2(KICKER2_PIN);

bool is_charging()
{
  float cap = kicker_cap_voltage();
  return kicker_is_charging() and (cap < KICKER_CHARGING_VALUE);
}

void enable_boost()
{
  swo.println("BOOOSTR");
  charging = true;
  booster.write(0.0);
}

void disable_boost()
{
  swo.println("BOOOST NOT");
  charging = false;
  booster.write(1.0);
}

void kicker_init()
{
  // booster.period_ms(100);
  disable_boost();

  kicker_off();
}

void kicker_clear()
{
  swo.println("CLEAR");

  for (int i = 0; i < 500; i++)
  {
    kicker_kick(0, 150);
    wait_us(10000);
  }
}

void kicker_boost_enable(bool enable)
{
  enable ? enable_boost() : disable_boost();
}

void kicker_off()
{
  kicker_1.write(0);
  kicker_2.write(0);
}

void kicker_kick(int kicker, int power)
{
  // if (!charging && !force)
  // {
  //   return;
  // }

  if (power < 0)
  {
    return;
  }
  if (power > 65000)
  {
    power = 65000;
  }

  (kicker == 0) ? kicker_1.write(1) : kicker_2.write(1);

  kicker_timeout.attach(kicker_off, std::chrono::microseconds(power));
}

// Rename to kicker::voltage
void kicker_tick()
{
  float voltage = 3.3 * mux_sample(CAP_ADDR) / (1 << 16); 
  cap = voltage * (CAP_R1 + CAP_R2) / CAP_R2;
}

float kicker_cap_voltage()
{
  return cap;
}

SHELL_COMMAND(cap, "cap")
{
  shell_println(kicker_cap_voltage());
}

SHELL_COMMAND(boost, "Enable/disable the booster")
{
  if (argc)
  {
    kicker_boost_enable(atoi(argv[0]) != 0);
  }
  else
  {
    shell_println("Usage: boost [0|1]");
  }
}

SHELL_COMMAND(kick, "Kicks")
{
  if (argc == 2)
  {
    kicker_kick(atoi(argv[0]), atoi(argv[1]));
  }
  else
  {
    shell_println("Usage: kick [kicker] [power]");
  }
}


SHELL_COMMAND(kick_info, "Kicks")
{
  shell_print("kicker 1: ");
  shell_println(kicker_1.read());
  shell_print("kicker 2: ");
  shell_println(kicker_2.read());
  shell_print("boost flag: ");
  shell_println(booster.read());
}

SHELL_COMMAND(em, "Emergency kicker")
{

  kinematic::stopRobotAndDisable();

  disable_boost();
  kicker_clear();
}