#include "drivers/i2c_bitbang.h"
#include "drivers/uart.h"
#include <stdint.h>
#include <util/delay.h>

#define PORTF (*((volatile uint8_t *)0x31))
#define DDRF (*((volatile uint8_t *)0x30))
void write_the_key(char k);
void do_you_need_key(void);
char read_the_key(void);

void do_you_need_key(void) {
  volatile char choice = '0', k;
  uart0.tx_str(&uart0, "Do you need to change the Key (Y / n) ?: ");

  while (1) {
    choice = uart0.rx_char(&uart0);
    if ((choice == 'Y') || (choice == 'y') || (choice == 'N') ||
        (choice == 'n')) {
      break; // got a valid char — exit loop
    }
    // anything else (\r \n spaces) — keep looping, discard it
  }

  switch (choice) {
  case 'Y':
  case 'y':
    uart0.tx_str(&uart0, "\n\r Enter the key: ");
    do {
      k = uart0.rx_char(&uart0);
    } while (k == '\r' || k == '\n');
    write_the_key(k);
    break;
  default:
    break;
  }
}

void write_the_key(char k) {
  i2c_init();
  _delay_ms(50);
  eeprom_write_byte(0x0000, (uint8_t)k);
}

char read_the_key(void) {
  i2c_init();
  _delay_ms(50);
  return (char)eeprom_read_byte(0x0000);
}

int main(void) {
  uart0.init(&uart0, 9600); /* Built-in USB (input)       */
  uart1.init(&uart1, 9600); /* External USB on TX1/RX1    */
  uart2.init(&uart2, 9600); /* Bridge to 8051             */

  uart0.tx_str(&uart0, " Command Input Ready\r\n ");

  volatile char count = 0;
  volatile char buffer;
  volatile char mode;
  volatile char val;

  uart0.tx_str(&uart0, "MODE (E for encryption and D for Decryption): ");
  mode = uart0.rx_char(&uart0);
  uart0.tx_str(&uart0, "\r\n");
  DDRF = 0xff;
  DDRB = 0x03;

  switch (mode) {
  case 'E':
  case 'e':
    PORTF = 0x01;
    do_you_need_key();
    break;
  case 'D':
  case 'd':
    PORTF = 0x00;
    break;
  default:
    uart0.tx_str(&uart0, "Then we are going to use the present key \n\r");
    break;
  }

  val = read_the_key();
  uart0.tx_str(&uart0, "\r\n your key == ");
  uart0.tx_char(&uart0, val);
  uart0.tx_str(&uart0, "\n\r");

  uart2.tx_char(&uart2, val);
  uart0.tx_str(&uart0, "we passed the key to 8051 \n\r");

  while (1) {
    /* S0 — Read from built-in USB via UART0 */
    buffer = uart0.rx_char(&uart0);

    /* S1 — Echo command received on UART0 */
    // uart0.tx_str(&uart0, "CMD: ");
    uart0.tx_char(&uart0, buffer);
    // uart0.tx_str(&uart0, "\r\n");

    /* S2 — Send byte to 8051 via UART2 */
    uart2.tx_char(&uart2, buffer);

    /* S3 — Receive byte from 8051 via UART2 */
    buffer = uart2.rx_char(&uart2);

    /* S4 — Skip first byte, then forward to UART1 */
    if (count == 0) {
      count = 1;
      goto skip;
    }
    uart1.tx_char(&uart1, buffer);

  skip:;
  }

  return 0;
}
