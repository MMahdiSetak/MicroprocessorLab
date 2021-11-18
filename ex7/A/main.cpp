#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "LCD.h"
#define login_state 0
#define home_state 1
#define actions_state 2
#define date_state 3
#define user_state 4
#define stepper_state 5
#define user_manager_state 6
int state = login_state, user_num = 1, current_user = -1, temperature = 25;
int year = 2021, month = 10, day = 18, hour = 20, minute = 10, second = 45;
const int timer1 = 65535 - 15625, max_users = 2;
struct User
{
    char id[20], pass[20];
} users[max_users] = {{"1", "123"}};
bool key_p = 0;
char keypad[4][4] = {
    {'7', '8', '9', '/'},
    {'4', '5', '6', '*'},
    {'1', '2', '3', '-'},
    {'c', '0', '=', '+'}};
void remove_char(char *input)
{
    int i = 0;
    for (; i < line_size; i++)
        if (input[i] == 0)
            break;
    input[i - 1] = 0;
}
char key_handler()
{
    int pin = 0x0F & PINC, j = 0, t = 1, i;
    if (!pin || key_p)
        return key_p = pin, '$';
    key_p = 1;
    while (pin >>= 1)
        j++;
    PORTC = 0x30;
    if (0x0F & PINC)
        t = 0;
    PORTC = (1 << (2 * t)) << 4;
    i = (0x0F & PINC ? 0 : 1) + (2 * t);
    PORTC = 0xF0;
    return keypad[j][i];
}
void input_handler(int line, char *out)
{
    LCD_Command(line);
    while (1)
    {
        char key = key_handler();
        if (key == '$')
            continue;
        if (key == '=')
            break;
        if (key == 'c' && strlen(out) > 0)
        {
            remove_char(out);
            LCD_Command(0x10);
            LCD_Char(' ');
            LCD_Command(0x10);
            continue;
        }
        LCD_Char((state == login_state && line == fourth_line) ? '*' : key);
        strncat(out, &key, 1);
    }
}
void login()
{
    LCD_Clear(1);
    LCD_String("Enter your ID:");
    char id[20] = "";
    input_handler(second_line, id);
    LCD_Write_Line(third_line, "Enter your password:");
    char pass[20] = "";
    input_handler(fourth_line, pass);
    for (int i = 0; i < user_num; i++)
        if (strcmp(users[i].id, id) == 0 && strcmp(users[i].pass, pass) == 0)
        {
            PORTD = 1 << PD6;
            current_user = i;
            state = home_state;
            return;
        }
    PORTD = 1 << PD7;
    LCD_Notify("Wrong credential,   Please try again");
}
void print_home_info()
{
    LCD_Clear(0);
    char date[20];
    sprintf(date, "Date: %d/%d/%d", year, month + 1, day + 1);
    LCD_String(date);
    char time[20];
    sprintf(time, "Time: %d:%d:%d", hour, minute, second);
    LCD_Write_Line(second_line, time);
    char temp[20];
    sprintf(temp, "Temperature: %dC", temperature);
    LCD_Write_Line(third_line, temp);
    LCD_Write_Line(fourth_line, "Press X for Actions");
}
void home_page()
{
    print_home_info();
    while (1)
    {
        char key = key_handler();
        if (key == '*')
            return void(state = actions_state);
    }
}
void actions_page()
{
    LCD_Clear(0);
    LCD_String("1.Stepper Motor");
    LCD_Write_Line(second_line, "2.Logout");
    if (current_user == 0)
    {
        LCD_Write_Line(third_line, "3.Change Date");
        LCD_Write_Line(fourth_line, "4.User Management");
    }
    while (1)
    {
        char key = key_handler();
        if (key == '1')
            return void(state = stepper_state);
        if (key == '2')
        {
            state = login_state;
            current_user = -1;
            PORTD = 0x00;
            return;
        }
        if ((key == '4' || key == '3') && current_user == 0)
            return void(state = key - '0');
        if (key == 'c')
            return void(state = home_state);
    }
}
void stepper_page()
{
    LCD_Clear(1);
    LCD_String("Enter the number of");
    LCD_Write_Line(second_line, "cycles: (1-255)");
    char cycles[20] = "";
    input_handler(third_line, cycles);
    int data = atoi(cycles);
    UDR = data;
    LCD_Notify("Rotating the StepperMotor");
    state = home_state;
}
void date_page()
{
    LCD_Clear(1);
    LCD_String("Enter Year,Month,Day");
    char y[20] = "";
    input_handler(second_line, y);
    year = atoi(y);
    char m[20] = "";
    input_handler(third_line, m);
    month = atoi(m) - 1;
    char d[20] = "";
    input_handler(fourth_line, d);
    day = atoi(d) - 1;
    LCD_Command(0x01);
    LCD_String("Enter Hour & Minute");
    char h[20] = "";
    input_handler(second_line, h);
    hour = atoi(h);
    char mint[20] = "";
    input_handler(third_line, mint);
    minute = atoi(mint);
    LCD_Notify("Date and clock      changed");
    state = home_state;
}
void user_manager(int mode)
{
    LCD_Clear(1);
    LCD_String("Enter the user ID:");
    char id[20] = "";
    input_handler(second_line, id);
    if (mode)
    {
        for (int i = 0; i < user_num; i++)
            if (strcmp(id, users[i].id) == 0)
            {
                user_num--;
                for (int j = i; j < user_num; j++)
                    users[j] = users[j + 1];
                break;
            }
        LCD_Notify("User removed        successfully");
        state = home_state;
        return;
    }
    LCD_Write_Line(third_line, "Enter User Password:");
    char pass[20] = "";
    input_handler(fourth_line, pass);
    bool edited = 0;
    for (int i = 0; i < user_num; i++)
        if (strcmp(id, users[i].id) == 0)
        {
            strcpy(users[i].pass, pass);
            edited = 1;
            LCD_Notify("User credential     updated successfully");
            break;
        }
    if (!edited)
    {
        if (user_num < max_users)
        {
            strcpy(users[user_num].id, id);
            strcpy(users[user_num].pass, pass);
            user_num++;
            LCD_Notify("User added          successfully");
        }
        else
            LCD_Notify("Action failed.      Memory is full");
    }
    state = home_state;
}
void user_page()
{
    LCD_Clear(0);
    LCD_String("1.Add/Edit User");
    LCD_Write_Line(second_line, "2.Remove User");
    while (state == user_state)
    {
        char key = key_handler();
        if (key == '1' || key == '2')
            user_manager(key - '1');
    }
}
void time_handler()
{
    minute = minute + floor((1.0 * second) / 60);
    hour = hour + floor((1.0 * minute) / 60);
    day = day + floor((1.0 * hour) / 24);
    month = month + floor((1.0 * day) / 30);
    year = year + floor((1.0 * month) / 12);
    month %= 12;
    day %= 30;
    hour %= 24;
    minute %= 60;
    second %= 60;
}
ISR(TIMER1_OVF_vect)
{
    TCNT1 = timer1;
    second++;
    time_handler();
    if (state == home_state)
        print_home_info();
}
ISR(USART_RXC_vect)
{
    temperature = UDR;
}
int main()
{
    UCSRB = (1 << RXCIE) | (1 << RXEN) | (1 << TXEN); // Enable RX and TX and interrupt on RX
    UBRRL = 103;                                      // 9600 bps
    LCD_Init();
    DDRC = 0xF0;
    PORTC = 0xF0;
    DDRD = 0xFE;
    TCCR1B |= (1 << CS11) | (1 << CS10); // Set Timer1 prescaler to Clk/64
    TIMSK |= (1 << TOIE1);               // Enable Timer1 OverFlow Interrupt
    sei();
    while (1)
    {
        switch (state)
        {
        case login_state:
            login();
            break;
        case home_state:
            home_page();
            break;
        case actions_state:
            actions_page();
            break;
        case stepper_state:
            stepper_page();
            break;
        case date_state:
            date_page();
            break;
        case user_state:
            user_page();
            break;
        }
    }
    return 0;
}