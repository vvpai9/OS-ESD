#include<LPC21XX.h>
#include<stdio.h>
void delay(unsigned int);
void extint0_ISR(void)__irq;
void delay(unsigned int );
int main(void)
{
	PINSEL1=0x00000001;
	EXTMODE=0x00000001;
	EXTPOLAR=0x00;
	VICVectAddr0=(unsigned long)extint0_ISR;
	VICVectCntl0=0x20|14;
	VICIntEnable|=0x00004000;
	while(1)
		delay(123);
}
void extint0_ISR(void)__irq
{
unsigned int i;	
//	unsigned int data[]={0xF03F0200,0xF0060200,0xF05B0200,0xF04F0200,0xF0260200,0xF06D0200,0XF07D0200,0XF00070200,0XF07F0200,0XF06F0200};
	
	PINSEL0=0x00000000;
	IODIR0=0x00000200;
	//count++;
	IOSET0=0X00000200;
					                  
	
	 delay(10000);	 delay(10000); delay(10000); delay(10000); 	 delay(10000);	 delay(10000); delay(10000); delay(10000);
	IOCLR0=0X00000200;
	EXTINT|=0X00000001;
	VICVectAddr=0;
}
void delay(unsigned int x)
{
 unsigned int i;
 for(i=0;i<x;i++) ;
}	