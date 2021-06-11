// A simple PTX driver for the nRF24L01+
// with fixed payload length

#ifndef NRF24L01P_PTX_H
#define NRF24L01P_PTX_H

#include "nRF24L01P.h"

class nRF24L01P_PTX
{
   public:
      nRF24L01P_PTX(nRF24L01P& Device_, PinName CE_, PinName Int_);

      // Initialize the device for transmitting.  There must be a delay of 
      // at least 100ms from initial power on before this function is called.
      void Initialize();

      // Set the channel number, 0 .. 125
      void SetChannel(int Channel);

      // sets the data rate in Kbps, valid values are 250 (nRF24L01+ only), 1000, 2000
      void SetDataRate(int Rate);
      
      // Set the transmitter power in dB.  Valid values are -18, -12, -6, 0
      void SetTransmitPower(int Power);
      
      // Set the 40-bit destination address
      void SetDestinationAddress(uint64_t Address);

      // Power up ready to transmit.  There is a delay of Tpd2stby_us
      // after PowerUp() before a packet can be transmitted.  This
      // is handled automatically with a timer, so that TransmitPacket()
      // will block until the device is ready.
      void PowerUp();
      
      // Powers down the device.  PowerUp must be called before a packet can be transmitted.
      void PowerDown();

      // Returns true if the device is ready to transmit a packet, ie
      // powered on and not busy
      bool IsReadyTransmit();

      // Does the actual transmit of a packet, in blocking mode.
      // Returns 0 on success.
      // Returns -1 if the packet wasn't successfully acknowledged.
      int TransmitPacket(char* Buf, int Size);

      // Transmits a packet in non-blocking mode.
      // returns 0 if the packet was successful, -1 on some error (eg
      // if the device isn't ready to transmit).
      int TransmitPacketNonBlocking(char* Buf, int Size);

      // For nonblocking mode, returns false if there is still a pending
      // packet waiting to send.  Returns true if the packet has been
      // sent or there was some error (eg max retries was hit).
      bool IsTransmitFinished();
      
      // For nonblocking mode, complete the transmission process.  If
      // IsTransmitFinished() is false, then this call is blocking.
      // returns 0 if the packet was sent successfully, -1 if it wasn't.
      int TransmitComplete();

      DigitalOut CE;
   private:
      void IntHandler();
      void ReadyInitialize();
      void ReadyStandby();
   
      nRF24L01P& Device;
      // InterruptIn Int;
      // Timeout PowerOnTimer;
      // Timeout InitializeTimer;
      
      int volatile Status;
};

#endif
