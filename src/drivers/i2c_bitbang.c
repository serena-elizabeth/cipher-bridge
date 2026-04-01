#include "i2c_bitbang.h"
#include <util/delay.h>

static inline void sda_low(void) {
  SDA_DDR |= (uint8_t)(1 << SDA_BIT);
  SDA_PORT &= (uint8_t)~(1 << SDA_BIT);
}
static inline void sda_high(void) { SDA_DDR &= (uint8_t)~(1 << SDA_BIT); }
static inline void scl_low(void) {
  SCL_DDR |= (uint8_t)(1 << SCL_BIT);
  SCL_PORT &= (uint8_t)~(1 << SCL_BIT);
}
static inline void scl_high(void) { SCL_DDR &= (uint8_t)~(1 << SCL_BIT); }
static inline uint8_t sda_read(void) { return (SDA_PIN >> SDA_BIT) & 1; }
static inline void i2c_delay(void) { _delay_us(I2C_DELAY_US); }

void i2c_init(void) {
  sda_high();
  scl_high();
  i2c_delay();
}

void i2c_start(void) {
  sda_high();
  scl_high();
  i2c_delay();
  sda_low();
  i2c_delay();
  scl_low();
  i2c_delay();
}

void i2c_stop(void) {
  sda_low();
  i2c_delay();
  scl_high();
  i2c_delay();
  sda_high();
  i2c_delay();
}

uint8_t i2c_write_byte(uint8_t byte) {
  for (int8_t i = 7; i >= 0; i--) {
    if ((byte >> i) & 1)
      sda_high();
    else
      sda_low();
    i2c_delay();
    scl_high();
    i2c_delay();
    scl_low();
  }
  sda_high();
  i2c_delay();
  scl_high();
  i2c_delay();
  uint8_t ack = !sda_read();
  scl_low();
  i2c_delay();
  return ack;
}

uint8_t i2c_read_byte(uint8_t send_ack) {
  uint8_t byte = 0;
  sda_high();
  for (int8_t i = 7; i >= 0; i--) {
    i2c_delay();
    scl_high();
    i2c_delay();
    if (sda_read())
      byte = (uint8_t)(byte | (1 << i));
    scl_low();
  }
  if (send_ack)
    sda_low();
  else
    sda_high();
  i2c_delay();
  scl_high();
  i2c_delay();
  scl_low();
  sda_high();
  i2c_delay();
  return byte;
}
