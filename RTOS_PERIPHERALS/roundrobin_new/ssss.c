#include <lpc21xx.h>
 #include <rtl.h>
 



#include <stdio.h>

void sev(void);
void lcd(void);
void cmd(unsigned int);
void data(unsigned int);
void delay(unsigned int);
void delay1(void);
								 void init_serial(void);
								 unsigned int counter1;
								 char arr1[20];

 unsigned int Disp[16]={0x003F0000, 0x00060000, 0x005B0000, 0x004F0000, 0x00660000,0x006D0000,0x007D0000, 0x00070000, 0x007F0000, 0x006F0000};

	unsigned char i;



__task void job1 (void);

__task void job2 (void);

__task void job3 (void);


__task void job1 (void) 
{

  os_tsk_create (job2, 0);   /* Create task 2 and mark it as ready */

                /* loop forever */
		 while(1)
		 {
    		   sev();

			  }

		     }



__task void job2 (void) {
 os_tsk_create (job3, 0);

    while(1)
	{             /* loop forever */

    	lcd();
	   delay(65000);
delay(65000);
  	 }



  }
__task void job3 (void) {
	counter1=0;
while(1)
{	init_serial();
counter1++;
sprintf(arr1,"counter:%d",counter1);
while(arr1[i]!='\0')
{ 
   while(!(U0LSR&0X20));
    U0THR=arr1[i];
	i++;
  }
  i=0;
  while(!(U0LSR&0X20));
     U0THR='\n';
  }
}


void delay1(void)
{ unsigned long int j;
for(j=0;j<650000;j++);
}

void sev(void)
{

IODIR0=0xf0ff0000; // making po.16 to p0.23  and p0.28 to p0.31 output lines
IOSET0=0xf0000000;

for(i=0;i<10;i++)
{
IOSET0 |=Disp[i];
					       
delay1();
 delay1();
 //;delay1()//;delay1();//delay1();delay1();delay1();
 IOCLR0=0x00ff0000;	
}

}







 void lcd()
{

unsigned  char msg[]={"ABCDEF"};
unsigned  int c[]={0x30,0x20,0x28,0x01,0x06,0x0e,0x80};
unsigned char i,j;
PINSEL0=0x00000000;
IODIR0=0x000000fc;
IOCLR0=0x000000fc;
for(i=0;i<9;i++)

{
cmd(c[i]);
delay(10000);


}
  cmd(0x80);
 delay(10000);
 delay(10000);
for(j=0;j<6;j++)
{

data(msg[j]);
delay(500);
delay(65000);
delay(65000); delay(65000);
delay(65000);delay(65000);
delay(65000);

delay(10000);

}  	cmd(0x01);
delay(65000);
delay(65000);
delay(65000);


}  


void cmd(unsigned int value)
{
unsigned int y;
y=value;
y=y & 0xf0;
IOCLR0=0x000000fc;
 IOCLR0=0X00000004;
IOSET0=y;

IOSET0=0x00000008;
delay(10);
IOCLR0=0x00000008;

y=value;
y=y & 0x0f;
y=y<<4;
IOCLR0=0x000000fc;
 IOCLR0=0X00000004;
IOSET0=y; 

IOSET0=0x00000008;
delay(10);
IOCLR0=0x00000008;

}
void data(unsigned int dat)
{
unsigned int y;
y=dat;
y=y & 0xf0;
IOCLR0=0x000000fc;
   IOSET0=0X00000004;
IOSET0=y; 
 
IOSET0=0x00000008;
delay(10);
IOCLR0=0x00000008;

y=dat;
y=y & 0x0f;
y=y<<4;
 IOCLR0=0x000000fc;	 IOSET0=0X00000004;
IOSET0=y;

IOSET0=0x00000008;
delay(10);
IOCLR0=0x00000008;

}

   void delay(unsigned int x)
   {
   unsigned int del;

   for(del=0;del<x;del++);
   }
   void init_serial(void)
{
  PINSEL0=0X00000005;
  U0LCR=0X83;
  U0DLL=0X61;
  U0LCR=0X03;
}


int  main (void)

 {

   os_sys_init (job1);       
  for (;;);
  	}


