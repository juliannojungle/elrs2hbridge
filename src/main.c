#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include "usi_uart.h"

// #define F_CPU 16000000UL // para cristal de 16 MHz
#define F_CPU 20000000UL // para cristal de 20 MHz
#define DEADZONE 100

#define DIR_PIN   PA6
#define PWM_PIN   PA7   // OC0A

void timer0_pwm_init(void);
uint16_t crsf_get_channel(uint8_t *payload, uint8_t ch);

int main(void) {
    DDRA |= (1 << DIR_PIN); // DIR saída
    timer0_pwm_init();
    usi_uart_init();
    sei();

    uint8_t frame[32];
    uint8_t pos = 0;
    uint8_t len = 0;

    while (1) {
        if (usi_uart_available()) {
            uint8_t b = usi_uart_getc();

            if (pos == 0 && b == 0xC8) {
                frame[pos++] = b;
            } else if (pos == 1) {
                len = b;
                frame[pos++] = b;
            } else if (pos > 1) {
                frame[pos++] = b;
                if (pos >= len + 2) {
                    if (frame[2] == 0x16) {
                        uint16_t ch0 = crsf_get_channel(&frame[3], 0);
                        int16_t centered = (int16_t)ch0 - 1024;

                        if (centered > DEADZONE) {
                            PORTA |= (1 << DIR_PIN);
                            OCR0A = (uint8_t)((centered - DEADZONE) * 255 / (1024 - DEADZONE));
                        } else if (centered < -DEADZONE) {
                            PORTA &= ~(1 << DIR_PIN);
                            OCR0A = (uint8_t)((-centered - DEADZONE) * 255 / (1024 - DEADZONE));
                        } else {
                            OCR0A = 0;
                        }
                    }
                    pos = 0;
                }
            }
        }
    }
}

// PWM OC0A
void timer0_pwm_init(void) {
    DDRA |= (1 << PWM_PIN);
    TCCR0A = (1 << COM0A1) | (1 << WGM01) | (1 << WGM00);
    TCCR0B = (1 << CS00);
    OCR0A = 0;
}

// Extrair canal CRSF
uint16_t crsf_get_channel(uint8_t *payload, uint8_t ch) {
    uint16_t bit_ofs = ch * 11;
    uint16_t byte_idx = bit_ofs / 8;
    uint8_t bit_in_byte = bit_ofs % 8;

    uint32_t value = payload[byte_idx] |
                    (payload[byte_idx+1] << 8) |
                    (payload[byte_idx+2] << 16);
    value >>= bit_in_byte;
    return (uint16_t)(value & 0x7FF);
}
