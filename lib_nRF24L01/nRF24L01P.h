//
// Interface for Nordic Semiconductor nRF24L01 and nRF24L01+
// see http://www.nordicsemi.no/files/Product/data_sheet/nRF24L01P_Product_Specification_1_0.pdf
//
// This class exposes basic SPI functionality for the nRF24L01+
// A driver program is required to actually send or receive data.
// This class doesn't provide any CE line control
//
// Differences between nRF24L01 and nRF24L01+
// 1. The nRF24L01 doesn't support 250kHz air data rate
// 2. The nRF24L01 received_power_detector() works differently
//

#ifndef NRF24L01P_H
#define NRF24L01P_H

#include "mbed.h"
#include <stdint.h>

#define NRF24L01P_MIN_RF_FREQUENCY    2400
#define NRF24L01P_MAX_RF_FREQUENCY    2525

// timing constants
#define Tundef2pd_us     100000   // 100mS, time from reset to power-down mode
#define Tstby2a_us          130   // 130uS, time from standby mode to TX/RX mode
#define Thce_us              10   //  10uS, minimum time for CE high
#define Tpd2stby_us        4500   // 4.5mS worst case, time from power-down to standby
#define Tpece2csn_us          4   //   4uS, delay from CE positive edge to CSN low

class nRF24L01P
{
   public:
      nRF24L01P(PinName mosi, PinName miso, PinName sck, PinName csn, long SPIFrequency = 2000000);

      // Clears the TX and RX FIFOs and resets all registers to their power-on values
      void reset();
      
      // change the SPI frequency
      void set_spi_frequency(long Freq);

      // register operations

      // set the data rate in kb/s, valid values are 250 (nRF24L01+ only), 1000, 2000.
      // DEFAULT:
      void set_air_data_rate(int Rate);
      
      // set the RF channel, 0 .. 125.
      // The frequency in MHz is 2400 + Channel.
      // In 2Mbps mode, you must use channels at least 2MHz appart to avoid overlap
      // DEFAULT: channel 2
      void set_channel(int Channel);
      
      // PRECONDITION: Mode = Receive.  True if a signal higher than -64bB is detected by the receiver
      bool received_power_detector();

      // set the transmit power in dB.  Valid values are 0, -6, -12, -18
      // DEFAULT: 0dB
      void set_tx_power(int Power);

 
      // enable or disable dynamic payload size on the given pipe
      // DEFAULT: false
      void enable_dynamic_payload(int Pipe, bool Enable);

      // Enable including a payload with ACK
      // DEFAULT: false
      void enable_ack_payload(bool Enable);

      // enable the write_tx_payload_no_ack() command
      // DEFAULT: disabled
      void enable_no_ack(bool Enable);

      //
      // receive functionality
      //
      
      // set the width in bytes for addresses.  Valid values are 3,4,5
      // DEFAULT: 5
      void set_address_width(int Width);
      
      // returns the width in bytes for addresses.  Valid values are 3,4,5
      int get_address_width();

      // set the receive address of the given pipe.  Pipes 0,1 have 3,4 or 5 byte addresses
      // as specified by set_address_width().  Pipes 2,3,4,5 can only set the low order byte of the
      // address, the other bytes are shared with pipe 1.
      
      // Set the rx address for pipe 0 or 1
      void set_rx_address(int Pipe, uint64_t Address);
      
      // Set the low byte of the address for pipe 2,3,4,5
      void set_rx_address_low(int Pipe, uint8_t Address);
      
      // Set the payload size for the receive buffer for the given pipe
      // (not used if dynamic payload size is used)
      void set_rx_payload_bytes(int Pipe, int Bytes);

      // returns true if the receive FIFO is full
      bool is_rx_full();

      // returns true if the receive FIFO is empty
      bool is_rx_empty();

      // receive packet status

      uint8_t debug_read();

      // read the status register and return true if there is a packet ready to be received (RX_DR)
      bool is_rx_ready();

      // clear the RX_DR bit
      void clear_rx_ready();
      
      // if Enable, reflect RX_DR as active low on the INT line.  The procedure to read a packet is:
      // 1) Read payload through SPI, 2) clear_rc_ready(), read is_rx_ready() to see if there is another 
      // packet waiting
      // DEFAULT: enabled
      void set_interrupt_rx_ready(bool Enable);

      // returns true if the transmit FIFO is full
      bool is_tx_full();

      // returns true if the transmit FIFO is empty
      bool is_tx_empty();

      // transmit packet status

      // reads the status register and returns true if a packet has been transmitted. (TX_DS)  If auto-acknowledge
      // is set, then this is only asserted when an ack is received.
      bool is_tx_sent();
      
      // Clear the TX_DS bit
      void clear_tx_sent();

      // if Enable, reflect TX_DS as active low on the INT line
      // DEFAULT: enabled
      void set_interrupt_tx(bool Enable);

      // retransmit behaviour
      
      // set the auto-retransmit delay, in units of 250 microseconds.  
      // Valid input is 0,1,...,15 (for 250 .. 4000 us)
      // DEFAULT: 0 (250us)
      void set_retransmit_delay(int Delay);

      // set the number of auto-retransmit attempts, 0 .. 15
      // DEFAULT: 3
      void set_retransmit_attempts(int Attempts);

      // returns true if we've hit the maximum number of restransmits
      bool is_max_rt();

      // clear the maximum number of retransmits flag.
      void clear_max_rt();

      // if Enable, reflect MAX_RT as active low on the INT line
      // DEFAULT: enabled
      void set_interrupt_max_rt(bool Enable);

      // enable or disable the CRC check.  Auto-acknowledge forces CRC enabled.
      // DEFAULT: true
      void enable_crc(bool Enable);      
      
      // set the CRC width, either 1 byte or 2 bytes
      // DEFAULT: 1 byte
      void set_crc_width(int width);

      // set PWR_UP (move into standby mode)
      void set_power_up();

      // turn off power (PWR_UP=0)
      void set_power_down();
      
      // set as primary receiver (disables transmit mode)
      void set_prx_mode();

      // set as primary transmitter (disables receive mode)
      void set_ptx_mode();

      // Enable or disable receiving on the given pipe
      // DEFAULT: Pipes 0,1 enabled, pipes 2,3,4,5 disabled.
      void enable_rx_pipe(int Pipe, bool Enable);

      // Enable or disable auto-acknowledge on the given pipe 0,1,2,3,4,5
      // DEFAULT: Enabled on all pipes
      void set_auto_acknowledge(int Pipe, bool Enable);
      
      // returns the pipe number for the payload available on the rx pipe, or -1 if the rx fifo is empty
      int which_rx_pipe();

      // returns true if the transmit fifo is full
      bool tx_full();   
      
      // returns the number of transmitted but lost packets.  Capped at 15, reset by writing the channel number.
      int num_lost_packets();
      
      // returns the number of times the current packet has been retransmitted.  Reset when a new packet starts
      int num_retransmitted_packets();

       
      // Set the tx address.  Used only for a PTX, set this the same as the rx_address of pipe 0 to enable 
      // auto-acknowledge with Enhanced ShockBurst(TM)
      // DEFAULT: 0xE7E7E7E7E
      void set_tx_address(uint64_t Address);
      
      // commands
      
      // read a byte from the given register
      int read_register(int Register);

      // read n bytes from the given register
      int read_register_n(int Register, char* Buf, int Bytes);

      // write one byte to the given register
      int write_register(int Register, char x);

      // write two bytes to a register
      int write_register_16(int Register, uint16_t x);

      // write five bytes to a register
      int64_t write_register_40(int Register, uint64_t x);

      // write some bytes to the given register
      int write_register_bytes(int Register, char const* Buf, int Bytes);

      // Write a transmit payload, 1-32 bytes
      void write_tx_payload(char const* Buf, int Bytes);

      // Write a transmit payload, 1-32 bytes, with ACK disabled.
      // NOTE: enable_no_ack() must be enabled
      void write_tx_payload_no_ack(char const* Buf, int Bytes);
      void write_tx_payload_no_ack(uint8_t const* Buf, int Bytes);

      // flush the transmit FIFO
      void flush_tx_fifo();

      // reuse the last transmitted payload.  Payload reuse is activated until the next flush_tx_fifo() or write_tx_payload()
      void reuse_tx_payload();

      // returns the width of the receive payload.  (If this isn't in the range 0..32 then the rx fifo should be flushed)
      int rx_payload_width();

      // read 1-32 bytes from the RX FIFO.  The payload is deleted after it is read.  The user must
      // know how many bytes to read.
      void read_rx_payload(char* Buf, int Bytes);
      void read_rx_payload(uint8_t* Buf, int Bytes);
      
      // this version reads the complete payload into Buf, and returns the number of bytes in the payload
      int read_rx_payload(char* Buf);
      int read_rx_payload(uint8_t* Buf);

      // flush the receive FIFO
      void flush_rx_fifo();
      
      // set the ACK payload for the given pipe.  
      void write_ack_payload(int Pipe, char const* Buf, int Bytes);


   private:
      SPI         Spi;
      DigitalOut  NCS;
};

#endif

