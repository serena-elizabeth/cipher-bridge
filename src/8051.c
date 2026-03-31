#include <reg51.h>
char KEY;
#define uint8_t   unsigned char
#define uint16_t  unsigned int
#define int8_t    signed char

// =====================================================
// 74HC595 PIN MAPPING (8051)
// =====================================================
sbit MOSI    = P1^0;
sbit SCK     = P1^1;
sbit LATCH   = P1^2;

// =====================================================
// MODE PIN
// FIX: Use sbit and compare with == 1 or == 0
//      Do NOT use ~PIN — that is bitwise NOT on a byte,
//      not a pin inversion, and always evaluates to true.
// Wire: Arduino PF0 ? 8051 P3.2
//       HIGH (1) = Encrypt
//       LOW  (0) = Decrypt
// =====================================================
sbit ENC_PIN = P3^2;

// =====================================================
// DEBUG BIT DEFINITIONS
// =====================================================
#define DBG_UART_RX     0x01
#define DBG_DECRYPT     0x02
#define DBG_ENCRYPT     0x04
#define DBG_EEPROM_RD   0x08
#define DBG_EEPROM_WR   0x10
#define DBG_KEY_LOADED  0x20
#define DBG_HEARTBEAT   0x40
#define DBG_ERROR       0x80

volatile unsigned char debug_flags = 0;

// =====================================================
// DELAY
// =====================================================
void delay_us(unsigned int us)
{
    unsigned int i;
    for (i = 0; i < us; i++) { }
}

// =====================================================
// SHIFT REGISTER DRIVER
// =====================================================
void shiftreg_init(void)
{
    MOSI  = 0;
    SCK   = 0;
    LATCH = 0;
}

void shift_out(unsigned char byte_val)
{
    unsigned char i;
    unsigned char mask;
    for (i = 0; i < 8; i++) {
        mask = (unsigned char)(0x80u >> i);
        if (byte_val & mask)
            MOSI = 1;
        else
            MOSI = 0;
        SCK = 1;
        delay_us(1);
        SCK = 0;
    }
}

void shiftreg_write(unsigned char value)
{
    LATCH = 0;
    shift_out(value);
    LATCH = 1;
    LATCH = 0;
}

// =====================================================
// DEBUG SYSTEM
// =====================================================
void debug_update(void) { shiftreg_write(debug_flags); }

void debug_set(unsigned char flag)
{
    debug_flags |= flag;
    debug_update();
}

void debug_clear(unsigned char flag)
{
    debug_flags &= ~flag;
    debug_update();
}

// =====================================================
// UART SEND
// =====================================================
void UART_SendChar(char ch)
{
    SBUF = ch;
    while(TI == 0);
    TI = 0;
}

// =====================================================
// ENCRYPTION
// =====================================================
char encrypt(char ch)
{
    debug_set(DBG_ENCRYPT);
    if(ch >= 32 && ch <= 126)
        return ((ch - 32 + KEY) % 95) + 32;
    else
        return ch;
}

// =====================================================
// DECRYPTION
// =====================================================
char decrypt(char ch)
{
    debug_set(DBG_DECRYPT);
    if(ch >= 32 && ch <= 126)
        return ((ch - 32 - KEY + 95) % 95) + 32;
    else
        return ch;
}

// =====================================================
// MAIN
// =====================================================
void main(void)
{
    char received;
    char encrypted;
    char decrypted;

    TMOD = 0x20;      // Timer1 Mode2
    TH1  = 0xFD;      // 9600 baud @ 11.0592 MHz
    SCON = 0x50;      // 8-bit UART, REN enabled
    TR1  = 1;         // Start Timer1

    shiftreg_init();

    // Set P3.2 as input so Arduino can drive it high or low
    ENC_PIN = 1;

    RI = 0;

    // Wait for key byte sent by Arduino before the data loop
    while(RI == 0);
    KEY = SBUF;
    RI = 0;

    // Data loop
    while(1)
    {
        while(RI == 0);         // Wait for incoming byte
        received = SBUF;
        RI = 0;

        if(ENC_PIN == 0) {      // P3.2 HIGH ? Encrypt
            encrypted = encrypt(received);
            UART_SendChar(encrypted);
            debug_clear(DBG_ENCRYPT);
        } else {                // P3.2 LOW  ? Decrypt
            decrypted = decrypt(received);
            UART_SendChar(decrypted);
            debug_clear(DBG_DECRYPT);
        }
    }
}