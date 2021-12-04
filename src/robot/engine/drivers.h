#pragma once

#include <common/buzzer/buzzer.h>
#include <common/define/hardware.h>
#include <USBSerial.h>
#include <mbed.h>
#include <shell.h>
#include "errors.h"

#include <swo.h>

#define DRIVER_PACKET_SET 0x00
#define NUM_OF_TRINAMIC     4

// For debug, max speed is 1m/s
#define DEBUG_MAX_TRINAMIC_SPEED    8928

namespace tritri {
    enum MotorType : uint8_t {
      NO_MOTOR           = 0,
      SINGLE_PHASE_MOTOR = 1,
      TWO_PHASE_STEPPER  = 2,
      THREE_PHASE_BLDC   = 3
    };

    enum ModeMotion : uint8_t {
      STOP            = 0,
      TORQUE          = 1,
      VELOCITY        = 2, 
      POSITION        = 3,
      PRBS_FLUX       = 4,
      PRBS_TORQUE     = 5,
      PRBS_VELOCITY   = 6,
      PRBS_POSITION   = 7,
      UQ_UD_EXT       = 8,
      // 9 reserved 
      AGPI_A_TORQUE   = 10,
      AGPI_A_VELOCITY = 11,
      AGPI_A_POSITION = 12,
      PWM_I_TORQUE    = 13,
      PWM_I_VELOCITY  = 14,
      PWM_I_POSITION  = 15,
    };
    enum ModeFF : uint8_t {
      DISABLED         = 0,
      VELOCITY_CONTORL = 1,
      TORQUE_CONTORL   = 2
    };

    enum ModePIDType : bool {
      PARALLEL_PI = 0,
      SEQUENTIAL_PI = 1,
    };

    
    // struct MODE_RAMP_MODE_MOTION {
      
    // };

    // struct InitialConfig {
    //   constexpr uint16_t N_POLE_PAIRS = 8; 
    //   constexpr MotorType MOTOR_TYPE  = THREE_PHASE_BLDC; 
    //   uint8_t controller_downsampling_factor : 7 = 0x0; // MODE_PID_SMPL 
    //   uint8_t mode : 1 = 0x0; // MODE_PID_SMPL 

    // };
}
namespace drivers
{
    void tmc4671_init(uint8_t motor);
    void start_motor(uint8_t motor);
    void stop_motor(uint8_t motor, bool freewheel = false);
    void set_speed(int id_motor, float target);

    void init();

    bool ping(int motor);

    void setVelocityGain(uint16_t P, uint16_t I);
} 
