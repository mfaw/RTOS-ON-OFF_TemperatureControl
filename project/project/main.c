#define Led_Red (1U << 2)
#define Led_Blue (1U << 1)
#include <stdint.h>
#include <FreeRTOS.h>
#include "tm4c123gh6pm.h"
#include "task.h"
#include "queue.h"
#include "system_TM4C123.h"
#include "semphr.h"
#include "TM4C123.h"    // Device header
#include "lcd_o.h""
#include "temp.h"
#include <stdlib.h>
#include <stdio.h>

static void mainControllerTask(void *pvParameters);
static void LcdControllerTask(void *pvParameters);
static void AlarmLedTask(void *pvParameters);
static void UartControler(void *pvParameters);

void UART0_Transmitter(char data);
void UART0_Transmitter_INT(unsigned int number);
char UART0_Read();
xSemaphoreHandle xSemaphoreHandler;
portBASE_TYPE xMutex;

xQueueHandle xUARTQueue;
xQueueHandle xLCDQueue;
xQueueHandle xLEDALARMQueue;


void printstring(char *str);


int main(void)
{ 
	xUARTQueue = xQueueCreate(1 , sizeof(char)); // Queue handler resposible for incoming setpoint request from the user
	xLEDALARMQueue = xQueueCreate(1 , sizeof(char)); // Queue handler resposible for enabling the buzzer (th eblue led)
	xLCDQueue = xQueueCreate(1 , 8);// Queue handler resposible for controlling and setting what will be displayed on the LCD 
	
	LCD_init(); // initialize LCD 
	TEMP_init();// initialize temperature sensor ( potentiometer )

	
	// enabling port A and setting the UART0 to communicate with Puty on baud rate of 9600
	GPIOE->DIR |=0x06; 
	GPIOE->DEN |=0x06;  
	
	SYSCTL_RCGCGPIO_R |= 0x01U;
	SYSCTL_RCGCUART_R |= 0x01U; 
	while(SYSCTL_PRGPIO_R & 0x01U == 0x00){}
	UART0_CTL_R &= ~0x001;
	UART0_IBRD_R = 104;
	UART0_FBRD_R = 11;
	UART0_CC_R = 0;
	UART0_LCRH_R = 0x060;
	UART0_CTL_R = 0x0301;
		
	GPIO_PORTA_AFSEL_R |= 0x03;
	GPIO_PORTA_PCTL_R = (GPIO_PORTA_PCTL_R & 0XFFFFFF00) + 0X011;
	GPIO_PORTA_DEN_R |= 0x03;
	GPIO_PORTA_AMSEL_R &= ~0x03;
		
	// craeting 4 tasks with the same prioirty 
	xTaskCreate( mainControllerTask,(const portCHAR *)"mainControllerTask", 240, NULL, 3, NULL );
	xTaskCreate( LcdControllerTask,(const portCHAR *)"LcdControllerTask", 240, NULL, 3, NULL );
	xTaskCreate( AlarmLedTask,(const portCHAR *)"AlarmLedTask", 240, NULL, 3, NULL );
	xTaskCreate( UartControler,(const portCHAR *)"UartControler", 240, NULL, 3, NULL );
	vTaskStartScheduler();
		

		
	return 0;
}

void printstring(char * str){
	while(*str){
		UART0_Transmitter(*(str++));
	}
}
/* the main controller that check the temp value and accordingly maniuplate the heater and the buzzer states */
static void mainControllerTask(void *pvParameters){
	unsigned char set_point = 30;
	typedef struct Message
	{
		char msg1[4]; // the measured value to be displayed (temp sensor value)
		char msg2[4]; // the setpoint value to be displayed 
	} ABmessage; // structre for the message that will be displayed
	
	unsigned char Alarm_value = 50; // the maximum predefined value for the temp value before the buzzer turning on 
	ABmessage msg;
	// char mesg[12];
	for( ;; ){
		const unsigned char on = 2; // value for enabling the buzzer
	    const unsigned char off = 1; // value for tunring the buzzer off

		xQueueReceive(xUARTQueue , &set_point , 0);  // get any new setpoint value if available from the user
		unsigned char Temperature = TEMP_read(); // ge tthe temperature 
		//sprintf(mesg, "\r\nVoltage = %d Volts", Temperature);
		//printstring(mesg);
		
		if(Temperature < set_point){
			GPIO_PORTE_DATA_BITS_R[Led_Red] = Led_Red; // enabling the red led ( the heater) if the value is less than the setpoint
		}
		else{
			GPIO_PORTE_DATA_BITS_R[Led_Red] = 0x0; // otherwise disable the heater
		}
		
		// build the message to be displayed on the LCD
		sprintf(msg.msg1, "%d", Temperature);  // the measure temperature value
		sprintf(msg.msg2, "%d", set_point); // the setpoint value ( the default or the new modified by the user)
		xQueueSendToBack(xLCDQueue , &msg , 0);
		
		
		// the blue led is to be the buzzer in this case
		if(Temperature > Alarm_value){
			xQueueSendToBack(xLEDALARMQueue , &on , 0); // enabling the buzzer if the value is greater than the maximim predefined value
		}
		else{
			xQueueSendToBack(xLEDALARMQueue , &off , 0); // disable the buzzer otherwise
		}
	}
}
/* the LCD controller responsible for displaying the constructed message on the 2 row 16 column LCD */
static void LcdControllerTask(void *pvParameters){
	typedef struct Message
	{
		char msg1[4];
		char msg2[4];
	} ABmessage;
	ABmessage msg;
	for( ;; ){
		
		xQueueReceive(xLCDQueue , &msg , 0); // recieve if there is any message to be displayed on the screen
		LCD_goToRowColumn(0,0);
		LCD_displayString("Measured:"); // type "measured:" on the first row along with the temperature value sensed 
		LCD_displayString(msg.msg1);
		LCD_goToRowColumn(1,0);
		LCD_displayString("Setpoint:"); // type "Setpoint:" alond with the setpoint value 
		LCD_displayString(msg.msg2);
		vTaskDelay(10);
		LCD_goToRowColumn(0,10); // clear the last 2 digits on the first row
		LCD_displayString("  "); 
		LCD_goToRowColumn(1,10); // clear the last 2 digits on the second row
		LCD_displayString("  ");
		
	}
}
static void AlarmLedTask(void *pvParameters){
	
	for( ;; ){
		unsigned char alarmState;
		xQueueReceive(xLEDALARMQueue , &alarmState , 0); // fetch the state (new state) the buzzer should be in (blue led)
		if(alarmState == 2){
			GPIO_PORTE_DATA_BITS_R[Led_Blue] = Led_Blue; // turn the led on if the value is 2
		}
		else{
			GPIO_PORTE_DATA_BITS_R[Led_Blue] = 0x0; // otherwise turn the blue led off 
		}
		
	}
}
/* the task responsible for handling the incoming requests from user */
static void UartControler(void *pvParameters){
	for( ;; ){
		printstring("\n\r\n\rEnter Temperature Setpoint (Degrees): ");
		unsigned int N = 0;
		unsigned char total = 0;
		while(1){
			N = UART0_Read(); //  read the charchter from the FIFO 
			UART0_Transmitter(N); // resend the same character to be displayed for the user 
			if(N == '\r') break;
			N -= '0'; // create a number from the charchter entered by the user
			total = total * 10 + N; // build the total number entered by the user from indvidual digits
		}
		xQueueSendToBack(xUARTQueue , &total , 10);
		printstring("\n\rTemperature Setpoint changed...");
		
	}
}
/* transmit a single charchter to UART0 DR_R */
void UART0_Transmitter(char data)  
{
    while((	UART0_FR_R& 0x20) != 0); // poll on the FIFO to check if it's full or empty
    UART0_DR_R = data;                 // send the data if the fifo isn't full 
}
/* read the charcter entered by the user */
char UART0_Read() {
	while((UART0_FR_R&(1<<4))!=0);	// // poll on the FIFO to check if it's full or empty
	char data = UART0_DR_R; // read the charchter entered by the user if the FIFO isn't empty
	return data; 
}


