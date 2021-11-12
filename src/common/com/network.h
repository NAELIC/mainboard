#pragma once

#define ACTION_ON (1 << 0)
#define ACTION_KICK1 (1 << 1)
#define ACTION_KICK2 (1 << 2)
#define ACTION_DRIBBLE (1 << 3)
#define ACTION_CHARGE (1 << 5)
#define ACTION_TARE_ODOM (1 << 7)

typedef struct
{
    uint8_t id;
    uint8_t actions;

    float x_speed; // Kinematic orders [mm/s]
    float y_speed;
    float t_speed; // Rotation in [mrad/s]

    uint8_t kickPower; // Kick power (this is a duration in [x25 uS])
} __attribute__((packed)) packet_robot;

#define STATUS_OK (1 << 0)         // The robot is alive and ok
#define STATUS_DRIVER_ERR (1 << 1) // Error with drivers
#define STATUS_IR (1 << 2)         // The infrared barrier detects the ball

// robot -> master
typedef struct
{
    uint8_t id;
    uint8_t status;
    
    uint8_t cap_volt; // Kick capacitor voltage [V]
    uint8_t voltage; // Battery voltage [8th of V]

    int16_t xpos; // Data planned by odometry
    int16_t ypos; // In mm
    int16_t ang;  // In rad/10000

} __attribute__((packed)) packet_status;

#define CARD_STATUS 0
#define CARD_ORDER  1
#define CARD_ICMP   2

// ICMP protocol
namespace icmp 
{
constexpr uint8_t ADDRESSE_WIDTH = 5;
constexpr uint8_t addr_for_icmp[ADDRESSE_WIDTH] = {0xA1,0xA3,0xB6,0xE4,0xB6};

enum Type : uint8_t {
  ICMP_ECHO = 0x01,
  ICMP_DHCP_REQUEST = 0x02,
  ICMP_DHCP_REPLY =  0x03,
  ICMP_NOREPLY =  0x04
};

enum Status : uint8_t {
  ICMP_FULL = 0x01,
  ICMP_OK   = 0x02
};

constexpr uint8_t CHANNEL = 120;

inline bool card_status_ok = false;
inline bool card_order_ok  = false;
inline bool card_icmp_ok   = false;

struct Order{
    Type icmp_type;
    Status arg; // ICMP_FULL or ICMP_OK
    uint8_t icmp_addr[ADDRESSE_WIDTH]; // for DHCP_REQUEST: the address where to send the ICMP reply and orders
                                       // for DHCP_REPLY: the adddress of the pipe for status
    struct Order &operator=(const struct Order &o){
        icmp_type = o.icmp_type;
        arg=o.arg;
        for(int i=0;i<5;++i)
            icmp_addr[i]=o.icmp_addr[i];
        return *this;
    }
};
} // namespace icmp