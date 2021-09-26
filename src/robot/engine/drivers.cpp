#include "drivers.h"

static naelic::SWO swo;

namespace drivers
{
	static SPI drivers(DRV_MOSI, DRV_MISO, DRV_CLK);
	static DigitalOut drivers_out[5] = {DRIVERS_CS1,
										DRIVERS_CS2,
										DRIVERS_CS3,
										DRIVERS_CS4,
										DRIVERS_CS5};

	void init()
	{
		drivers.frequency(250000);
		for (int k = 0; k < 5; k++)
		{
			drivers_out[k] = 1;
		}
	}

	bool ping(int index)
	{
		drivers_out[index] = 0;
		wait_us(35);
		drivers.write(0);
		uint8_t status = drivers.write(0);
		wait_us(5);
		drivers_out[index] = 1;
		swo.println(status);
		return (status == 0x55 || ((status & 0xf0) == 0x80));
	}

	void set_speed(int id_motor, float target)
	{
		driver_packet_set packet;
		packet.enable = true;
		packet.pwm = 0;
		packet.targetSpeed = target;
		packet.padding1 = 0;
		packet.padding2 = 0;

		driver_packet_ans ans;

		drivers_out[id_motor] = 0;
		wait_us(35);
		drivers.write(DRIVER_PACKET_SET);
		drivers.write((char *)&packet, sizeof(driver_packet_set), (char *)&ans, sizeof(driver_packet_ans));
		wait_us(5);
		drivers_out[id_motor] = 1;
	}

	/* relationship between the wheels speed v0, v1, v2 and v3, with the robot speeds x, y and t
| v0(t) |   | 0  1  d |    
| v1(t) |   |-1  0  d |   | x_speed(t) |
| v2(t) | = | 0 -1  d | * | y_speed(t) |
| v3(t) |	| 1  0  d |   | t_speed(t) |
*/
	float *com_packet_to_driver_packet(engine_msg packet)
	{ //TODO: change name of function, change variable 'd'
		float speed_for_drivers[4];
		float d = 9.0; /* is distance between center of robot and wheel */
		speed_for_drivers[0] = packet.y_speed + d * packet.t_speed;
		speed_for_drivers[1] = d * packet.t_speed - packet.x_speed;
		speed_for_drivers[2] = d * packet.t_speed - packet.y_speed;
		speed_for_drivers[3] = packet.x_speed + d * packet.t_speed;
		return speed_for_drivers;
	}

	void launch()
	{
		init();
		while (true)
		{
			// Tâche de fond pour les erreurs
			// Pilotage des moteurs en fonction des messages reçus
			// si on est pas en débug.
			ThisThread::sleep_for(100ms);
		}
	}

	SHELL_COMMAND(scan, "Scan for drivers")
	{
		for (int k = 0; k < 5; k++)
		{
			shell_print("Driver #");
			shell_print(k);
			shell_print(": ");
			if (ping(k))
			{
				shell_println("Present!");
			}
			else
			{
				shell_println("-");
			}
		}
	}

	SHELL_COMMAND(set, "Set speed for one driver")
	{
		if (argc != 2)
		{
			shell_println("Usage: set [driver] [speed]");
		}
		else
		{
			while (!shell_available())
			{
				drivers::set_speed(atoi(argv[0]), atof(argv[1]));
				wait_us(500);
			}
		}
	}
}