#include<lpc21xx.h>
void delay(unsigned int);
void disp(unsigned int);
int main()
{

unsigned  long int value,i;	
unsigned int row0[4]={ 0x00ee0000,0x00ed0000,0x00eb0000,0x00e70000};
	
unsigned int row1[4]={ 0x00de0000,0x00dd0000,0x00db0000,0x00d70000};
	
unsigned int row2[4]={ 0x00db0000,0x00bd0000,0x00bb0000,0x00b70000};
	
unsigned int row3[4]={ 0x007e0000,0x007d0000,0x007b0000,0x00770000};

IO1DIR   = 0XFFF0FFFF;	//set rows as output and colomn as input

PINSEL1=0x00000000;
IODIR0=0xf0ff0000; // making po.16 to p0.23  and p0.28 to p0.31 output lines for disp
IOSET0=0XF0000000;
while(1)
{
  IO1PIN=0x00ff0000; //initialize rows and colomns with one
IOCLR1=0x00100000; //enable row0
value=IOPIN1;
delay(50000);  

value=value & 0x00ff0000;
for(i=0; i<4;i++)
{
if(value==row0[i])
{
disp(i);
delay(65000);  delay(65000);delay(65000);delay(65000);delay(65000);

}
}
  IO1PIN=0x00ff0000; //initialize rows and colomns with one
IOCLR1=0x00200000; //enable row0
value=IOPIN1;
delay(50000);	 delay(50000);

value=value & 0x00ff0000;
for(i=0; i<4;i++)
{
if(value==row1[i])
{
disp(i+4);
delay(65000);  delay(65000);delay(65000);delay(65000);delay(65000);
}
}

 IO1PIN=0x00ff0000; //initialize rows and colomns with one
IOCLR1=0x00400000; //enable row0
value=IOPIN1; 
delay(65000);  delay(65000);delay(65000);delay(65000);delay(65000);
value=value & 0x00ff0000;
for(i=0; i<4;i++)
{
if(value==row2[i])
{
disp(i+8);
delay(50000);  

}
}

 IO1PIN=0x00ff0000; //initialize rows and colomns with one
IOCLR1=0x00800000; //enable row0
value=IOPIN1; 
delay(65000);  delay(65000);delay(65000);delay(65000);delay(65000);
value=value & 0x00ff0000;
for(i=0;i<4;i++)
{
if(value==row3[i])
{
disp(i+12);
delay(65000);  delay(65000);delay(65000);delay(65000);delay(65000);
}
}
}


}
void disp(unsigned int temp)
{
unsigned int i;	 
unsigned int da[16]={0x003F0000, 0x00060000, 0x005B0000, 0x004F0000, 0x00660000,0x006D0000,
					   0x007D0000, 0x00070000, 0x007F0000, 0x006F0000, 0x00770000,0x007C0000,
					   0x00390000, 0x005E0000, 0x00790000, 0x00710000 };

IOCLR0=0x00ff0000; //clear disp
i=temp;

IOSET0=da[i];
delay(65000);  delay(65000);delay(65000);delay(65000);delay(65000);
IOCLR0=0X00FF0000;
}




void delay(unsigned int del)
{ unsigned int k;
for(k=0;k<del;k++);
}
