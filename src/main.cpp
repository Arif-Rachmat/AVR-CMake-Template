#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include <stdlib.h>
#include <stddef.h>

#include <util/delay.h>


int main(void) {

    DDRB |= (1 << PB5); // Set PB5 (Arduino pin 13) as output for the onboard LED.

    while (1) {
        PORTB ^= (1 << PB5); // Toggle the LED state.
        _delay_ms(1000);     // Wait for 1 second.
    }
}
