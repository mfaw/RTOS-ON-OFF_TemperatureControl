#include "temp.h"

void Delay_MS_2(uint64 n)
{
 uint64 i,j;
 for(i=0;i<n;i++)
 for(j=0;j<3180;j++)
 {}
}
void TEMP_init(){
	/* Enable Clock to ADC0 and GPIO pins*/
    SYSCTL_RCGCGPIO_R |= (1<<4);   /* Enable Clock to GPIOE or PE3/AN0 */
	  Delay_MS_2(2);									/* 10 msec delay to enable the clock */
    SYSCTL_RCGCADC_R |= (1<<0);    /* ADC0 clock enable*/
    
    /* initialize PE3 for AN0 input  */
    GPIO_PORTE_AFSEL_R   |= (1<<3);       /* enable alternate function */
    GPIO_PORTE_DEN_R &= ~(1<<3);        /* disable digital function */
    GPIO_PORTE_AMSEL_R |= (1<<3);       /* enable analog function */
   
    /* initialize sample sequencer3 */
    ADC0_ACTSS_R &= ~(1<<3);        				/* disable SS3 during configuration */
    ADC0_EMUX_R &= ~0xF000;    						/* software trigger conversion */
    ADC0_SSMUX3_R = 0;         	 					/* get input from channel 0 */
    ADC0_SSCTL3_R |= (1<<1)|(1<<2);        /* take one sample at a time, set flag at 1st sample */
    ADC0_ACTSS_R |= (1<<3);         				/* enable ADC0 sequencer 3 */
	
}

volatile float mv;
volatile unsigned int adc_value;

int TEMP_read(){
    ADC0_PSSI_R |= (1<<3);        		/* Enable SS3 conversion or start sampling data from AN0 */
    while((ADC0_RIS_R & 8) == 0) ;   /* Wait untill sample conversion completed*/
    adc_value = ADC0_SSFIFO3_R; 			/* read adc coversion result from SS3 FIFO*/
    ADC0_ISC_R = 8;          				/* clear coversion clear flag bit*/
		/* convert digital value back into voltage */
		mv = (adc_value * 0.8);
		mv = (mv - 500.0) / 10.0;
		int Temperature = (int)mv;
    return Temperature;
	
			  

}