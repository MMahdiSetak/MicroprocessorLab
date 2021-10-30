#include <avr/io.h>
#include <avr/interrupt.h>

long timer1 = 0, accelerate = 2000, max_speed = 63000;
int mode = 0, on = 0, dir = 1, T[4] = {0b1001, 0b0101, 0b0110, 0b1010};

ISR(TIMER1_OVF_vect)
{
    if (on)
    {
        timer1 += accelerate;
        TCNT1 = timer1;
        if (timer1 >= max_speed)
            accelerate = -accelerate;
        if (timer1 < accelerate)
        {
            accelerate = -accelerate;
            timer1 = 0;
            dir = -dir;
        }
        PORTA = T[mode];
        mode = (mode + dir + 4) % 4;
    }
}

ISR(INT0_vect)
{
    on ^= 1;
}

int main()
{
    DDRA = 0x0F;
    TCCR1B |= (1 << CS11); // Set Timer1 prescaler to Clk/8
    TIMSK |= (1 << TOIE1); // Enable Timer1 OverFlow Interrupt
    GICR |= (1 << INT0);   // enable INT0 interupt
    MCUCR |= (1 << ISC01); // enable intrupt request on falling edge of INT0
    sei();
    TCNT1 = timer1;
    while (true);
    return 0;
}
