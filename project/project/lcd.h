 /******************************************************************************
 *
 * Module: LCD
 *
 * File Name: lcd.h
 *
 * Description: Header file for the LCD driver
 *
 * Author: Mohamed Tarek
 *
 *******************************************************************************/

#ifndef LCD_H_
#define LCD_H_

#include "std_types.h"
#include "common_macros.h"
#include "Platform_types.h"

/*******************************************************************************
 *                      Preprocessor Macros                                    *
 *******************************************************************************/
/* LCD Data bits mode configuration */
#define DATA_BITS_MODE 8

/* Use higher 4 bits in the data port */
#if (DATA_BITS_MODE == 4)
#define UPPER_PORT_PINS
#endif

/* LCD HW Pins */
#define RS PIN1
#define RW PIN2
#define E  PIN3
#define LCD_CTRL_PORT GPIO_PORTF_DATA_R
#define LCD_CTRL_PORT_DIR GPIO_PORTF_DIR_R

#define LCD_DATA_PORT GPIO_PORTB_DATA_R
#define LCD_DATA_PORT_DIR GPIO_PORTB_DIR_R

/* LCD Commands */
#define CLEAR_COMMAND 0x01
#define FOUR_BITS_DATA_MODE 0x02
#define TWO_LINE_LCD_Four_BIT_MODE 0x28
#define TWO_LINE_LCD_Eight_BIT_MODE 0x38
#define CURSOR_OFF 0x0C
#define CURSOR_ON 0x0E
#define SET_CURSOR_LOCATION 0x80 

/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/
void LCD_sendCommand(uint8 command);
void LCD_displayCharacter(uint8 data);
void LCD_displayString(const char *Str);
void LCD_init(void);
void LCD_clearScreen(void);
void LCD_displayStringRowColumn(uint8 row,uint8 col,const char *Str);
void LCD_goToRowColumn(uint8 row,uint8 col);
void LCD_intgerToString(int data);
#define NUMBER_OF_ITERATIONS_PER_ONE_MILI_SECOND 762
void Delay_MS(unsigned long long n);
#endif /* LCD_H_ */
