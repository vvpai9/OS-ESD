#include <RTL.h>                      /* RTX kernel functions & defines      */
#include <LPC21xx.h>                  /* LPC21xx definitions                 */
#include <stdio.h>
#include "lcd.h"
 void cmd(unsigned int x)
{
  unsigned int value;
  value = x;
  value = value & 0xF0;
  IOCLR0 = 0xF0;
  IOCLR0 = 0x04; 
  IOSET0 = value;
  IOSET0 = 0x08; 
  delay(100);
  IOCLR0 = 0x08; 
  value = x;
  value = value & 0x0F;
  value = value << 4;
  IOCLR0 = 0xF0;
  IOCLR0 = 0x04; 
  IOSET0 = value;
  IOSET0 = 0x08;
  delay(100);
  IOCLR0 = 0x08; 
}

void data(unsigned int p)
{		
  unsigned int value;
  
  value = p;
  value = value & 0xF0;
  IOCLR0 = 0xF0;
  IOSET0 = 0x04; 
  IOSET0 = value;
  IOSET0 = 0x08; 
  delay(100);
  IOCLR0 = 0x08; 
  value = p;
  value = value & 0x0F;
  value = value << 4;
  IOCLR0 = 0xF0;
  IOSET0 = 0x04; 
  IOSET0 = value;
  IOSET0 = 0x08; 
  delay(100);
  IOCLR0 = 0x08; 
}

void lcd_init ()		 
{
  unsigned int c[] = {0x30,0x30,0x30,0x20,0x28,0x01,0x06,0x0E,0x80};
  unsigned int i;
  for(i=0;i<7;i++)
  { 
    cmd(c[i]);
    delay(10000);
  }
} 


void delay(unsigned  long int x)
{
	unsigned  long int t;
	for(t=0;t<x;t++);

}

