#include "drivers.h"

#include <common/define/common.h>

//#define DRIVER_LOG
#include <stdarg.h> //used for driver_log()
#include <stdio.h> //used for driver_log()

#define MAX_READING_VERIF 3

// Include the TMC4671 lib with corresponding SPI transfert function (see end of drivers.cpp)
extern "C" {
#include <TMC4671.h>
uint8_t tmc4671_readwriteByte(uint8_t motor, uint8_t data, uint8_t lastTransfer);
}

static naelic::SWO swo;

void driver_log(const char *fmt, ...) {
#ifdef DRIVER_LOG
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
#endif
}


namespace drivers {

    // Custom functions to secure SPI communication
    void tmc4671_writeInt_verif(uint8_t motor, uint8_t address, int32_t value);
    int32_t tmc4671_readInt_verif(uint8_t motor, uint8_t address);
    void tmc4671_writeRegister16BitValue_verif(uint8_t motor, uint8_t address, uint8_t channel, uint16_t value);
    uint16_t tmc4671_readRegister16BitValue_verif(uint8_t motor, uint8_t address, uint8_t channel);
    int16_t tmc4671_field_read_verif(uint8_t motor, uint8_t address, uint16_t mask, int16_t shift);
    int16_t tmc4671_getS16CircleDifference(int16_t newValue, int16_t oldValue);

    static SPI drivers(DRV_MOSI, DRV_MISO, DRV_CLK);
    static DigitalOut drivers_out[5] = {DRIVERS_CS1,
                                        DRIVERS_CS2,
                                        DRIVERS_CS3,
                                        DRIVERS_CS4,
                                        DRIVERS_CS5};

    void init() {
        // Setup SPI frequency to 1Mhz (8mHz max for TMC4671-LA, but robot cables must be very good)
        drivers.format(8, 3); // 40 bits datagram (datasheet TMC4671 p.16), but 16 max for TMC4671 module, so 8x5=40
        drivers.frequency(500000);

        // Be sure all SS are high
        for (int k = 0; k < NUM_OF_TRINAMIC + 1; k++) {
            drivers_out[k] = 1;
        }

        //Init all motors (ROBOT MUST BE UPSIDE DOWN AT START)
        for (int k = 0; k < NUM_OF_TRINAMIC; k++) {
            ThisThread::sleep_for(100ms);
            tmc4671_init(k);
        }

        //start all motors
        for (int k = 0; k < NUM_OF_TRINAMIC; k++) {
            start_motor(k);
        }
    }

    bool ping(int motor) {

        tmc4671_writeInt_verif(motor, TMC4671_CHIPINFO_ADDR, CHIPINFO_ADDR_SI_TYPE);
        uint32_t test = tmc4671_readInt(motor, TMC4671_CHIPINFO_DATA);

        if (test == 0x34363731) // "4671"
            return true;
        else
            return false;
    }

    void set_speed(int id_motor, float target) {
        // Calculate the right TMC4671 speed target; according to float target [m/s]
        int32_t speed = (int32_t) (target *
                                   (500000.0f / 176.0f)); // wheel diameter is 56mm, speed target is 500 to get one rps

        if (speed > DEBUG_MAX_TRINAMIC_SPEED) {
            speed = DEBUG_MAX_TRINAMIC_SPEED;
        } else if (speed < -DEBUG_MAX_TRINAMIC_SPEED) {
            speed = -DEBUG_MAX_TRINAMIC_SPEED;
        }

        tmc4671_writeInt_verif(id_motor, TMC4671_PID_VELOCITY_TARGET, speed);
    }

    SHELL_COMMAND(scan, "Scan for drivers") {
        for (int k = 0; k < 5; k++) {
            shell_print("Driver #");
            shell_print(k);
            shell_print(": ");
            if (ping(k)) {
                shell_println("Present!");
            } else {
                shell_println("-");
            }
        }
    }

    SHELL_COMMAND(set, "Set speed for one driver") {
        if (argc != 2) {
            shell_println("Usage: set [driver] [speed]");
        } else {
            while (!shell_available()) {
                drivers::set_speed(atoi(argv[0]), atof(argv[1]));
                wait_us(500);
            }
        }
    }

    SHELL_COMMAND(start, "Start a driver") {
        if (argc != 1) {
            shell_println("Usage: start [driver]");
        } else {
            while (!shell_available()) {
                drivers::start_motor(atoi(argv[0]));
                wait_us(500);
            }
        }
    }

    SHELL_COMMAND(motor_init, "Init a motor power board") {
        if (argc != 1) {
            shell_println("Usage: motor_init [driver]");
        } else {
            while (!shell_available()) {
                drivers::tmc4671_init(atoi(argv[0]));
                wait_us(500);
            }
        }
    }

    SHELL_COMMAND(stop, "Stop a driver") {
        if (argc != 1) {
            shell_println("Usage: stop [driver]");
        } else {
            while (!shell_available()) {
                drivers::stop_motor(atoi(argv[0]));
                wait_us(500);
            }
        }
    }

    void start_motor(uint8_t motor) {
        tmc4671_writeInt_verif(motor, TMC4671_PWM_SV_CHOP, 0x00000007); // Enable PWM Output
        tmc4671_writeInt_verif(motor, TMC4671_MODE_RAMP_MODE_MOTION, 0x00000002); // Velocity mode
    }

    void stop_motor(uint8_t motor, bool freewheel) {

        if (freewheel) {
            tmc4671_writeInt_verif(motor, TMC4671_PWM_SV_CHOP, 0x00000000); // 0 = PWM Disabled -> free running
        } else {
            tmc4671_writeInt_verif(motor, TMC4671_MODE_RAMP_MODE_MOTION, 0x00000000); // STOP MODE
            tmc4671_writeInt_verif(motor, TMC4671_PID_VELOCITY_TARGET, 0); // ensure no velocity
        }
    }


    // Setup for averaging ADC values to calculate offset
    const int numReadings = 20;
    uint16_t readings_I0[numReadings], readings_I1[numReadings];
    uint16_t index_I0 = 0, index_I1 = 0, average_I0 = 0, average_I1 = 0;
    int32_t total_I0 = 0, total_I1 = 0;

    // Use to averaging value of ADC to get proper offset (TODO: Improve this averaging function)
    void smooth_ADCs(uint16_t I0, uint16_t I1) {
        total_I0 = total_I0 - readings_I0[index_I0];
        readings_I0[index_I0] = I0;
        total_I0 = total_I0 + readings_I0[index_I0];
        index_I0++;
        if (index_I0 >= numReadings) {
            index_I0 = 0;
        }
        average_I0 = (uint16_t) (((uint32_t) total_I0) / ((uint32_t) numReadings));

        total_I1 = total_I1 - readings_I1[index_I1];
        readings_I1[index_I1] = I1;
        total_I1 = total_I1 + readings_I1[index_I1];
        index_I1++;
        if (index_I1 >= numReadings) {
            index_I1 = 0;
        }
        average_I1 = (uint16_t) (((uint32_t) total_I1) / ((uint32_t) numReadings));
    }

    void tmc4671_init(uint8_t motor) {
        uint8 is_tmc_4671 = 0;
        /*
     * ============ CHIP VERIFICATION ==============
     */
        tmc4671_writeInt_verif(motor, TMC4671_CHIPINFO_ADDR, CHIPINFO_ADDR_SI_TYPE);
        uint32_t test = tmc4671_readInt(motor, TMC4671_CHIPINFO_DATA);

        if (test == 0x34363731)
            is_tmc_4671++;

        uint8_t a = (test >> 24) & 0xFF;
        uint8_t b = (test >> 16) & 0xFF;
        uint8_t c = (test >> 8) & 0xFF;
        uint8_t d = (test) & 0xFF;

        driver_log("Chip info type : 0x%08lx (TMC-%c%c%c%c).\n", test, a, b, c, d);

        tmc4671_writeInt_verif(motor, TMC4671_CHIPINFO_ADDR, CHIPINFO_ADDR_SI_VERSION);
        test = tmc4671_readInt(motor, TMC4671_CHIPINFO_DATA);

        uint16_t e = (test >> 16) & 0xFFFF;
        uint16_t f = test & 0xFFFF;

        string rev;
        if ((e == 1) && (f == 0)) { // rev 1.0 => TMC4671-ES
            rev = "-ES";
            is_tmc_4671++;
        } else if ((e == 1) && (f == 3)) {  // rev 1.3 => TMC4671-LA
            rev = "-LA";
            is_tmc_4671++;
        } else
            rev = "-??";

        driver_log("Chip info version : 0x%08lx (rev %d.%d \"%s\").\n", test, e, f, rev.c_str());

        tmc4671_writeInt_verif(motor, TMC4671_CHIPINFO_ADDR, CHIPINFO_ADDR_SI_DATA);
        test = tmc4671_readInt(motor, TMC4671_CHIPINFO_DATA);

        driver_log("Chip info data : 0x%08lx .\n", test);

        tmc4671_writeInt_verif(motor, TMC4671_CHIPINFO_ADDR, CHIPINFO_ADDR_SI_TIME);
        test = tmc4671_readInt(motor, TMC4671_CHIPINFO_DATA);

        driver_log("Chip info time : 0x%08lx .\n", test);

        tmc4671_writeInt_verif(motor, TMC4671_CHIPINFO_ADDR, CHIPINFO_ADDR_SI_VARIANT);
        test = tmc4671_readInt(motor, TMC4671_CHIPINFO_DATA);

        driver_log("Chip info variant : 0x%08lx .\n", test);

        tmc4671_writeInt_verif(motor, TMC4671_CHIPINFO_ADDR, CHIPINFO_ADDR_SI_BUILD);
        test = tmc4671_readInt(motor, TMC4671_CHIPINFO_DATA);

        driver_log("Chip info build : 0x%08lx .\n", test);

        if (is_tmc_4671 != 2) {
            driver_log("It seems that the chip is not a TMC4671-ES or a TMC4671-LA, program can't continue.\n");
            return;
        } else
            driver_log("TMC4671 detected, configuring registers for SSL_Brushless motor control...\n");

        tmc4671_writeInt_verif(motor, TMC4671_MODE_RAMP_MODE_MOTION, 0x00000000); // Ensure motion is OFF (no PWM)
        ThisThread::sleep_for(200ms);
        /*
         * ============ MOTOR TYPE & PWM ==============
         */

        tmc4671_writeInt_verif(motor, TMC4671_MOTOR_TYPE_N_POLE_PAIRS,
                               0x00030008); // 0x0003 = BLDC, 0x0008 = number of pole pairs
        tmc4671_writeInt_verif(motor, TMC4671_PWM_POLARITIES, 0x00000000); // default
        tmc4671_writeInt_verif(motor, TMC4671_PWM_MAXCNT, 3999); // 25kHz
        tmc4671_writeInt_verif(motor, TMC4671_PWM_BBM_H_BBM_L, 0x00001919); // default (25 BBM)
        // This line could be used as an enable function :
        tmc4671_writeInt_verif(motor, TMC4671_PWM_SV_CHOP,
                               0x00000007); // 7 = centered PWM for FOC, 0 (bit8) = Space Vector PWM disabled

        /*
         * ============ ADC CONFIGURATION ==============
         */

        bool adc_to_be_configured = true;
        while (adc_to_be_configured) {
            // Reset for next motor (TODO: Improve this averaging function)
            for (int i = 0; i < numReadings; i++) {
                readings_I0[i] = 0;
                readings_I1[i] = 0;
            }
            index_I0 = 0;
            index_I1 = 0;
            average_I0 = 0;
            average_I1 = 0;
            total_I0 = 0;
            total_I1 = 0;

            tmc4671_writeInt_verif(motor, TMC4671_ADC_I_SELECT, 0x24000100); // default
            tmc4671_writeInt_verif(motor, TMC4671_dsADC_MCFG_B_MCFG_A, 0x00100010);  // default
            tmc4671_writeInt_verif(motor, TMC4671_dsADC_MCLK_A, 0x20000000);
            tmc4671_writeInt_verif(motor, TMC4671_dsADC_MCLK_B, 0x00000000);
            tmc4671_writeInt_verif(motor, TMC4671_dsADC_MDEC_B_MDEC_A, 0x014E014E); // default

            ThisThread::sleep_for(100ms); //just in case

            // Setup ADC_RAW_ADDR to show I0 and I1 ADC
            tmc4671_writeInt_verif(motor, TMC4671_ADC_RAW_ADDR, ADC_RAW_ADDR_ADC_I1_RAW_ADC_I0_RAW);
            uint32_t ADCs_raw;
            uint16_t ADC_I0, ADC_I1;

            // Do an averaging of ADC values to get proper offset (different for each board)
            for (int i = 0; i < numReadings; i++) {

                bool adc_not_ok = true;
                // get raw ADC data
                while (adc_not_ok) {
                    ADCs_raw = tmc4671_readInt(motor, TMC4671_ADC_RAW_DATA);
                    ADC_I0 = (uint16_t) (ADCs_raw & 0x0000FFFF);
                    ADC_I1 = (uint16_t) ((ADCs_raw & 0xFFFF0000) >> 16);

                    if ((ADC_I0 < 26000) | (ADC_I1 < 26000) | (ADC_I0 > 38000) | (ADC_I1 > 38000)) {
                        adc_not_ok = true; // Error while reading, probably missing a byte
                    } else {
                        adc_not_ok = false;
                    }
                }

                driver_log("* (i=%d) ADC_I0 = %d\tADC_I1 = %d\n", i, ADC_I0, ADC_I1);

                smooth_ADCs(ADC_I0, ADC_I1);

                ThisThread::sleep_for(20ms);
            }

            int32_t delta_average = abs(int32_t(average_I0) - int32_t(average_I1));
            if (delta_average > 3000) {
                driver_log("Error while computing average I0, too much delta (%ld), trying again ...\n", delta_average);

            } else {
                adc_to_be_configured = false;

                // Debug
                driver_log("ADC OFFSET of I0 : %d\n", average_I0);
                driver_log("ADC OFFSET of I1 : %d\n", average_I1);

                // Write new offset.
                // Scale has been decreased (instead of default 256) to get better response from PID, because hardware shunt amplifier have "too much" gain.
                tmc4671_writeInt_verif(motor, TMC4671_ADC_I0_SCALE_OFFSET,
                                       49 | 
                                       ((int32_t) average_I0)); // scale 60, offset = calculated average before
                tmc4671_writeInt_verif(motor, TMC4671_ADC_I1_SCALE_OFFSET,
                                       49 |
                                       ((int32_t) average_I1)); // scale 60, offset = calculated average before
            }
        }

        /*
         * ============ HALL CONFIGURATION ==============
         */

        // nothing to do, all is good by default
        tmc4671_writeInt_verif(motor, TMC4671_HALL_MODE, 0x00000000); // normal, all default

        /*
         * ============ PID LIMITS CONFIGURATION ==============
         */

        tmc4671_writeInt_verif(motor, TMC4671_PIDOUT_UQ_UD_LIMITS,
                               9500); // 9500 == 1.9A max
        tmc4671_writeInt_verif(motor, TMC4671_PID_TORQUE_FLUX_LIMITS,
                               12000); // 12 000
        tmc4671_writeInt_verif(motor, TMC4671_PID_ACCELERATION_LIMIT, 0xFFFFFFFF); // 10 000, does not seems to work
        tmc4671_writeInt_verif(motor, TMC4671_PID_VELOCITY_LIMIT,
                          24000); // 24 000
        tmc4671_writeInt_verif(motor, TMC4671_PID_POSITION_LIMIT_LOW, 0x80000001); // no limit
        tmc4671_writeInt_verif(motor, TMC4671_PID_POSITION_LIMIT_HIGH, 0x7FFFFFFF); // no limit

        /*
         * ============ PID VALUES TORQUE/FLUX CONFIGURATION ==============
         */

        tmc4671_writeInt_verif(motor, TMC4671_PID_FLUX_P_FLUX_I, 0x01000100); // P=256 & I=256 (default)
        tmc4671_writeInt_verif(motor, TMC4671_PID_TORQUE_P_TORQUE_I, 0x01000100); // P=256 & I=256 (default)


        /*
         * ============ ABn ENCODER CONFIGURATION ==============
         */

        tmc4671_writeInt_verif(motor, TMC4671_ABN_DECODER_MODE, 0x00000000); // normal, all default
        tmc4671_writeInt_verif(motor, TMC4671_ABN_DECODER_PPR, 0x000007D0); // Encoder PPR = 2000
        tmc4671_writeInt_verif(motor, TMC4671_VELOCITY_SELECTION, 0x00000000); // phi_e selected from PHI_E_SELECTION
        tmc4671_writeInt_verif(motor, TMC4671_POSITION_SELECTION, 0x00000000); // phi_e selected from PHI_E_SELECTION
        tmc4671_writeInt_verif(motor, TMC4671_PHI_E_SELECTION,
                               0x00000003); // PHI_E_SELECTION = phi_e_abn (the motor ABn encoder)

        /*
         *  ============ ABn ENCODER OFFSET INITIALISATION ==============
         */
        driver_log("init encoder PHI E offset using halls ...\n");


        // Hack the lib to force a encoder initialisation using motor Halls
        uint8_t mode = 2; // use hall sensor signals to do initialisation
        uint8_t initMode = 0;
        uint8_t initState = 0; // nothing to do
        tmc4671_startEncoderInitialization(mode, &initMode, &initState);

        // Begin the periodic job one time to start the initialisation
        int16_t hall_phi_e_old, hall_phi_e_new, hall_actual_coarse_offset;
        uint16_t last_Phi_E_Selection;

        // save actual set value for PHI_E_SELECTION
        last_Phi_E_Selection = (uint16_t) tmc4671_readRegister16BitValue_verif(motor, TMC4671_PHI_E_SELECTION,
                                                                               BIT_0_TO_15);

        // turn hall_mode interpolation off (read, clear bit 8, write back)
        tmc4671_writeInt_verif(motor, TMC4671_HALL_MODE, tmc4671_readInt_verif(motor, TMC4671_HALL_MODE) & 0xFFFFFEFF);

        // set ABN_DECODER_PHI_E_OFFSET to zero
        tmc4671_writeRegister16BitValue_verif(motor, TMC4671_ABN_DECODER_PHI_E_PHI_M_OFFSET, BIT_16_TO_31, 0);

        // read actual hall angle
        hall_phi_e_old = tmc4671_field_read_verif(motor, TMC4671_HALL_PHI_E_INTERPOLATED_PHI_E, TMC4671_HALL_PHI_E_MASK,
                                                  TMC4671_HALL_PHI_E_SHIFT);

        // read actual abn_decoder angle and compute difference to actual hall angle
        hall_actual_coarse_offset = tmc4671_getS16CircleDifference(hall_phi_e_old,
                                                                   (int16_t) tmc4671_readRegister16BitValue_verif(motor,
                                                                                                                  TMC4671_ABN_DECODER_PHI_E_PHI_M,
                                                                                                                  BIT_16_TO_31));

        // set ABN_DECODER_PHI_E_OFFSET to actual hall-abn-difference, to use the actual hall angle for coarse initialization
        tmc4671_writeRegister16BitValue_verif(motor, TMC4671_ABN_DECODER_PHI_E_PHI_M_OFFSET, BIT_16_TO_31,
                                              hall_actual_coarse_offset);

        // normally MOTION_MODE_UQ_UD_EXT is only used by e.g. a wizard, not in normal operation
        if (tmc4671_field_read_verif(motor, TMC4671_MODE_RAMP_MODE_MOTION, TMC4671_MODE_MOTION_MASK,
                                     TMC4671_MODE_MOTION_SHIFT) !=
            TMC4671_MOTION_MODE_UQ_UD_EXT) {
            // select the use of phi_e_hall to start motor with hall signals
            tmc4671_writeRegister16BitValue_verif(motor, TMC4671_PHI_E_SELECTION, BIT_0_TO_15, TMC4671_PHI_E_HALL);
        }

        initState = 2;

        // Move the motor a bit using HALL
        tmc4671_writeInt_verif(motor, TMC4671_PID_VELOCITY_P_VELOCITY_I,
                               50<<16 | 0); // P=50 & I=50 (only when using halls)
        tmc4671_writeInt_verif(motor, TMC4671_MODE_RAMP_MODE_MOTION, 0x00000002); // Velocity mode
        tmc4671_writeInt_verif(motor, TMC4671_PID_VELOCITY_TARGET, 40); //move the motor a bit

        // Call the periodicJob until hall angle change to set the right phi E offset (see comments in TMC4671 library)
        uint16_t enc_init_loop = 0;
        while (initState != 0) {
//        tmc4671_periodicJob(motor, 0, initMode, &initState, 0, &actualInitWaitTime, 0, &hall_phi_e_old, &hall_phi_e_new,
//                            &hall_actual_coarse_offset, &last_Phi_E_Selection, &last_UQ_UD_EXT, &last_PHI_E_EXT);

            // read actual hall angle
            hall_phi_e_new = tmc4671_field_read_verif(motor, TMC4671_HALL_PHI_E_INTERPOLATED_PHI_E,
                                                      TMC4671_HALL_PHI_E_MASK,
                                                      TMC4671_HALL_PHI_E_SHIFT);

            // wait until hall angle changed
            if (hall_phi_e_old != hall_phi_e_new) {
                tmc4671_writeInt_verif(motor, TMC4671_MODE_RAMP_MODE_MOTION, 0x00000000); // STOP MODE
                // estimated value = old value + diff between old and new (handle int16_t overrun)
                int16_t hall_phi_e_estimated =
                        hall_phi_e_old + tmc4671_getS16CircleDifference(hall_phi_e_new, hall_phi_e_old) / 2;

                // read actual abn_decoder angle and consider last set abn_decoder_offset
                int16_t abn_phi_e_actual =
                        ((int16_t) tmc4671_readRegister16BitValue_verif(motor, TMC4671_ABN_DECODER_PHI_E_PHI_M,
                                                                        BIT_16_TO_31)) - hall_actual_coarse_offset;

                // set ABN_DECODER_PHI_E_OFFSET to actual estimated angle - abn_phi_e_actual difference
                tmc4671_writeRegister16BitValue_verif(motor, TMC4671_ABN_DECODER_PHI_E_PHI_M_OFFSET, BIT_16_TO_31,
                                                      tmc4671_getS16CircleDifference(hall_phi_e_estimated,
                                                                                     abn_phi_e_actual));

                // switch back to last used PHI_E_SELECTION setting
                tmc4671_writeRegister16BitValue_verif(motor, TMC4671_PHI_E_SELECTION, BIT_0_TO_15,
                                                      last_Phi_E_Selection);

                // go to ready state
                initState = 0;
            }


            enc_init_loop++;

            if (enc_init_loop > 20000) {
                driver_log("Error while trying to init ABn encoder, motor didn't move (Halls not wired ?).\nStopping program.");
                initState = 0;
                tmc4671_writeInt_verif(motor, TMC4671_PWM_SV_CHOP, 0x00000000); // 0 = PWM Disabled -> free running
                return;
            }
        }

        tmc4671_writeInt_verif(motor, TMC4671_PWM_SV_CHOP, 0x00000000); // 0 = PWM Disabled -> free running
        tmc4671_writeInt_verif(motor, TMC4671_MODE_RAMP_MODE_MOTION, 0x00000000); // STOP MODE
        tmc4671_writeInt_verif(motor, TMC4671_PID_VELOCITY_TARGET, 0);
        ThisThread::sleep_for(200ms);
        driver_log("ABn offset has been set to : %d\n",
               (int16_t) tmc4671_readRegister16BitValue(motor, TMC4671_ABN_DECODER_PHI_E_PHI_M_OFFSET, BIT_16_TO_31));
        driver_log("Number of loop to get the offset : %d\n", enc_init_loop);



        // ---- END OF ENOCDER INIT -----

        /*
         * ============ PID VALUES CONFIGURATION ==============
        */

        // VALUE FOR VELOCITY CONTROL
        tmc4671_writeInt_verif(motor, TMC4671_PID_VELOCITY_P_VELOCITY_I,100 + ( 500 << 16));
        tmc4671_writeInt_verif(motor, TMC4671_PID_POSITION_P_POSITION_I, 0x00000000); // P=0 & I=0

        // VALUE FOR POSITION CONTROL
        // tmc4671_writeInt_verif(motor, TMC4671_PID_VELOCITY_P_VELOCITY_I, 0x01F40028); // P=500 & I=40 (arbitrary from live test, good results)
        // tmc4671_writeInt_verif(motor, TMC4671_PID_POSITION_P_POSITION_I , 0x00500000); // P=80 & I=0 (arbitrary from live test, good results)
    }

    void setVelocityGain(uint16_t P, uint16_t I) {
        for(uint8_t i = 0; i < 4; i++) {
            tmc4671_writeInt_verif(i, TMC4671_PID_VELOCITY_P_VELOCITY_I, (uint32_t) I + (P << 16) );
        }
        common::swo.print("GAIN: P:");
        common::swo.println((int) P << 16);
        common::swo.print("GAIN: I:");
        common::swo.println((int) I);
    }
    
    // void setTorqueGain(uint16_t P, uint16_t I) {
    //     for(uint8_t i = 0; i < 4; i++) {
    //         tmc4671_writeInt_verif(i, TMC4671_PID_FLUX_P_FLUX_I, (uint32_t) (P << 16) | I);
    //     }
    // }

    void tmc4671_writeInt_verif(uint8_t motor, uint8_t address, int32_t value) {

        bool loop = true;
        while (loop) {
            tmc4671_writeInt(motor, address, value);
            int32_t verif = tmc4671_readInt(motor, address);

            if (verif != value) {
                driver_log("---Error SPI while writing 0x%08lx to register 0x%04x , trying again ...\n", value, address);
                ThisThread::sleep_for(10ms);
            } else {
                loop = false;
            }
        }
    }

// USE THS FUNCTION ONLY IF THE RESULT SHOULD BE ALWAYS THE SAME !!
    int32_t tmc4671_readInt_verif(uint8_t motor, uint8_t address) {

        int32_t reading[MAX_READING_VERIF] = {0};
        bool reading_not_good = true;

        while (reading_not_good) {

            for (int i = 0; i < MAX_READING_VERIF; i++) {
                reading[i] = tmc4671_readInt(motor, address);
            }

            reading_not_good = false;

            for (int i = 0; i < MAX_READING_VERIF; i++) {
                driver_log("* (i=%d) SPI INT READ =  %ld\n", i, reading[i]);
                if (i == (MAX_READING_VERIF - 1)) {
                    if (reading[i] != reading[0]) {
                        reading_not_good = true;
                    }
                } else {
                    if (reading[i] != reading[i + 1]) {
                        reading_not_good = true;
                    }
                }

            }

            if (reading_not_good) {
                driver_log("---Error SPI while reading register 0x%04x , trying again ...\n", address);
//            ThisThread::sleep_for(10ms);
            }
        }

        return reading[0];
    }

    void tmc4671_writeRegister16BitValue_verif(uint8_t motor, uint8_t address, uint8_t channel, uint16_t value) {
        // read actual register content
        int32_t registerValue = tmc4671_readInt_verif(motor, address);

        // update one channel
        switch (channel) {
            case BIT_0_TO_15:
                registerValue &= 0xFFFF0000;
                registerValue |= value;
                break;
            case BIT_16_TO_31:
                registerValue &= 0x0000FFFF;
                registerValue |= (value << 16);
                break;
        }
        // write the register
        tmc4671_writeInt_verif(motor, address, registerValue);
    }

    uint16_t tmc4671_readRegister16BitValue_verif(uint8_t motor, uint8_t address, uint8_t channel) {
        int32_t registerValue = tmc4671_readInt_verif(motor, address);

        // read one channel
        switch (channel) {
            case BIT_0_TO_15:
                return (registerValue & 0xFFFF);
                break;
            case BIT_16_TO_31:
                return ((registerValue >> 16) & 0xFFFF);
                break;
        }
        return 0;
    }


    int16_t tmc4671_getS16CircleDifference(int16_t newValue, int16_t oldValue) {
        return (newValue - oldValue);
    }

    int16_t tmc4671_field_read_verif(uint8_t motor, uint8_t address, uint16_t mask, int16_t shift) {

//    TMC4671_FIELD_READ(motor, address, mask, shift);

        int16_t reading[MAX_READING_VERIF] = {0};
        bool reading_not_good = true;

        while (reading_not_good) {

            for (int i = 0; i < MAX_READING_VERIF; i++) {
                reading[i] = TMC4671_FIELD_READ(motor, address, mask, shift);
            }

            reading_not_good = false;

            for (int i = 0; i < MAX_READING_VERIF; i++) {
                driver_log("* (i=%d) SPI FIELD READ =  %hd\n", i, reading[i]);
                if (i == (MAX_READING_VERIF - 1)) {
                    if (reading[i] != reading[0]) {
                        reading_not_good = true;
                    }
                } else {
                    if (reading[i] != reading[i + 1]) {
                        reading_not_good = true;
                    }
                }

            }

            if (reading_not_good) {
                driver_log("---Error SPI while reading register 0x%04x , trying again ...\n", address);
//            ThisThread::sleep_for(10ms);
            }
        }

        return reading[0];
    }

}


// This function can't be in "drivers" namespace
uint8_t tmc4671_readwriteByte(uint8_t motor, uint8_t data, uint8_t lastTransfer) {

    drivers::drivers_out[motor] = 0; //select the right TMC4671
    uint8_t temp = drivers::drivers.write(data);

    if (lastTransfer) {
        drivers::drivers_out[motor] = 1; //deselect TMC4671
    }
    return temp;
}

