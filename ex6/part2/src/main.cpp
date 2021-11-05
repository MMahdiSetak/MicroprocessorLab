#include <stdio.h>
#include <util/delay.h>
#include "LCD.h"

float ctemp, ftemp;
int limit = 35, hyster = 5, mode = 1;
char lcd[16];

ISR(INT0_vect)
{
    if ((PINC & (1 << PC1)) > 0)
        mode ^= 1;
    if (mode && (PINC & (1 << PC0)))
        hyster++;
    else if (mode && (PINC & (1 << PC2)))
        hyster--;
    else if (PINC & (1 << PC0))
        limit++;
    else if (PINC & (1 << PC2))
        limit--;
}

int main()
{
    LCD_Init();

    ADMUX = (1 << REFS0);                                              // Use AVCC as Reference and select ADC0
    ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // enable ADC, Prescaler 128

    DDRD = 1 << PC6;

    GICR |= (1 << INT0);                  // enable INT0 interupt
    MCUCR |= (1 << ISC01) | (1 << ISC00); // enable intrupt request on rising edge of INT0
    sei();

    while (1)
    {
        ADCSRA |= (1 << ADSC);              // Start conversion
        while ((ADCSRA & (1 << ADIF)) == 0) // Wait for the result
            ;
        ctemp = (float)ADC;
        float cFrac = (ctemp - (int)ctemp) * 100;

        LCD_Command(0x01);
        sprintf(lcd, "Limit:%d H:%d", limit, hyster);
        LCD_Command(0x80);
        LCD_String(lcd);
        LCD_Command(0xC0);
        sprintf(lcd, "Temp:%d.%02d C", (int)ctemp, (int)cFrac);
        LCD_String(lcd);

        if (ctemp >= limit)
            PORTD = 1 << PORTD6;
        if (ctemp <= limit - hyster)
            PORTD = 0;

        _delay_ms(500);
    }

    return 0;
}