
#ifndef LCD
#define LCD

#include "std_type.h"
#include "tm4c123gh6pm.h"
#include "TM4C123.h"
#include "macros.h"

#define LCD_CONTROL_R GPIO_PORTF_DATA_R
#define LCD_DATA_R GPIO_PORTB_DATA_R

#define LCDC GPIOF
#define LCDD GPIOB

#define RS 1
#define RW 2
#define EN 3

#define CLEAR_COMMAND 0x01
#define FOUR_BITS_DATA_MODE 0x02
#define TWO_LINE_LCD_Four_BIT_MODE 0x28
#define TWO_LINE_LCD_Eight_BIT_MODE 0x38
#define CURSOR_OFF 0x0C
#define CURSOR_ON 0x0E
#define SET_CURSOR_LOCATION 0x80 


void LCD_sendCommand(uint8 command);
void LCD_displayCharacter(uint8 data);
void LCD_displayString(const char *Str);
void LCD_init(void);
void LCD_clearScreen(void);
void LCD_goToRowColumn(uint8 row,uint8 col);
void Delay_MS(uint64 n);
void Delay_US (uint64 n);
#endif