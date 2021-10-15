#include <avr/io.h>
#include <avr/interrupt.h>

int timer1 = 65535 - 625;
int timer0 = 255 - 1;
int counter = 0;
int mode = 0;

int to7seg(int num)
{
    switch (num)
    {
    case 0:
        return 63;
    case 1:
        return 6;
    case 2:
        return 91;
    case 3:
        return 79;
    case 4:
        return 102;
    case 5:
        return 109;
    case 6:
        return 125;
    case 7:
        return 7;
    case 8:
        return 127;
    case 9:
        return 111;
    }
    return 0;
}

ISR(TIMER1_OVF_vect)
{
    counter++;
    counter %= 10000;
    TCNT1 = timer1;
}

ISR(TIMER0_OVF_vect)
{
    switch (mode)
    {
    case 0:
        PORTD = to7seg(counter % 10);
        PORTC = 7;
        break;
    case 1:
        PORTD = to7seg((counter / 10) % 10);
        PORTC = 11;
        break;
    case 2:
        PORTD = to7seg((counter / 100) % 10);
        PORTC = 13;
        break;
    case 3:
        PORTD = to7seg((counter / 1000) % 10);
        PORTC = 14;
        break;

    default:
        break;
    }
    mode++;
    mode %= 4;
}

int main()
{
    DDRC = 0x0F;
    DDRD = 0x7F;

    TCCR1B |= (1 << CS11); // Set Timer1 prescaler to Clk/8
    TCCR0 |= (1 << CS01);  // Set Timer0 prescaler to Clk/8
    TCNT1 = timer1;
    TCNT0 = timer0;
    TIMSK |= (1 << TOIE1) | (1 << TOIE0); // Enable Timer1 & Timer2 OverFlow Interrupt
    sei();

    PORTC = 0x00;

    while (true)
    {
    }
    return 0;
}
