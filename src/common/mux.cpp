#include "mux.h"

//pin init    
AnalogIn mux(MUX);
DigitalOut addr1(ADDR1, 0);
DigitalOut addr2(ADDR2, 0);
DigitalOut addr3(ADDR3, 0);

unsigned short mux_sample(int addr)
{

    addr1 = (((addr>>0)&1) ? 1 : 0 );
    addr2 = (((addr>>1)&1) ? 1 : 0 );
    addr3 = (((addr>>2)&1) ? 1 : 0 );

    return mux.read_u16();
}

bool get_hall(int addr)
{
  return mux_sample(addr)<HALL_THRES;
}

SHELL_COMMAND(mdb, "Mux debug")
{
    for (int k=0; k<8; k++) {
        shell_println("Mux #");
        shell_println(k);
        shell_println(": ");
        shell_println(mux_sample(k));
        // Watchdog::get_instance().kick();
    }
}
