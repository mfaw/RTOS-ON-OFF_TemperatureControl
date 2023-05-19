
#include "lcd_o.h"
#include <stdlib.h>
#include <stdio.h>

#define NUMBER_OF_ITERATIONS_PER_ONE_MILI_SECOND 762
void Delay_MS(uint64 n)
{
    volatile unsigned long long count = 0;
    while(count++ < (NUMBER_OF_ITERATIONS_PER_ONE_MILI_SECOND * n) );
}

void Delay_US(uint64 n)
{
 uint64 i,j;
 for(i=0;i<n;i++)
 for(j=0;j<3;j++)
 {}
}

void LCD_init(void)
{
  	 SYSCTL->RCGCGPIO |=0x22;  // clock for port F and port B
	 LCDD->DIR |=0xFF; 
	 LCDD->DEN |=0xFF; 
	 LCDC->DIR |=0xFF; 
	 LCDC->DEN |=0xFF; 
	  
	LCD_sendCommand(TWO_LINE_LCD_Eight_BIT_MODE); 
	LCD_sendCommand(CURSOR_OFF);
	LCD_clearScreen();
}

void LCD_sendCommand(uint8 command)
{
	CLEAR_BIT(LCD_CONTROL_R,RS); /* Instruction Mode RS=0 */
	CLEAR_BIT(LCD_CONTROL_R,RW); /* write data to LCD so RW=0 */
	Delay_MS(1); /* delay for processing Tas = 50ns */
	SET_BIT(LCD_CONTROL_R,EN); /* Enable LCD E=1 */
	Delay_MS(1); /* delay for processing Tpw - Tdws = 190ns */

    LCD_DATA_R = command; /* out the required command to the data bus D0 --> D7 */
    Delay_MS(1); /* del	ay for processing Tdsw = 100ns */
    CLEAR_BIT(LCD_CONTROL_R,EN); /* disable LCD E=0 */
    Delay_MS(1); /* delay for processing Th = 13ns */
}

void LCD_displayCharacter(uint8 data)
{
	SET_BIT(LCD_CONTROL_R,RS); /* Data Mode RS=1 */
	CLEAR_BIT(LCD_CONTROL_R,RW); /* write data to LCD so RW=0 */
	Delay_US(15); /* delay for processing Tas = 50ns */
	SET_BIT(LCD_CONTROL_R,EN); /* Enable LCD E=1 */
	Delay_US(15); /* delay for processing Tpw - Tdws = 190ns */

	LCD_DATA_R = data; /* out the required data to the data bus D0 --> D7 */
	if ((data&1<<6) == 0)
	{
		LCD_DATA_R &= ~(1<<6);
	}
	else if ((data & 1 <<6) == 1<<6)
	{
		LCD_DATA_R |= (1<<6);
	}
	Delay_US(15); /* delay for processing Tdsw = 100ns */
	CLEAR_BIT(LCD_CONTROL_R,EN); /* disable LCD E=0 */
	Delay_US(15); /* delay for processing Th = 13ns */
}

void LCD_displayString(const char *Str)
{
	// uint8 i = 0;
	// while(Str[i] != '\0')
	// {
	// 	LCD_displayCharacter(Str[i]);
	// 	Delay_MS(500); /* delay for processing Th = 13ns */
	// 	i++;
	// }

	while((*Str) != '\0')
	{
		LCD_displayCharacter(*Str);
		Str++;
	}		
}

void LCD_goToRowColumn(uint8 row,uint8 col)

{
	uint8 Address;
	
	/* first of all calculate the required address */
	switch(row)
	{
		case 0:
				Address=col;
				break;
		case 1:
				Address=col+0x40;
				break;
		case 2:
				Address=col+0x10;
				break;
		case 3:
				Address=col+0x50;
				break;
	}					
	/* to write to a specific address in the LCD 
	 * we need to apply the corresponding command 0b10000000+Address */
	LCD_sendCommand(Address | SET_CURSOR_LOCATION); 
}

void LCD_clearScreen(void)
{
	LCD_sendCommand(CLEAR_COMMAND); //clear display 
}

