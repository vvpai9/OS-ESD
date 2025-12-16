// EXTERNAL INTERRUPT1
//----------------------------------------------------------------
// CONTROLLER 		: LPC-2148  	
// DATE 			: December - 2015
// Developed By 	: Advanced Electronic Systems Bangalore,India
//----------------------------------------------------------------
//----------------------------------------------------------------

#include <LPC21xx.h>

void EINT1_Init(void);
void Extint1_Isr(void)__irq;

unsigned char int_flg=0x00, flag=0x00;

int main ( void )
{
	EINT1_Init();

	while(1)
	{
		if(int_flg == 0xFF)
		{
			int_flg = 0x00;
			if(flag == 0x00)
			{							// when flag is '0x00' ON the LED
				IO1SET = 0x02000000;
				flag = 0xFF;
			}										
		
			else
			{							// when flag is '0xFF' OFF the LED
				IO1CLR = 0X02000000;
				flag = 0x00;
			}
		}		   
	}			 
}

void EINT1_Init(void) {
	IO1DIR |= 0X02000000; 							// P1.25 int led
   	PINSEL0 &= ~0x000000C0;
   	PINSEL0	|= 0X000000C0;          				// Setup P0.3 to alternate function EINT1
   	EXTMODE	= 0x02;                					// edge i.e falling egge trigger and active low
   	EXTPOLAR= 0X00;
   	VICVectAddr0 = (unsigned long) Extint1_Isr;   	// Assign the EINT1 ISR function 
   	VICVectCntl0 = 0x20 | 15;      					// Assign the VIC channel to EINT1 interrupt
   	VICIntEnable |= 0x00008000;    					// Enable the EINT1 interrupt
}

void Extint1_Isr(void)__irq {             			// whenever there is a low edge on EINT1
	EXTINT |= 0x02;									// clear the interrupt
	int_flg = 0xFF;
	VICVectAddr=0;                        			// Acknowledge Interrupt
} 



