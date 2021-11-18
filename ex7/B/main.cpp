#include <avr/interrupt.h>
#include <util/delay.h>
int temperature = 25, mode = 0, T[4] = {0b1001, 0b0101, 0b0110, 0b1010};
ISR(USART_RXC_vect)
{
    int cycles = UDR;
    while (cycles--)
    {
        for (int i = 0; i < 12; i++)
        {
            PORTB = T[mode];
            mode = (mode + 1) % 4;
            _delay_ms(100);
        }
    }
}
int main()
{
    UCSRB = (1 << RXCIE) | (1 << RXEN) | (1 << TXEN); // Enable RX and TX and interrupt on RX
    UBRRL = 103;                                      // 9600 bps
    ADMUX = (1 << REFS0);                                              // Use AVCC as Reference and select ADC0
    ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // enable ADC, Prescaler 128
    sei();
    DDRB = 0xFF;
    DDRD = 0xFE;
    while (1)
    {
        ADCSRA |= (1 << ADSC);              // Start conversion
        while ((ADCSRA & (1 << ADIF)) == 0) // Wait for the result
            ;
        temperature = ADC;
        UDR = temperature;
        PORTD = 0x40;
        _delay_ms(500);
        PORTD = 0x80;
    }
    return 0;
}