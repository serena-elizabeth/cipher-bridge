#ifndef I2C_BITBANG_H
#define I2C_BITBANG_H

#include <stdint.h>

/* ── Pin mapping ────────────────────────────────────────────────────────── */
#define PORTB (*((volatile uint8_t *)0x25))
#define DDRB (*((volatile uint8_t *)0x24))
#define PINB (*((volatile uint8_t *)0x23))

#define SDA_DDR DDRB
#define SDA_PORT PORTB
#define SDA_PIN PINB
#define SDA_BIT 5 /* PB5 = Arduino Mega pin 11 */

#define SCL_DDR DDRB
#define SCL_PORT PORTB
#define SCL_PIN PINB
#define SCL_BIT 6 /* PB6 = Arduino Mega pin 12 */

/* ── Bus config ─────────────────────────────────────────────────────────── */
#define I2C_DELAY_US 5 /* 5 us → ~100 kHz */

/* ── EEPROM ─────────────────────────────────────────────────────────────── */
#define EEPROM_ADDR 0x50 /* A2=A1=A0=GND */

/* ── I2C primitives ─────────────────────────────────────────────────────── */
void i2c_init(void);
void i2c_start(void);
void i2c_stop(void);
uint8_t i2c_write_byte(uint8_t byte);
uint8_t i2c_read_byte(uint8_t send_ack);

/* ── EEPROM helpers ─────────────────────────────────────────────────────── */
uint8_t eeprom_write_byte(uint16_t addr, uint8_t data);
uint8_t eeprom_read_byte(uint16_t addr);

/* ── Application-level key helpers ─────────────────────────────────────── */
// void do_you_need_key(void);
// void write_the_key(char k);
// char read_the_key(void);

#endif /* I2C_BITBANG_H */
