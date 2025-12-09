// 3MHZ,VPBDIV =ENABLE


#include <lpc21xx.h>
#include <stdio.h>
 char arr1[10],arr2[10],arr3[20];
void rtc(void)
{
PREINT=0X5A;
PREFRAC=0X46C0;
CCR=0X00;
SEC=0;
MIN=24;
HOUR=3;
CCR=0X01;
}

   void serial()
   {
   PINSEL0=0X0000005;  		//select TXD0 and RXD0 lines										0											0       IODIR1 = 0X00ff0000;//define as o/p lines
        U0LCR  = 0X00000083;		//enable baud rate divisor loading and
	          						//select the data format
        U0DLM = 0X00; 
        U0DLL = 0x61;      		//select baud rate 9600 bps
        U0LCR  = 0X00000003;
		}

void delay(void)
{
int i;
for (i=0;i<200000;i++);
}
unsigned int i,x,y,z;

//void lcd_init(void);

int main()
{

serial();
	rtc();
	while(1)
	{
	i=0;
		x=SEC; 	y=MIN; z=HOUR;
  	   sprintf(arr1,"sec:%d",x);
  	while (arr1[i] != '\0')
	{	 
	  	while (!(U0LSR & 0x20));
	  	U0THR = arr1[i];
		i++;
	}
	U0THR='\n';
//	delay();  	delay();	delay();

	i=0;
	 	sprintf(arr2,"min:%d",y);
  	while (arr2[i] != '\0')
	{
	  	while (!(U0LSR & 0x20));
	  	U0THR = arr2[i];
		i++;
	}
		U0THR='\n';
	i=0;
		//	delay();
			
			sprintf(arr3,"hr:%d",z);
  	while (arr3[i] != '\0')
	{
	  	while (!(U0LSR & 0x20));
	  	U0THR = arr3[i];
		i++;
	}
		U0THR='\n';
	i=0;
	//  delay();

    }
}