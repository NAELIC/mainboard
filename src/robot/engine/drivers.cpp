#include "drivers.h"

namespace drivers {

    SPI drivers(DRV_MOSI, DRV_MISO, DRV_CLK);
    DigitalOut drivers_out[NUM_DRIVERS] = {
        DRIVERS_CS1, DRIVERS_CS2, DRIVERS_CS3, DRIVERS_CS4, DRIVERS_CS5};

    void init() {
        // 40 bits datagram (datasheet TMC4671 p.16), but 16 max for TMC4671
        // module, so 8x5=40
        drivers.format(8, 3);
        drivers.frequency(1000000);

        for (int k = 0; k < NUM_DRIVERS; k++) {
            drivers::drivers_out[k] = 1;
        }
    }
}  // namespace drivers