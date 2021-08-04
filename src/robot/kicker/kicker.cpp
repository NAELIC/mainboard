#include "kicker.h"

static naelic::SWO swo;

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

// static bool kicking = false;
// static int kick_end = 0;
static bool clearing = false;
static int clear = 0;
static bool charging = false;
bool developer_mode = false;
Timeout kicker_timeout;

#define KICKER_OFF 0
#define KICKER_ON 1
#define BOOST_ON 0
#define BOOST_OFF 1

PwmOut booster(BOOSTER_PIN);
PwmOut kicker_1(KICKER1_PIN);
PwmOut kicker_2(KICKER2_PIN);

bool is_charging()
{
  float cap = kicker_cap_voltage();
  return kicker_is_charging() and (cap < KICKER_CHARGING_VALUE);
}
void enable_boost()
{
  swo.println("BOOOSTR");
  booster.write(BOOST_ON);
}

void disable_boost()
{
  swo.println("BOOOST NOT");
  booster.write(BOOST_OFF);
}

void init_boost()
{
  disable_boost();
}
static bool kicker_pause = true;

void pause_boost()
{
  kicker_pause = true;
  if (charging)
  {
    disable_boost();
  }
}

void resume_boost()
{
  kicker_pause = false;
  if (charging)
  {
    enable_boost();
  }
}

float cap = 0.0;

static bool relaunch_charging = true;

static void _kicker_irq()
{
  kicker_1.write(KICKER_OFF);
  kicker_2.write(KICKER_OFF);

  relaunch_charging = true;
}

void kicker_init()
{
  // Configuring booster pin
  init_boost();

  // Kicker pin
  kicker_1.write(KICKER_OFF);
  kicker_2.write(KICKER_OFF);
  kicker_1.period_ms(100);
  kicker_2.period_ms(100);

  // if (developer_mode == false)
  // {
  //   kicker_boost_enable(true);
  // }
}

void kicker_clear()
{
  clearing = true;
  clear = 0;
}

void kicker_boost_enable(bool enable)
{
  swo.println(enable);
  resume_boost();
  charging = enable;

  if (enable)
  {
    clearing = false;
  }
  relaunch_charging = true;
}

void kicker_off()
{
  kicker_1.write(0);
  kicker_2.write(0);
}

void kicker_kick(int kicker, int power)
{
  if (power < 0)
  {
    return;
  }
  if (power > 65000)
  {
    power = 65000;
  }

  swo.println("test");

  (kicker == 0) ? kicker_1.write(KICKER_ON) : kicker_2.write(KICKER_ON);

  kicker_timeout.attach(kicker_off, std::chrono::microseconds(power));

  if (charging)
  {
    disable_boost();
  }
}

#define DICHARGE_VOLTAGE 24
void kicker_tick()
{
  static int lastClear = Kernel::get_ms_count();
  static int lastSample = Kernel::get_ms_count();

  // Sampling capacitor voltage
  if (Kernel::get_ms_count() - lastSample > 5)
  {
    float voltage = 3.3 * mux_sample(CAP_ADDR) / (1 << 16);
    cap = voltage * (CAP_R1 + CAP_R2) / CAP_R2;

    lastSample = Kernel::get_ms_count();
    // cap = voltage * 0.99 + cap * 0.01;

    if (cap > DICHARGE_VOLTAGE && !charging)
    {
      kicker_clear();
    }
  }

  // Triggering kicks for clearing
  if (clearing)
  {
    if ((Kernel::get_ms_count() - lastClear) >= 10)
    {
      lastClear = Kernel::get_ms_count();
      kicker_kick(0, 150);
      // kicker_kick(1, 150);
      clear++;

      if (clear > 500)
      {
        clearing = false;
      }
    }
  }

  if (relaunch_charging)
  {
    
    relaunch_charging = false;
    if (charging)
    {
      if (kicker_pause)
      {
        // Should not happend !
        disable_boost();
      }
      else
      {
        enable_boost();
      }
    }
    else
    {
      disable_boost();
    }
  }

  //If we are charging, we allow for some more time without the com interlacing (faster charge)
  if (is_charging())
  {
    wait_us(1000);
  }
}

bool kicker_is_charging()
{
  return charging;
}

float kicker_cap_voltage()
{
  return cap;
}

SHELL_COMMAND(cap, "cap")
{
  shell_println(kicker_cap_voltage());
}

SHELL_COMMAND(cd, "Cap debug")
{
  shell_println(mux_sample(CAP_ADDR));
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

SHELL_COMMAND(em, "Emergency kicker")
{
  // if (!com_master) {
  //     kinematic_stop();
  //     for (int k=0; k<5; k++) {
  //         drivers_set(k, false, 0.0);
  //     }
  // }

  kicker_boost_enable(false);
}