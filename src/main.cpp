#include <mbed.h>
#include <USBSerial.h>
#include "shell.h"
#include "buzzer.h"
#include "hardware.h"

uint32_t TIMEOUT_WATCHDOG_MS = 2000;
DigitalOut led(LED1);

PwmOut buzzer(BUZZER_PIN);

int main()
{
    shell_init_usb();
    Watchdog &watchdog = Watchdog::get_instance();
    //watchdog.start(TIMEOUT_WATCHDOG_MS);

    // Buzzer
        buzzer_init();
        // buzzer.period(1.0/890.0);
        // buzzer = 0.5;
        // wait_us(1000000);
        // buzzer.period(1.0/494);
        // buzzer = 0.5;
        // wait_us(500000);
        // buzzer = 0.0;        
        

    //RICKroll
        // buzzer_play(RICKROLL);
        // buzzer_wait_play();
        // wait_us(2000000);

    


    while (true) {
        // ir
        buzzer_tick();
        

        
        // led = 0;
        // ThisThread::sleep_for(500ms);
        // led = 1;
        // ThisThread::sleep_for(500ms);
    }
}


// #include "mbed.h"
// #include "hardware.h"
 
// PwmOut buzzer(BUZZER_PIN);
 
// int main() {
 
//  while(1){
   
//     buzzer.period(1.0/659.0);
//     buzzer=0.5;
//     wait_us(100000);
//     buzzer.period(1.0/494.0);
//     buzzer=0.5;
//     wait_us(100000);
//     buzzer.period(1.0/554.0);
//     buzzer=0.5;
//     wait_us(100000);
//     buzzer=0.0; // turn off audio 
    
//     for (float i=0; i<26; i=i+5) {
//             buzzer.period(1.0/969.0);
//             buzzer = i/50.0f;
//             wait_us(50000);
//             buzzer.period(1.0/800.0);
//             wait_us(50000);
//     }
//     buzzer=0.0; // turn off audio 
  
//  }
// }