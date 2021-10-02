#define F_CPU 8000000UL /* Define CPU Frequency e.g. here 8MHz */

#include <string.h>
#include <avr/io.h>
#include <avr/delay.h>
#include <avr/eeprom.h>

char data[200], content[200] = "The European languages are members of the same family. Their separate existence is a myth. For science, music, sport, etc, Europe uses the same vocabulary. The languages only differ in their grammar";

int main()
{
    DDRB = 0xFF;
    PORTB = 0xFF;

    eeprom_busy_wait();
    eeprom_write_block(content, 0, strlen(content)); //Write the content to EEPROM
    eeprom_read_block(data,0,strlen(content)); // Read the content from EEPROM

    _delay_ms(100);

    for (int i = 0; i != 200; i++)
    {
        PORTB = data[i];
        _delay_ms(1);
    }

    PORTB = 0;

    return 0;
}
