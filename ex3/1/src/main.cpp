#include <avr/io.h>
#include <avr/interrupt.h>

int timer = 65535 - 62500;
int counter = 0;

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
    PORTD = to7seg(counter % 10);
    counter++;
    TCNT1 = timer;
}

int main()
{
    DDRC = 0x01;
    DDRD = 0x7F;

    TCCR1B |= (1 << CS11); // Set prescaler to Clk/8
    TCNT1 = timer;
    TIMSK |= (1 << TOIE1); // Enable Timer1 OverFlow Interrupt
    sei();

    PORTC = 0x00;

    while (true)
    {
    }
    return 0;
}
