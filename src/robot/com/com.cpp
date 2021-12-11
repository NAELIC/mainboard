#include "com.h"
#include <nrf.h>
#include "../infos.h"
#include "../engine/drivers.h"
#include <common/buzzer/buzzer.h>
#include "../ir/ir.h"
#include "../kicker/kicker.h"
#include "../voltage/voltage.h"
#include "../engine/kinematic.h"
#include "common/define/common.h"

constexpr long SPI_COM_FREQUENCY = 2250000;

// last byte will be from FF for tx and C0 to C5 for pipes 1 to 6
#define addr_for_status {0xC0, 0xB4, 0xC5, 0xD6, 0XA3};
#define status_chan 110
#define status_payload_size sizeof(packet_robot)
#define STATUS_TIMEOUT_MS 500

// last byte will be from A0 to A5 for master to robot com
#define addr_for_orders {0x8A,0x7B,0x6C,0x5D,0xA0};
#define orders_chan 116
#define order_payload_size sizeof(packet_master)
#define ORDER_TIMEOUT_MS 5000

#define icmp_chan 120
#define addr_for_icmp {0xA1,0xA3,0xB6,0xE4,0xB6};
#define icmp_payload_size sizeof(icmp_order)
#define ICMP_TIMEOUT_MS 1000

namespace com
{
  bool first_order_timeout = true;  
  // STATE variable
  State state = INIT;
  int last_order_received = 0;
  int last_status_ack;
  int icmp_stamp=0;
  int wait_init_timeout;  
  unsigned long int status_send_ok=0;
  unsigned long int status_send_lost=0;
  unsigned long int dhcp_failure=0;
  uint16_t last_order_id=0;  
  icmp::Order dhcp_request;
  packet_master order={0,ACTION_ON,0,0,0,0};
    

  void apply_order(packet_master &master_packet);

  void build_status(packet_status &packet, int last_order_id)
  {
      packet.id = infos_get_id();
      packet.status = STATUS_OK;

      // TODO(11/19/2021): add a way to check trinamic status
      //if (!drivers_is_all_ok()) {
      //    packet.status |= STATUS_DRIVER_ERR;
      //}
  
      if (ir::present()) {
          packet.status |= STATUS_IR;
      }
  
      packet.cap_volt = kicker_cap_voltage();
      packet.voltage  = voltage::value()*8.0;
      packet.xpos     = 0;   // getOdometry().xpos*1000;
      packet.ypos     = 0;   // getOdometry().ypos*1000;
      packet.ang      = 0;   // getOdometry().ang*1000;
      packet.last_order_id = last_order_id;
  }

  void launch()
  {
    robot_init();

    while (true)
    {
      if(isComAvailable())
      switch (state)
      {
      case INIT:
        common::swo.println("[com] INIT");
       // Send a DHCP Request and wait for answer
        if (com_send(CARD_ICMP,(uint8_t *)&dhcp_request, sizeof(dhcp_request))){
            com_set_state(CARD_ICMP,RX);
            state=WAIT_DHCP;
            icmp_stamp = Kernel::get_ms_count();
          //  SerialUSB.println("wait for dhcp reply");
        } else {
            dhcp_failure+=1;
            com_flush_tx(CARD_ICMP);
            state=WAIT_AND_INIT;
            wait_init_timeout= Kernel::get_ms_count() + 100; // wait 100ms
        }
        break;
      case WAIT_AND_INIT:
       if ( Kernel::get_ms_count() > wait_init_timeout)
            state=INIT;
        break;
      case WAIT_DHCP:
        common::swo.println("[com] WAIT DHCP");
        if ((Kernel::get_ms_count() - icmp_stamp)>ICMP_TIMEOUT_MS){
            //buzzer_play(MELODY_WARNING, false);
            dhcp_failure+=1;
            //SerialUSB.println("dhcp timeout");
            state=INIT;
        } else {
            if ((com_has_data(CARD_ICMP))>=0){
                icmp::Order o;
                com_receive(CARD_ICMP,(uint8_t *)&o, sizeof(o));
                /*
                SerialUSB.print("receive icmp reply type: ");
                SerialUSB.print(o.icmp_type);
                SerialUSB.print(" / ");
                SerialUSB.println(o.arg);
                */
                if (o.icmp_type == icmp::DHCP_REPLY){
                    if (o.arg == icmp::FULL){
                        //buzzer_play(MELODY_WARNING, false);
                        state=WAIT_AND_INIT;
                        wait_init_timeout=Kernel::get_ms_count()+2000; // wait for 2s
                    } else {
                        //buzzer_play(MELODY_BEGIN, false);
                        com_set_tx_addr(CARD_STATUS,o.icmp_addr);
                        com_set_rx_addr(CARD_STATUS,0,o.icmp_addr);
                        com_set_pipe_payload(CARD_STATUS,0, sizeof(packet_status));
                        com_set_state(CARD_STATUS,RX);
                        state=RUNNING;
                        status_send_ok=0;
                        status_send_lost=0;
                        last_status_ack= Kernel::get_ms_count();
                    }
                } else {
                    //SerialUSB.println("wrong icmp reply type");
                    state=INIT;
                }
                com_flush_rx(CARD_ICMP);
            }
        }
        break;
      case RUNNING:
        common::swo.println("[com] RUNNING");

//        static int counter=0;
//        if (counter<10){
//            counter+=1;
//            return;
//        }
//        counter=0;

        if (com_has_data(CARD_ORDER)!=-1){
            common::swo.println("data received");
            first_order_timeout=true;
            last_order_received = Kernel::get_ms_count();
            com_receive(CARD_ORDER,(uint8_t *)&order,order_payload_size);
            common::swo.println("order receive: action");
            
            // common::swo.print(" | kickpow: ");
            // common::swo.print(order.kickPower);
            common::swo.print(" | x : ");
            common::swo.print(order.x_speed);
            common::swo.print(" | y : ");
            common::swo.print(order.y_speed);
            common::swo.print(" | t : ");
            common::swo.print(order.t_speed);
            common::swo.println("");


//            SerialUSB.print("order receive: action:");
//            print_byte_as_hex(order.actions);
//            SerialUSB.print(" | kickpow: ");
//            SerialUSB.print(order.kickPower);
//            SerialUSB.print(" | x : ");
//            SerialUSB.print(order.x_speed);
//            SerialUSB.print(" | y : ");
//            SerialUSB.print(order.y_speed);
//            SerialUSB.print(" | t : ");
//            SerialUSB.print(order.t_speed);
//            SerialUSB.println();
            kinematic::apply_order(order.x_speed/1000.0, order.y_speed/1000.0,
                          order.t_speed/1000.0);
            com_flush_rx(CARD_ORDER);
            com_clear_status(CARD_ORDER);

        }

        packet_status status;
        build_status(status, last_order_id);
        if (com_send(CARD_STATUS,(uint8_t *)&status, sizeof(packet_status))){
            last_status_ack= Kernel::get_ms_count();
            status_send_ok+=1;
        } else status_send_lost+=1;

        // if ((Kernel::get_ms_count()-last_order_received)>ORDER_TIMEOUT_MS){
        //     if (first_order_timeout){
        //         //buzzer_play(MELODY_ALERT_FAST, false);
        //         first_order_timeout=false;
        //     }
        //     //state=INIT;
        //     order.id = infos_get_id();
        //     order.actions=0;
        //     order.t_speed=0;
        //     order.x_speed=0;
        //     order.y_speed=0;
        //     order.kickPower=0;
        // }
        // if ((Kernel::get_ms_count()-last_status_ack)>STATUS_TIMEOUT_MS){
        //     //buzzer_play(MELODY_ALERT_FAST, false);
        //     state=INIT;
        //     order.actions=0;
        //     order.t_speed=0;
        //     order.x_speed=0;
        //     order.y_speed=0;
        //     order.kickPower=0;
        // }

        // if ( order.id == infos_get_id()) {
          // ensure there is at least 4ms between two order (250Hz order freq)
          //if ((Kernel::get_ms_count()-last_order_received) > 4) {
            //   if (!kinematic::isManualControl())
                // apply_order(order);
         // }
        //  }
        break;
      }
     
      ThisThread::sleep_for(100ms);
      // common::swo.println(common::mode);
      // if (common::mode == Mode::NORMAL)
      // {
      //   common::swo.println("TODO");
      // }
      // else
      // {
      //   ThisThread::sleep_for(1s);
      // }
    }
  }

  void robot_init(){
      com_init(); // default card setup
      state=INIT;
      for(int card=0;card<3;++card){
          com_set_ack(card,true); // set ACK
          com_set_crc(card,2); // 2bytes for CRC
          //com_set_rf(card,SPEED_2M,POW_0db);
          com_clear_status(card);
      }
  
      icmp::card_icmp_ok = com_is_ok(CARD_ICMP);
      icmp::card_status_ok = com_is_ok(CARD_STATUS);
      icmp::card_order_ok = com_is_ok(CARD_ORDER);
  
  
      // use stm32 uid to fix the address:
      uint8_t id[12];
      get_uid(id);
      unsigned int d= Kernel::get_ms_count();
      uint8_t uaddr[5];
      uint8_t icmp_addr[5]=addr_for_icmp;
      for(int i=0;i<5;++i)
          uaddr[i]=id[0]^id[5+i];
      while(uaddr[0]==icmp_addr[0]) uaddr[0]+=1;// First byte must be unique!!!!
  
      // Set card 0 on receive mode with multiple pipe adresses
      if (icmp::card_status_ok){
          com_set_channel(CARD_STATUS,status_chan);
          com_set_state(CARD_STATUS,OFF);
      }
      if (icmp::card_order_ok){
          com_set_channel(CARD_ORDER,orders_chan);
          com_set_tx_addr(CARD_ORDER,uaddr);
          com_set_rx_addr(CARD_ORDER,0,uaddr);
          com_set_pipe_payload(CARD_ORDER,0,order_payload_size);
          com_set_state(CARD_ORDER,RX);
      }
      if (icmp::card_icmp_ok){
          // Set card 2 on rx mode, waiting for robot dhcp request
          com_set_channel(CARD_ICMP,icmp_chan);
          com_set_tx_addr(CARD_ICMP,icmp_addr);
          com_set_rx_addr(CARD_ICMP,0,icmp_addr);
          // use stm32 uid to fix the address:
          com_set_rx_addr(CARD_ICMP,1,uaddr);
          com_set_pipe_payload(CARD_ICMP,0, sizeof(icmp::Order));
          com_set_pipe_payload(CARD_ICMP,1, sizeof(icmp::Order));
          com_set_state(CARD_ICMP,TX);
      }
  
      dhcp_request.icmp_type = icmp::DHCP_REQUEST;
      dhcp_request.arg = icmp::INIT;
      com_get_rx_addr(CARD_ICMP,1,dhcp_request.icmp_addr); // get internal pipe 1 address for reply
  }

  SHELL_COMMAND(state, "Get robot state")
  {
    shell_print("robot state is: ");
    shell_println((int)(com::state));
    shell_print("stats: ");
    shell_print((int)(com::status_send_ok));
    shell_print(" ok, ");
    shell_print((int)(com::status_send_lost));
    shell_println(" lost!");
    shell_print("dhcp failure: ");
    shell_println((int)(com::dhcp_failure));
  }
  
  SHELL_COMMAND(rdiag,"")
  {
    robot_init();
  }

  SHELL_COMMAND(order,"apply order on robot: charge kick dribble x y t")
  {
      if (argc<6){
          shell_println("wrong args");
          return;
      }
      order.id=infos_get_id();
      order.actions=ACTION_ON;
      if (atoi(argv[0])==1)
          order.actions |=ACTION_CHARGE;
      int k=atoi(argv[1]);
      if (k==1)
          order.actions |= ACTION_KICK1;
      if (k==2)
          order.actions |= ACTION_KICK2;
      if (atoi(argv[2])==1)
          order.actions |= ACTION_DRIBBLE;
      order.kickPower=100;
      order.x_speed=atoi(argv[3]);
      order.y_speed=atoi(argv[4]);
      order.t_speed=atoi(argv[5]);
      com::last_order_received = Kernel::get_ms_count()-4;
  //    apply_order(master_packet);
  }

/**
 * @brief INCOMPLETE : only applying command from com for the moment
 */
  void apply_order(packet_master &master_packet)
  {
    last_order_id=max(master_packet.order_id,last_order_id);
    // Driving wheels
    if ((master_packet.actions & ACTION_ON)) {
        if(master_packet.actions & ACTION_TARE_ODOM) {
            // odometry_tare((master_packet.x_speed)/1000.0, (master_packet.y_speed)/1000.0, (master_packet.t_speed)/10000.0);
            //odometry_tare(0.0, 0.0, 0.0);

        }else{
            kinematic::apply_order(master_packet.x_speed/1000.0, master_packet.y_speed/1000.0,
                          master_packet.t_speed/1000.0);
        }
    } else {
        for (uint8_t i = 0; i < 4; i++)
            drivers::set_speed(i, 0.0);
    }
}
//    else if (com_master_frame[0] == MUSIC_PARAMS){
//         struct packet_music *music_params;
//         music_params = (struct packet_music*)(com_master_frame + 1);

//        if(music_params->instrument & SOUND_ON){
//            if(music_params->instrument & BEEPER){
//                buzzer_beep(music_params->note, music_params->duration);
//                buzzer_wait_play();
//            }
//            if(music_params->instrument & KICK){
//                kicker_kick(1,1000);
//            }
//            if(music_params->instrument & CHIP_KICK){
//                kicker_kick(0,1000);
//            }
//            if(music_params->instrument & DRIBBLER){
//                drivers_set(4, true, 1000);
//            }
//            else
//            {
//                drivers_set(4, false, 0);
//            }

//        }

//     }

}