#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>

#define LCD_Data_Dir DDRD      /* Define LCD data port direction */
#define LCD_Command_Dir DDRC   /* Define LCD command port direction register */
#define LCD_Data_Port PORTD    /* Define LCD data port */
#define LCD_Command_Port PORTC /* Define LCD data port */
#define RS PC0                 /* Define Register Select (data/command reg.)pin */
#define RW PC1                 /* Define Read/Write signal pin */
#define EN PC2                 /* Define Enable signal pin */

void LCD_Command(unsigned char cmnd)
{
    LCD_Data_Port = cmnd;
    LCD_Command_Port &= ~(1 << RS); /* RS=0 command reg. */
    LCD_Command_Port &= ~(1 << RW); /* RW=0 Write operation */
    LCD_Command_Port |= (1 << EN);  /* Enable pulse */
    _delay_us(1);
    LCD_Command_Port &= ~(1 << EN);
    _delay_ms(3);
}

void LCD_Init(void) /* LCD Initialize function */
{
    LCD_Command_Dir = 0xFF; /* Make LCD command port direction as o/p */
    LCD_Data_Dir = 0xFF;    /* Make LCD data port direction as o/p */

    _delay_ms(20);     /* LCD Power ON delay always >15ms */
    LCD_Command(0x38); /* Initialization of 16X2 LCD in 8bit mode */
    LCD_Command(0x0C); /* Display ON Cursor OFF */
    LCD_Command(0x06); /* Auto Increment cursor */
    LCD_Command(0x01); /* clear display */
    LCD_Command(0x80); /* cursor at home position */
}

void LCD_Char(unsigned char char_data)
{
    LCD_Data_Port = char_data;
    LCD_Command_Port |= (1 << RS);  /* RS=1 Data reg. */
    LCD_Command_Port &= ~(1 << RW); /* RW=0 write operation */
    LCD_Command_Port |= (1 << EN);  /* Enable Pulse */
    _delay_us(1);
    LCD_Command_Port &= ~(1 << EN);
    _delay_ms(1);
}

void LCD_String(char *str)
{
    for (int i = 0; str[i] != 0; i++)
        LCD_Char(str[i]); /* call LCD data write */
}

int timer2 = 255 - 125, counter = 0;
int h = 0, min = 0, sec = 0;
char time[20];

void tic()
{
    sec++;
    min = min + (sec / 60);
    h = h + (min / 60);
    h %= 24;
    min %= 60;
    sec %= 60;
}

void show_time(){
    LCD_Command(0x01); /* clear display */
    LCD_Command(0x80); /* cursor at home position */
    sprintf (time, "%d:%d:%d", h, min, sec);
    LCD_String(time);
}

ISR(TIMER2_OVF_vect)
{
    TCNT2 = timer2;
    counter++;
    if (counter >= 1000)
    {
        counter = 0;
        tic();
        show_time();
    }
}

int main()
{
    TCCR2 |= (1 << CS21);  // Set Timer1 prescaler to Clk/8
    TIMSK |= (1 << TOIE2); // Enable TTimer2 OverFlow Interrupt
    sei();

    LCD_Init(); /* Initialize LCD */

    TCNT2 = timer2;
    counter = 0;
    h = min = sec = 0;

    while(true){

    }
    
    return 0;
}
