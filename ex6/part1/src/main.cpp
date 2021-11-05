#include <stdio.h>
#include <util/delay.h>
#include "LCD.h"

float ctemp, ftemp;
char lcd[16];

int main()
{
    LCD_Init();
    ADMUX = (1 << REFS0);                                              // Use AVCC as Reference and select ADC0
    ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // enable ADC, Prescaler 128

    while (1)
    {
        ADCSRA |= (1 << ADSC);
        while ((ADCSRA & (1 << ADIF)) == 0)
            ;
        ctemp = (float)ADC;
        float cFrac = (ctemp - (int)ctemp) * 10;
        ftemp = (9.5 * ctemp) + 32;
        float fFrac = (ftemp - (int)ftemp) * 10;

        LCD_Command(0x01);
        sprintf(lcd, "Temp:%d.%01d C", (int)ctemp, (int)cFrac);
        LCD_Command(0x80);
        LCD_String(lcd);
        LCD_Command(0xC0);
        sprintf(lcd, "%d.%01d F", (int)ftemp, (int)fFrac);
        LCD_String(lcd);
        _delay_ms(500);
    }

    return 0;
}