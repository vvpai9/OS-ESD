#include<LPC21xx.h>

void delay(void);
void serial(void);

unsigned char mg;

int main()
{
  unsigned int i;
  unsigned char msg[]={"BVB"};		
  serial();
 
   while(1)
  {
   for(i=0;i<3;i++)
    {
	while(!(U0LSR & 0x20));		
        U0THR = msg[i];
    }
   while(!(U0LSR & 0x01));

   mg=U0RBR;
   U0THR=mg;
  

  delay();
  }

}

void serial()
{
  PINSEL0 = 0x00000005; 			
  U0LCR = 0x83; 					
  U0DLL = 0x61; 					
  U0LCR = 0x03; 					
  U0IER = 0x01;						
}



void delay()
{
  unsigned int i;
  for(i=0;i<10000;i++);
} 



