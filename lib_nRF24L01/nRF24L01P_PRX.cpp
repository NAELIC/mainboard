
#include "nRF24L01P_PRX.h"

// status
#define STATUS_UNDEFINED       0
#define STATUS_READY_INIT      1
#define STATUS_POWER_DOWN      2
#define STATUS_STARTUP_STANDBY 3
#define STATUS_STANDBY         4
#define STATUS_RECEIVE         5

nRF24L01P_PRX::nRF24L01P_PRX(nRF24L01P& Device_, PinName CE_, PinName Int_)
  : Device(Device_),
    CE(CE_)
    // ,Int(Int_)
{
   CE = 0;
   PayloadSize = 0;
   Status = STATUS_READY_INIT;
   wait_us(150000);
  //  Int.mode(PullNone);
  //  Int.fall(NULL);
  //  InitializeTimer.attach_us(this, &nRF24L01P_PRX::ReadyInitialize, Tundef2pd_us);
}

void
nRF24L01P_PRX::Initialize()
{
  //  PowerOnTimer.detach();
   CE = 0;
   Device.reset();
   Device.set_prx_mode();
   Status = STATUS_POWER_DOWN;
   PayloadSize = 0;
  //  Int.fall(this, &nRF24L01P_PRX::IntHandler);
}

void
nRF24L01P_PRX::SetChannel(int Channel)
{
   Device.set_channel(Channel);
}

void
nRF24L01P_PRX::SetDataRate(int Rate)
{
   Device.set_air_data_rate(Rate);
}

void
nRF24L01P_PRX::SetAddress(uint64_t Address)
{
   Device.set_rx_address(0, Address);
}

void
nRF24L01P_PRX::SetPayloadSize(int Size)
{
   PayloadSize = Size;
   Device.set_rx_payload_bytes(0, Size);
}

void
nRF24L01P_PRX::PowerUp()
{
   if (Status != STATUS_POWER_DOWN)
   {
      error("nRF24L01P_PRX::PowerUp(): can only be called when device is powered down");
   }
   Status = STATUS_STARTUP_STANDBY;
   Device.set_power_up();
   wait_us(4500);
   Status = STATUS_STANDBY;
}

void
nRF24L01P_PRX::PowerDown()
{
   if (Status == STATUS_UNDEFINED || Status == STATUS_READY_INIT || Status == STATUS_POWER_DOWN)
   {
      error("nRF24L01P_PRX::PowerDown(): error: device is not powered up!");
   }
  //  PowerOnTimer.detach();
   Device.set_power_down();
   Status = STATUS_POWER_DOWN;
}

void
nRF24L01P_PRX::StartReceive()
{
   if (Status == STATUS_UNDEFINED || Status == STATUS_READY_INIT || Status == STATUS_POWER_DOWN)
   {
      error("nRF24L01P_PRX::StartReceive(): error: device is not powered up!");
   }
   CE = 1;
   Status = STATUS_RECEIVE;
}

void
nRF24L01P_PRX::StopReceive()
{
   if (Status != STATUS_RECEIVE)
   {
      error("nRF24L01P_PRX::StopReceive(): error: device is not receiving!");
   }
   CE = 0;
   Status = STATUS_STANDBY;
}
   
bool
nRF24L01P_PRX::IsPacketReady()
{
   return Device.is_rx_ready();
}

int
nRF24L01P_PRX::ReadPacket(char* Buf)
{
   if (!this->IsPacketReady())
   {
      error("nRF24L01P_PRX::ReadPacket(): error: no packet to read!");
   }

   Device.read_rx_payload(Buf);
   Device.clear_rx_ready();
   return PayloadSize;
}

void
nRF24L01P_PRX::IntHandler()
{
// here we do nothing, since we use polling instead
/*
   while (Device.is_rx_ready())
   {
      // read payload
      Device.read_rx_payload(Buf);
      Device.clear_rx_ready();
   }
*/
}

void
nRF24L01P_PRX::ReadyInitialize()
{
   Status = STATUS_READY_INIT;
}

void
nRF24L01P_PRX::ReadyStandby()
{
   if (Status == STATUS_STARTUP_STANDBY)
      Status = STATUS_STANDBY;
}
