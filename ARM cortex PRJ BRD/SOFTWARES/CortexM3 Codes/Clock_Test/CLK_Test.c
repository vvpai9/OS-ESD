/**********************************************************************
 * CPU Clock Testing
 * Developed by
 * Advanced Electronics Systems, Bengaluru
 *---------------------------------------------------------------------
 * Pin P1.27 of LPC1768 outputs the Clock pulse when it is configured
 * as clkout pin. CLKOUTCFG register allows to select clock source and
 * division factor. A continuous clock pulse can bee observed at the pin
 * P1.27. CLKOUTCFG regster directly configure the clock source, no need
 * to add SystemInit() function here 
 **********************************************************************/

 #include <LPC17xx.h>
 #include <stdio.h>

 int main(void)
 {
	LPC_PINCON->PINSEL3 &= ~0x00C00000;	//masking if any other functions are enabled
	LPC_PINCON->PINSEL3 |= 0x00400000;	//clkout pin as P1.27

	LPC_SC->CLKOUTCFG = 0x00000131;		//main oscillator clock source, divide by 4, enable the clkout
	while(1);
 }
