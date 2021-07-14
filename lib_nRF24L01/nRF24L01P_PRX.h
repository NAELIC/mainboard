// A simple PRX driver for the nRF24L01+
// for polling mode with fixed payload length

#ifndef NRF24L01P_PRX_H
#define NRF24L01P_PRX_H

#include "nRF24L01P.h"

class nRF24L01P_PRX
{
   public:
      nRF24L01P_PRX(nRF24L01P& Device_, PinName CE_, PinName Int_);

      // Initialize the device for transmitting.  There must be a delay of 
      // at least 100ms from initial power on before this function is called.
      void Initialize();

      // Set the channel number, 0 .. 125
      void SetChannel(int Channel);

      // sets the data rate in Kbps, valid values are 250 (nRF24L01+ only), 1000, 2000
      void SetDataRate(int Rate);
      
      // Set the 40-bit address
      void SetAddress(uint64_t Address);

      // Set the expected payload size
      void SetPayloadSize(int Size);

      // Power up ready to enter receive mode.  There is a delay of Tpd2stby_us
      // after PowerUp() before receiving can start.
      void PowerUp();
      
      // Powers down the device.  PowerUp must be called before a packet can be transmitted.
      void PowerDown();

      // Start listening for packets.
      void StartReceive();
      
      // Stop listening for packets.
      void StopReceive();

      // returns true if there is a packet ready to read
      bool IsPacketReady();

      // PRECONDITION: IsPacketReady().
      // Reads the packet into the given buffer.
      // Returns the size of the packet.
      int ReadPacket(char* Buf);
      int ReadPacket(uint8_t* Buf);
   private:
      void IntHandler();
      void ReadyInitialize();
      void ReadyStandby();
   
      nRF24L01P& Device;
      DigitalOut CE;
      // InterruptIn Int;
      // Timeout PowerOnTimer;
      // Timeout InitializeTimer;
      
      int volatile Status;
      int PayloadSize;
};

#endif
