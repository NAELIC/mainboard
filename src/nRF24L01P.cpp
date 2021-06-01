#include "nRF24L01P.h"

#define NRF24L01P_SPI_MAX_DATA_RATE     10000000

#define NRF24L01P_CMD_R_REGISTER        0x00
#define NRF24L01P_CMD_W_REGISTER        0x20
#define NRF24L01P_CMD_R_RX_PAYLOAD      0x61   
#define NRF24L01P_CMD_W_TX_PAYLOAD      0xA0
#define NRF24L01P_CMD_FLUSH_TX          0xE1
#define NRF24L01P_CMD_FLUSH_RX          0xE2
#define NRF24L01P_CMD_REUSE_TX_PL       0xE3
#define NRF24L01P_CMD_R_RX_PL_WID       0x60
#define NRF24L01P_CMD_W_ACK_PAYLOAD     0xA8
#define NRF24L01P_CMD_W_TX_PYLD_NO_ACK  0xB0
#define NRF24L01P_CMD_NOP               0xFF

// registers
#define NRF24L01P_CONFIG                0x00
#define NRF24L01P_EN_AA                 0x01
#define NRF24L01P_EN_RXADDR             0x02
#define NRF24L01P_SETUP_AW              0x03
#define NRF24L01P_SETUP_RETR            0x04
#define NRF24L01P_RF_CH                 0x05
#define NRF24L01P_RF_SETUP              0x06
#define NRF24L01P_STATUS                0x07
#define NRF24L01P_OBSERVE_TX            0x08
#define NRF24L01P_RPD                   0x09
#define NRF24L01P_RX_ADDR_P0            0x0A
#define NRF24L01P_RX_ADDR_P1            0x0B
#define NRF24L01P_RX_ADDR_P2            0x0C
#define NRF24L01P_RX_ADDR_P3            0x0D
#define NRF24L01P_RX_ADDR_P4            0x0E
#define NRF24L01P_RX_ADDR_P5            0x0F
#define NRF24L01P_TX_ADDR               0x10
#define NRF24L01P_RX_PW_P0              0x11
#define NRF24L01P_RX_PW_P1              0x12
#define NRF24L01P_RX_PW_P2              0x13
#define NRF24L01P_RX_PW_P3              0x14
#define NRF24L01P_RX_PW_P4              0x15
#define NRF24L01P_RX_PW_P5              0x16
#define NRF24L01P_FIFO_STATUS           0x17
#define NRF24L01P_DYNPD                 0x1C
#define NRF24L01P_FEATURE               0x1D

#define NRF24L01P_MAX                   0x1F

nRF24L01P::nRF24L01P(PinName mosi, PinName miso, PinName sck, PinName csn, long SPIFrequency)
 : Spi(mosi, miso, sck), 
   NCS(csn)
{
   Spi.format(8,0);                      // 8-bit, ClockPhase = 0, ClockPolarity = 0
   Spi.frequency(SPIFrequency);
   NCS = 1;
   this->reset();
}

void
nRF24L01P::reset()
{
   this->write_register(NRF24L01P_CONFIG,        0x08); // power down
   this->flush_tx_fifo();
   this->flush_rx_fifo();
   this->write_register(NRF24L01P_EN_AA,         0x3F);
   this->write_register(NRF24L01P_EN_RXADDR,     0x03);
   this->write_register(NRF24L01P_SETUP_AW,      0x03);
   this->write_register(NRF24L01P_SETUP_RETR,    0x03);
   this->write_register(NRF24L01P_RF_CH,         0x02);
   this->write_register(NRF24L01P_RF_SETUP,      0x0F);
   this->write_register(NRF24L01P_STATUS,        0x0E); 
   this->write_register_40(NRF24L01P_RX_ADDR_P0, 0xE7E7E7E7E7ULL);
   this->write_register_40(NRF24L01P_RX_ADDR_P1, 0xC2C2C2C2C2ULL);
   this->write_register(NRF24L01P_RX_ADDR_P2,    0xC3);
   this->write_register(NRF24L01P_RX_ADDR_P3,    0xC4);
   this->write_register(NRF24L01P_RX_ADDR_P4,    0xC4);
   this->write_register(NRF24L01P_RX_ADDR_P5,    0xC5);
   this->write_register_40(NRF24L01P_TX_ADDR,    0xE7E7E7E7E7ULL);    
   this->write_register(NRF24L01P_RX_PW_P0,      0x00);
   this->write_register(NRF24L01P_RX_PW_P1,      0x00);
   this->write_register(NRF24L01P_RX_PW_P2,      0x00);
   this->write_register(NRF24L01P_RX_PW_P3,      0x00);
   this->write_register(NRF24L01P_RX_PW_P4,      0x00);
   this->write_register(NRF24L01P_RX_PW_P5,      0x00);
   this->write_register(NRF24L01P_FIFO_STATUS,   0x00);
   this->write_register(NRF24L01P_DYNPD,         0x00);
   this->write_register(NRF24L01P_FEATURE,       0x00); 
}

void
nRF24L01P::set_spi_frequency(long Freq)
{
   Spi.frequency(Freq);
}

void
nRF24L01P::set_air_data_rate(int Rate)
{
   char r = 0;
   if (Rate == 250)
      r = 0x20;
   else if (Rate == 1000)
      r = 0x00;
   else if (Rate == 2000)
      r = 0x08;
   char c = this->read_register(NRF24L01P_RF_SETUP);
   this->write_register(NRF24L01P_RF_SETUP, (c & 0xD7) | r);
}

void
nRF24L01P::set_channel(int Channel)
{
   char c = Channel;
   c = c & 0x7F;
   this->write_register(NRF24L01P_RF_CH, c);   
}

bool
nRF24L01P::received_power_detector()
{
   return this->read_register(NRF24L01P_RPD) == 0x01;
}

void
nRF24L01P::set_tx_power(int Power)
{
   char p = 0;
   if (Power == 0)
      p = 0x03;
   else if (Power == -6)
      p = 0x02;
   else if (Power == -12)
      p = 0x01;
   else if (Power == -18)
      p = 0x00;
   char c = this->read_register(NRF24L01P_RF_SETUP);
   this->write_register(NRF24L01P_RF_SETUP, (c & 0xFC) | p);     
}

void
nRF24L01P::enable_dynamic_payload(int Pipe, bool Enable)
{
   char Mask = 1 << Pipe;
   char c = this->read_register(NRF24L01P_DYNPD);
   if (Enable)
      c = c | Mask;
   else
      c = c & ~Mask;
   this->write_register(NRF24L01P_DYNPD, c);      
}

void
nRF24L01P::enable_ack_payload(bool Enable)
{
   char c = this->read_register(NRF24L01P_FEATURE);
   this->write_register(NRF24L01P_FEATURE, Enable ? (c | 0x02) : (c & 0xFD));
}

void
nRF24L01P::enable_no_ack(bool Enable)
{
   char c = this->read_register(NRF24L01P_FEATURE);
   this->write_register(NRF24L01P_FEATURE, Enable ? (c | 0x01) : (c & 0xFE));
}

void
nRF24L01P::set_address_width(int Width)
{
   char c = 0;
   if (Width == 3)
      c = 0x01;
   else if (Width == 4)
      c = 0x02;
   else if (Width == 5)
      c = 0x03;
   this->write_register(NRF24L01P_SETUP_AW, c);
}

int
nRF24L01P::get_address_width()
{
   int r = this->read_register(NRF24L01P_SETUP_AW);
   switch (r & 0x03)
   {
      case 0x01 : return 3;
      case 0x02 : return 4;
      case 0x03 : return 5;
   }
   return 0;
}

void
nRF24L01P::set_rx_address(int Pipe, uint64_t Address)
{
   if (Pipe < 0 || Pipe > 1)
      error("RF24L01P::set_rx_address: invalid pipe");
   this->write_register_40(NRF24L01P_RX_ADDR_P0+Pipe, Address);
}

void
nRF24L01P::set_rx_address_low(int Pipe, uint8_t Address)
{
   this->write_register(NRF24L01P_RX_ADDR_P0+Pipe, Address);
}

void
nRF24L01P::set_rx_payload_bytes(int Pipe, int Bytes)
{
   this->write_register(NRF24L01P_RX_PW_P0+Pipe, Bytes & 0x1F);
}

bool
nRF24L01P::is_rx_full()
{
   return (this->read_register(NRF24L01P_FIFO_STATUS) & 0x02) != 0x00;
}

bool
nRF24L01P::is_rx_empty()
{
   return (this->read_register(NRF24L01P_FIFO_STATUS) & 0x01) != 0x00;
}

bool
nRF24L01P::is_rx_ready()
{
   return (this->read_register(NRF24L01P_STATUS) & 0x40) != 0x00;
}

uint8_t
nRF24L01P::debug_read()
{
  return this->read_register(NRF24L01P_EN_AA);
}

void
nRF24L01P::clear_rx_ready()
{
   char c = this->read_register(NRF24L01P_STATUS);
   this->write_register(NRF24L01P_STATUS, c | 0x40);
}

void
nRF24L01P::set_interrupt_rx_ready(bool Enable)
{
   char c = this->read_register(NRF24L01P_CONFIG);
   this->write_register(NRF24L01P_CONFIG, Enable ? (c & 0xBF) : (c | 0x40));
}

bool
nRF24L01P::is_tx_full()
{
   return (this->read_register(NRF24L01P_FIFO_STATUS) & 0x20) != 0x00;
}

bool
nRF24L01P::is_tx_empty()
{
   return (this->read_register(NRF24L01P_FIFO_STATUS) & 0x10) != 0x00;
}

bool
nRF24L01P::is_tx_sent()
{
   return (this->read_register(NRF24L01P_STATUS) & 0x20) != 0x00;
}

void
nRF24L01P::clear_tx_sent()
{
   char c = this->read_register(NRF24L01P_STATUS);
   this->write_register(NRF24L01P_STATUS, c | 0x20);
}

void
nRF24L01P::set_interrupt_tx(bool Enable)
{
   char c = this->read_register(NRF24L01P_CONFIG);
   this->write_register(NRF24L01P_CONFIG, Enable ? (c & 0xDF) : (c | 0x20));
}

void
nRF24L01P::set_retransmit_delay(int Delay)
{
   char c = this->read_register(NRF24L01P_SETUP_RETR);
   this->write_register(NRF24L01P_SETUP_RETR, (c & 0x0f) | ((Delay & 0x0f) << 4));
}

void
nRF24L01P::set_retransmit_attempts(int Attempts)
{
   char c = this->read_register(NRF24L01P_SETUP_RETR);
   this->write_register(NRF24L01P_SETUP_RETR, (c & 0xf0) | (Attempts & 0x0f));
}

bool
nRF24L01P::is_max_rt()
{
   return (this->read_register(NRF24L01P_STATUS) & 0x10) != 0x00;
}

void
nRF24L01P::clear_max_rt()
{
   char c = this->read_register(NRF24L01P_STATUS);
   this->write_register(NRF24L01P_STATUS, c | 0x10);
}

void
nRF24L01P::set_interrupt_max_rt(bool Enable)
{
   char c = this->read_register(NRF24L01P_CONFIG);
   this->write_register(NRF24L01P_CONFIG, Enable ? (c & 0xEF) : (c | 0x10));
}

void
nRF24L01P::set_crc_width(int Width)
{
   char c = this->read_register(NRF24L01P_CONFIG);
   this->write_register(NRF24L01P_CONFIG, Width == 1 ? (c & 0xFB) : (c | 0x04));
}

void
nRF24L01P::enable_crc(bool Enable)
{
   char c = this->read_register(NRF24L01P_CONFIG);
   this->write_register(NRF24L01P_CONFIG, Enable ? (c | 0x08) : (c & 0xF7));
}


void
nRF24L01P::set_power_up()
{
   char c = this->read_register(NRF24L01P_CONFIG);
   this->write_register(NRF24L01P_CONFIG, c | 0x02);
}

void
nRF24L01P::set_power_down()
{
   char c = this->read_register(NRF24L01P_CONFIG);
   this->write_register(NRF24L01P_CONFIG, c & 0xFD);
}

void
nRF24L01P::set_prx_mode()
{
   char c = this->read_register(NRF24L01P_CONFIG);
   this->write_register(NRF24L01P_CONFIG, c | 0x01);
}

void
nRF24L01P::set_ptx_mode()
{
   char c = this->read_register(NRF24L01P_CONFIG);
   this->write_register(NRF24L01P_CONFIG, c & 0xFE);
}


void
nRF24L01P::set_auto_acknowledge(int Pipe, bool Enable)
{
   char c = this->read_register(NRF24L01P_EN_AA);
   char Mask = 1 << Pipe;
   this->write_register(NRF24L01P_EN_AA, Enable ? (c | Mask) : (c & ~Mask));
}

void
nRF24L01P::enable_rx_pipe(int Pipe, bool Enable)
{
   char c = this->read_register(NRF24L01P_EN_RXADDR);
   char Mask = 1 << Pipe;
   this->write_register(NRF24L01P_EN_RXADDR, Enable ? (c | Mask) : (c & ~Mask));
}


int
nRF24L01P::which_rx_pipe()
{
   return (this->read_register(NRF24L01P_STATUS) & 0x0E) >> 1;
}

bool
nRF24L01P::tx_full()
{
   return this->read_register(NRF24L01P_STATUS) & 0x01;
}

int
nRF24L01P::num_lost_packets()
{
   return (this->read_register(NRF24L01P_OBSERVE_TX) & 0xF0) >> 4;
}

int 
nRF24L01P::num_retransmitted_packets()
{
   return this->read_register(NRF24L01P_OBSERVE_TX) & 0x0F;
}

void
nRF24L01P::set_tx_address(uint64_t Address)
{
   this->write_register_40(NRF24L01P_TX_ADDR, Address);
}

// commands

int
nRF24L01P::read_register(int Register)
{
   NCS = 0;
   Spi.write(NRF24L01P_CMD_R_REGISTER | Register);
   int Result = Spi.write(NRF24L01P_CMD_NOP);
   NCS = 1;
   return Result;
}

int
nRF24L01P::read_register_n(int Register, char* Buf, int Bytes)
{
   NCS = 0;
   Spi.write(NRF24L01P_CMD_R_REGISTER | Register);
   for (int i = 0; i < Bytes; ++i)
   {
      *Buf++ = Spi.write(NRF24L01P_CMD_NOP);
   }
   NCS = 1;
   return 0;
}

int
nRF24L01P::write_register(int Register, char x)
{
   NCS = 0;
   Spi.write(NRF24L01P_CMD_W_REGISTER | Register);
   Spi.write(x);
   NCS = 1;
   return 0;
}

int
nRF24L01P::write_register_16(int Register, uint16_t x)
{
   NCS = 0;
   Spi.write(NRF24L01P_CMD_W_REGISTER | Register);
   Spi.write(x & 0xFF);
   Spi.write((x >> 8) & 0xFF);
   NCS = 1;
   return 0;
}

int64_t
nRF24L01P::write_register_40(int Register, uint64_t x)
{
   NCS = 0;
   Spi.write(NRF24L01P_CMD_W_REGISTER | Register);
   Spi.write(x & 0xFF);
   x = x >> 8;
   Spi.write(x & 0xFF);
   x = x >> 8;
   Spi.write(x & 0xFF);
   x = x >> 8;
   Spi.write(x & 0xFF);
   x = x >> 8;
   Spi.write(x & 0xFF);
   NCS = 1;
   return 0;
}

int
nRF24L01P::write_register_bytes(int Register, char const* Buf, int Bytes)
{
   NCS = 0;
   Spi.write(NRF24L01P_CMD_W_REGISTER | Register);
   for (int i = 0; i < Bytes; ++i)
   {
      Spi.write(*Buf++);
   }
   NCS = 1;
   return 0;
}

void
nRF24L01P::write_tx_payload(char const* Buf, int Bytes)
{
   NCS = 0;
   Spi.write(NRF24L01P_CMD_W_TX_PAYLOAD);
   for (int i = 0; i < Bytes; ++i)
   {
      Spi.write(*Buf++);
   }
   NCS = 1;
}

void
nRF24L01P::write_tx_payload_no_ack(char const* Buf, int Bytes)
{
   NCS = 0;
   Spi.write(NRF24L01P_CMD_W_TX_PYLD_NO_ACK);
   for (int i = 0; i < Bytes; ++i)
   {
      Spi.write(*Buf++);
   }
   NCS = 1;
}

void
nRF24L01P::flush_tx_fifo()
{
   NCS = 0;
   Spi.write(NRF24L01P_CMD_FLUSH_TX);
   NCS = 1;
}

void
nRF24L01P::reuse_tx_payload()
{
   NCS = 0;
   Spi.write(NRF24L01P_CMD_REUSE_TX_PL);
   NCS = 1;
}

int
nRF24L01P::rx_payload_width()
{
   NCS = 0;
   Spi.write(NRF24L01P_CMD_R_RX_PL_WID);
   int Result = Spi.write(NRF24L01P_CMD_NOP);
   NCS = 1;
   return Result;
   
}

void
nRF24L01P::read_rx_payload(char* Buf, int Bytes)
{
   NCS = 0;
   Spi.write(NRF24L01P_CMD_R_RX_PAYLOAD);
   for (int i = 0; i < Bytes; ++i)
   {
      *Buf++ = Spi.write(NRF24L01P_CMD_NOP);
   }
   NCS = 1;
}

int
nRF24L01P::read_rx_payload(char* Buf)
{
   this->read_rx_payload(Buf, this->rx_payload_width());
   return 0;
}

void
nRF24L01P::flush_rx_fifo()
{
   NCS = 0;
   Spi.write(NRF24L01P_CMD_FLUSH_RX);
   NCS = 1;
}

void
nRF24L01P::write_ack_payload(int Pipe, char const* Buf, int Bytes)
{
   NCS = 0;
   Spi.write(NRF24L01P_CMD_W_ACK_PAYLOAD);
   for (int i = 0; i < Bytes; ++i)
   {
      Spi.write(*Buf++);
   }
   NCS = 1;
}
