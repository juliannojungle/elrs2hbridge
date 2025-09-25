#include "usi_uart.h"

#define RX_PIN   PB2   // Entrada UART (ligar ao TX do EP2)

static volatile uint8_t rx_buffer[RX_BUFFER_SIZE];
static volatile uint8_t rx_head = 0;
static volatile uint8_t rx_tail = 0;

static volatile uint8_t rx_data;
static volatile uint8_t rx_bitcount;
static volatile uint8_t receiving = 0;

// Cada bit dura 1/420000 s = ~2.38 us
// A 16 MHz, cada ciclo = 62.5 ns → 38 ciclos por bit
#define BIT_TICKS 38

void usi_uart_init(void) {
    DDRB &= ~(1 << RX_PIN);    // RX como entrada
    PORTB |= (1 << RX_PIN);    // Pull-up

    // Habilita interrupção de pin change no RX
    GIMSK |= (1 << PCIE0);
    PCMSK0 |= (1 << PCINT0);

    // Timer1 modo CTC
    TCCR1A = 0;
    TCCR1B = (1 << WGM12) | (1 << CS10); // no prescale
}

// Interrupção: start bit detectado (borda de descida)
ISR(PCINT0_vect) {
    if (!(PINB & (1 << RX_PIN)) && !receiving) {
        receiving = 1;
        rx_bitcount = 0;
        rx_data = 0;

        // Ajusta timer para meio bit (amostra no meio)
        OCR1A = BIT_TICKS / 2;
        TCNT1 = 0;
        TIFR1 |= (1 << OCF1A);
        TIMSK1 |= (1 << OCIE1A);
    }
}

// Timer1: amostra bits
ISR(TIM1_COMPA_vect) {
    if (!receiving) {
        TIMSK1 &= ~(1 << OCIE1A);
        return;
    }

    if (rx_bitcount < 8) {
        rx_data >>= 1;
        if (PINB & (1 << RX_PIN))
            rx_data |= 0x80;
        rx_bitcount++;
    } else if (rx_bitcount == 8) {
        // Stop bit
        if (PINB & (1 << RX_PIN)) {
            rx_buffer[rx_head % RX_BUFFER_SIZE] = rx_data;
            rx_head++;
        }
        receiving = 0;
        TIMSK1 &= ~(1 << OCIE1A);
    }

    OCR1A = BIT_TICKS;
    TCNT1 = 0;
}

uint8_t usi_uart_available(void) {
    return (uint8_t)(rx_head - rx_tail);
}

uint8_t usi_uart_getc(void) {
    while (rx_head == rx_tail);
    uint8_t data = rx_buffer[rx_tail % RX_BUFFER_SIZE];
    rx_tail++;
    return data;
}
