#include <LPC214x.h>

unsigned int i;
unsigned char flag=0, pwm4_int_flg=0;

/* Function prototypes */
void pwm_init(void);
void pwm_interrupt(void)__irq;

/*--------------------------------------------------------------------------------------------------------
 *        main starts here.....
 *------------------------------------------------------------------------------------------------------*/
int main ( void ) {
	pwm_init();

	while (1) {
    
	   	if(pwm4_int_flg == 0xFF) {
			pwm4_int_flg = 0x00;

	   		if(flag == 0x00) {		   
				PWMMR4 = PWMMR4+1500;	    // increment ON time 0.5msec 
		
	    		if(PWMMR4 >= 30000) {	    // turn on LED when 10msec ON time reaches 
	    			PWMMR4 = 30000;
		   		    flag = 0xff;
				}
			}
		
			if(flag == 0xFF) {		   
				PWMMR4 = PWMMR4-1500;		// decrement ON time by 0.5msec
				
	    		if(PWMMR4 <= 3000)	{		// turn OFF LED when 1msec ON time reaches 
	    			PWMMR4 = 3000;
		   		    flag = 0x00;
				}
			}
		}
	}
}

/*--------------------------------------------------------------------------------------------------------
 *        pwm_init: PWM4
 *------------------------------------------------------------------------------------------------------*/
void pwm_init(void) {
	PINSEL0 = 0X00020000;   	// PWM4(P0.8) channel 4 is selected
	PWMPR  = 0X00000000;      	// PRESCALER 
  	/*
      for selecting PWM channels from 1 to 6 in single edge
      controlled mode select the PWMPCR as following
      PWM 1 o/p PWMPCR =0X00000200; 
      PWM 2 o/p PWMPCR =0X00000400; 
      PWM 3 o/p PWMPCR =0X00000800; 
      PWM 4 o/p PWMPCR =0X00001000; 
      PWM 5 o/p PWMPCR =0X00002000; 
      PWM 6 o/p PWMPCR =0X00004000; */

	PWMPCR = 0X00001000;      	// select PWM4 single edge 
	PWMMCR = 0X00000003;      	// Reset and interrupt on PWMMR0
	PWMMR0 = 30000;           	// 10msec at 12MHz, setup match register 0 count //setup match register 0 count 	 
	PWMMR4 = 1500;      		// 0.5msec setup match register 4 count for PWM channel 4 //setup match register MR0 count for PWM channel 4
	PWMLER = 0X000000FF;      	// enable shadow copy register
	PWMTCR = 0X00000002;      	// RESET COUNTER AND PRESCALER
	PWMTCR = 0X00000009;      	// enable PWM and counter

	VICVectAddr0 = (unsigned long)pwm_interrupt;        
	VICVectCntl0 = 0x20 |8; 
	VICIntEnable |= 0x00000100;
}

/*--------------------------------------------------------------------------------------------------------
 *        pwm_interrupt: PWM4
 *------------------------------------------------------------------------------------------------------*/
void pwm_interrupt(void)__irq {
	pwm4_int_flg = 0xFF;
	PWMIR = 0XFF;
	PWMLER = 0X000000FF;
	VICVectAddr=0; 
}

