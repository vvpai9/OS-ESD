/************************************************************************
 * LED Test Demonstration
 * Developed by
 * Advanced Electronics Systems, Bengaluru
 *------------------------------------------------------------------------
 * This example toggles the LED LD1 through the port line P2.0
 * Delay is given in between changing the LED status
 ************************************************************************/

 #include<LPC17xx.h>
 #include<stdio.h>

 #define LED_TOGGLE (1<<0)							//0th bit is made high
 #define LED_ON() LPC_GPIO2->FIOSET = LED_TOGGLE	//Set P2.0 bit
 #define LED_OFF() LPC_GPIO2->FIOCLR =  LED_TOGGLE	//clear P2.0 bit

 unsigned int delay(int);

 int main(void)
 {
	SystemInit();
	SystemCoreClockUpdate();
	
	LPC_PINCON->PINSEL4 = 0x00000000;	 //P2 is made as GPIO
	LPC_GPIO2->FIODIR = 0x00000001;		 //p2.0 is output
	LPC_GPIO2->FIOCLR = 0x00000001;	 //Clear the P2.0 bit

	while(1)
	{
		LED_ON();						//On the LED
		delay(100000);					//13usec to execute single instruction
		LED_OFF();						//Off the LED
		delay(100000);					//40msec for 10000 counts
	}
 }

 unsigned int delay(int r1)
 {
 	int r2	;
	for(r2=0;r2<r1;r2++);
	return 0;
 }
