// EXTERNAL INTERRUPT0
//----------------------------------------------------------------
// CONTROLLER 		: LPC-2148  	
// DATE 			: December - 2015
// Developed By 	: Advanced Electronic Systems Bangalore,India
//----------------------------------------------------------------
//----------------------------------------------------------------

#include <LPC21xx.h>
 
void EINT0_Init(void);
void Extint0_Isr(void)__irq;

unsigned char int_flg=0x00, flag=0x00;

int main ( void )
{
	EINT0_Init();

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
			{									    // when flag is '0xFF' OFF the LED
				IO1CLR = 0X02000000;
				flag = 0x00;
			}
		}		   
	}			 
}

void EINT0_Init(void)
{
	IO1DIR |= 0X02000000;  							// P1.25 for LED indication
   	PINSEL1 &= ~0x00000003;
   	PINSEL1	|= 0X00000001;          				// Setup P0.16 to alternate function EINT0
   	EXTMODE	= 0x01;                					// edge i.e falling egge trigger and active low
   	EXTPOLAR = 0X00;
   	VICVectAddr0 = (unsigned long) Extint0_Isr;   	// Assign the EINT0 ISR function 
   	VICVectCntl0 = 0x20 | 14;      					// Assign the VIC channel EINT0 to interrupt priority 0
   	VICIntEnable |= 0x00004000;    					// Enable the EINT0 interrupt
}

void Extint0_Isr(void)__irq           			// whenever there is a low level on EINT0
{
	EXTINT |= 0x01;                 				// Clear interrupt
	int_flg = 0xFF;
 	VICVectAddr = 0;								// Acknowledge Interrupt
}



