#define F_CPU 1000000UL /* Define CPU Frequency 1MHz */

#include <avr/io.h>
#include <avr/delay.h>

int main()
{
    DDRA = 0;
    DDRB = 0xFF;

    bool dir = 1;
    while (true)
    {
        if (dir)
            PORTB <<= 1;
        else
            PORTB >>= 1;
        if (PORTB == 0)
            dir = 1;
        if (PORTB & (1 << PORTB7))
            dir = 0;
        if (PINA & 1)
            PORTB |= 1;
        _delay_ms(500);
    }
    return 0;
}
