#include "i2c_bitbang.h"
#include <util/delay.h>

uint8_t eeprom_write_byte(uint16_t addr, uint8_t data) {
    i2c_start();
    if (!i2c_write_byte((EEPROM_ADDR << 1) | 0)) goto fail;
    if (!i2c_write_byte((uint8_t)(addr >> 8)))    goto fail;
    if (!i2c_write_byte((uint8_t)(addr & 0xFF)))  goto fail;
    if (!i2c_write_byte(data))                    goto fail;
    i2c_stop();

    /* ACK polling — wait for internal write cycle (~5 ms) */
    for (uint8_t i = 0; i < 100; i++) {
        _delay_ms(1);
        i2c_start();
        if (i2c_write_byte((EEPROM_ADDR << 1) | 0)) {
            i2c_stop();
            return 1;
        }
        i2c_stop();
    }
    return 0;

fail:
    i2c_stop();
    return 0;
}

uint8_t eeprom_read_byte(uint16_t addr) {
    /* Dummy write to set address pointer */
    i2c_start();
    if (!i2c_write_byte((EEPROM_ADDR << 1) | 0)) goto fail;
    if (!i2c_write_byte((uint8_t)(addr >> 8)))    goto fail;
    if (!i2c_write_byte((uint8_t)(addr & 0xFF)))  goto fail;

    /* Repeated START, switch to read */
    i2c_start();
    if (!i2c_write_byte((EEPROM_ADDR << 1) | 1)) goto fail;
    uint8_t val = i2c_read_byte(0);
    i2c_stop();
    return val;

fail:
    i2c_stop();
    return 0xFF;
}
