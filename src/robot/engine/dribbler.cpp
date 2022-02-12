#include "dribbler.h"

static naelic::SWO swo;

namespace dribbler {
    SPI drivers(DRV_MOSI, DRV_MISO, DRV_CLK);
    DigitalOut drivers_out(DRIVERS_CS5);

    void init() {
        drivers.frequency(9600);
        assert(sizeof(dribbler_packet_set) > sizeof(dribbler_packet_ans));
    }

    void send(uint8_t *data, size_t len) {
        drivers_out = 0;  // Select the dribbler
        for (int i = 0; i < len; i++) {
            int temp = drivers.write(data[i]);
            swo.println(temp);
        }

        drivers_out = 1;  // Deselect dribbler
    }

    void set_speed(uint32_t speed) {
        dribbler_packet_set packet;
        packet.targetSpeed = speed;
        packet.padding1 = 0;
        packet.padding2 = 0;

        send((uint8_t*) &packet, sizeof(dribbler_packet_set));
    }
}  // namespace dribbler

SHELL_COMMAND(set_dribbler, "Set speed for dribbler") {
    if (argc != 1) {
        shell_println("Usage: set_dribbler [speed]");
    } else {
        while (!shell_available()) {
            dribbler::set_speed(atof(argv[0]));
            wait_us(500);
        }
    }
}

//   void init()
//   {
//     dribbler.frequency(250000);
//     dribbler_out = 1;
//   }

//   bool ping()
//   {
//     dribbler_out = 0;
//     wait_us(35);
//     dribbler.write(0);
//     uint8_t status = dribbler.write(0);
//     wait_us(5);
//     dribbler_out = 1;
//     swo.println(status);
//     return (status == 0x55 || ((status & 0xf0) == 0x80));
//   }

//   void set_speed(float target)
//   {
//     dribbler_packet_set packet;
//     packet.enable = true;
//     packet.pwm = 0;
//     packet.targetSpeed = target;
//     packet.padding1 = 0;
//     packet.padding2 = 0;

//     dribbler_packet_ans ans;

//     dribbler_out = 0;
//     wait_us(35);
//     dribbler.write(DRIBBLER_PACKET_SET);
//     dribbler.write((char *)&packet, sizeof(dribbler_packet_set), (char
//     *)&ans, sizeof(dribbler_packet_ans)); wait_us(5); dribbler_out = 1;
//   }

//   void launch()
//   {
//     init();
//     while (true)
//     {
//       // Tâche de fond pour les erreurs
//       // Pilotage du dribbler en fonction des messages reçus
//       // si on est pas en débug.
//       ThisThread::sleep_for(100ms);
//     }
//   }

//   SHELL_COMMAND(scan_dribbler, "Scan for dribbler")
//   {
//     shell_print("Dribbler : ");
//     if (ping())
//     {
//     shell_println("Present!");
//     }
//     else
//     {
//     shell_println("-");
//     }
//   }
