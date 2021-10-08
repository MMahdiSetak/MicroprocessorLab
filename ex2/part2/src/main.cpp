#define F_CPU 1000000UL /* Define CPU Frequency 1MHz */

#include <avr/io.h>
#include <avr/delay.h>

int main()
{
    DDRB = 0xFF;

    while (true)
    {
        PORTB ^= 0xFF;
        _delay_ms(500); // eash cycle takes 1/1000000 seconds so we need 500000 cycles to get 0.5 second delay (500000 us or 500 ms)
    }
    return 0;
}
