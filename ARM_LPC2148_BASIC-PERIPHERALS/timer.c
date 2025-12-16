
// example program to show TIMER0 
//basetimevalue=1ms 
//  count(PR)=(pclk*basetimevalue)-1    
//			 =(12MHz *0.001 sec)-1
//			= 11999	


//DESIRED_COUNT = requiredtime *basetimevalue	= 1000* 0.001= 1sec 

#include <LPC21xx.H>
 
#define DESIRED_COUNT	1000			// for 1sec

#define PRESCALER	11999


void InitTimer0(void);

void InitTimer0(void)
{
			 	
	T0PR=PRESCALER;
	T0MR0=DESIRED_COUNT;	//interrupt every 1 sec for interval = 1000
	T0MCR=3;		//interrupt and reset when counter=match
	T0TCR=2;		 //	reset timer
   
}

int main (void) 
{	
	PINSEL0 = 0X00000000 ;			// Configure P0.16-p0.23 as GPIO
	IODIR0=0X00FF0000;
	InitTimer0();  					// initialise timer0 - 
	T0TCR = 0x01;					// start timer 
	while(1)
	{
	while(!( T0IR==0x01));			   // wait for overflow
	T0IR=0x01;						  // clear flag
 	IOCLR0=0X00FF0000;					 // turn on led 
	while(!( T0IR==0x01));
	IOSET0=0X00FF0000;					// turn off flag
	T0IR=0x01;
    }

}








