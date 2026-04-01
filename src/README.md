# Cipher Bridge

UART-linked encryption system using AT89S52 (8051) and ATmega2560.

## Overview

A secure data communication system built across two microcontroller platforms.
The ATmega2560 acts as the communication gateway. The AT89S52 performs
Caesar cipher encryption and decryption on all printable ASCII characters.
The encryption key is stored in an AT24C32 EEPROM over I2C.
System status is displayed via a 74HC595 shift register driving an 8-LED array.

## Hardware

| Component      | Role                        |
|----------------|-----------------------------|
| ATmega2560     | Communication gateway       |
| AT89S52        | Cryptographic unit          |
| AT24C32 EEPROM | Non-volatile key storage    |
| 74HC595        | SPI shift register for LEDs |

## Protocols Used

- UART — host communication and inter-MCU link
- I2C (bit-bang) — EEPROM key storage
- SPI (bit-bang) — shift register LED control
- GPIO — mode signal (encrypt/decrypt)

## Contributions

| Module | Author |
|--------|--------|
| ATmega2560 main firmware | Sangeeth Binu |
| UART driver | Sangeeth Binu |
| I2C bit-bang primitives | Sangeeth Binu |
| EEPROM helper driver | Serena Elizabeth Jacob |
| AT89S52 cipher firmware | Serena Elizabeth Jacob |
| Caesar cipher engine + SPI driver | Serena Elizabeth Jacob |

## Project Structure

    cipher_bridge/
    ├── src/
    │   ├── main.c
    │   ├── 8051.c
    │   └── drivers/
    │       ├── uart.c
    │       ├── uart.h
    │       ├── i2c_bitbang.c
    │       ├── i2c_bitbang.h
    │       └── i2c_eeprom.c
    ├── tools/
    │   ├── monitor.py
    │   └── input.txt
    ├── docs/
    ├── Makefile
    └── README.md

## Build (ATmega2560)

    make

Requires avr-gcc and AVRDUDE.

## Flash (AT89S52)

Open src/8051.c in Keil uVision, compile, and flash via ProgISP.

## Authors

- [Serena Elizabeth Jacob](https://github.com/serena-elizabeth)
- [Sangeeth Binu](https://github.com/Sangeeth-binu)
