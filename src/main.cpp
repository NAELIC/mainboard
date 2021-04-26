#include <mbed.h>
#include <USBSerial.h>
#include "shell.h"
#include "buzzer.h"
#include "hardware.h"

uint32_t TIMEOUT_WATCHDOG_MS = 2000;
DigitalOut led(LED1);

int main()
{
    shell_init_usb();
    //Watchdog &watchdog = Watchdog::get_instance();
    //watchdog.start(TIMEOUT_WATCHDOG_MS);

    // Buzzer
        
        buzzer_init();  
        //ir_init();


    //RICKroll
    
        buzzer_init();

        // buzzer_play(RICKROLL);
        // buzzer_wait_play();
        // wait_us(2000000);


    while (true) {
        // ir
        buzzer_tick();
        //ir_tick();

        
        led = 0;
        ThisThread::sleep_for(500ms);
        led = 1;
        ThisThread::sleep_for(500ms);
    }
}


//afficher valeur millis dans le tickn pour changement.
