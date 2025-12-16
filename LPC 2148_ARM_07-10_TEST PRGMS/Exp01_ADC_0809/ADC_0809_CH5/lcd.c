#include <lpc21xx.h>

#include "lcd.h"

void wr_cn(void);
void wr_dn(void);


void lcd_init()
{
	IO0DIR |= 0x000000fc;  // ( port line p0.2,p0.3 , p0.4 to p0.7 are made o/p)
	                                        //lcd RS , EN , D4 to D7 
	IO0CLR = 0x000000fc;  // clear the LCD lines
	 
	tmp2=0x30;		   
	wr_cn();	   
	delay_lcd(30000); 
		
	tmp2=0x30;
	wr_cn();
	delay_lcd(30000);	 
		
	tmp2=0x30;
	wr_cn();
	delay_lcd(30000);

	tmp2=0x20;
	wr_cn();
	delay_lcd(30000);

	tmp1 = 0x28;
	lcd_com();
	delay_lcd(30000);
		
	tmp1 = 0x0c;		
	lcd_com();
	delay_lcd(800);
	
	tmp1 = 0x06;
	lcd_com();
	delay_lcd(800);

	tmp1 = 0x01;
	lcd_com();
 	delay_lcd(10000);
	
	tmp1 = 0x80;
	lcd_com();
	delay_lcd(800);
}

void lcd_com(void)
{
	tmp2= tmp1 & 0xf0;
	wr_cn();
	tmp2 = tmp1 & 0x0f;
	tmp2 = tmp2 << 4; 
	wr_cn();
    delay_lcd(1000);
}

// command nibble o/p routine
void wr_cn(void)                        //write command reg
{ 	 
	IO0CLR  = 0x000000fc;		// clear the port lines.
    //tmp2 = tmp2 << 4;
	IO0SET 	= tmp2;			// Assign the value to the PORT lines    
	IO0CLR  = 0x00000004;		// clear bit  RS = 0
    //IO0CLR	= 0x00010000;    	// R/W = 0	 not used 
	IO0SET 	= 0x00000008;   	// EN=1
	delay_lcd(25);
	IO0CLR  = 0x00000008;		 // EN =0

 }

// data o/p routine which also outputs high nibble first
// and lower nibble next
 void lcd_data(void)
{  
    //tmp1 = data_lcd;             
    tmp2 = tmp1 & 0xf0;
    //tmp2 = tmp2 << 6;
    wr_dn();
    tmp2= tmp1 & 0x0f;	
    tmp2= tmp2 << 4;
    wr_dn();
    delay_lcd(1000);					
} 

// data nibble o/p routine
void wr_dn(void)
{  	  
	IO0CLR = 0x000000fc;	// clear the port lines.
	IO0SET = tmp2;			// Assign the value to the PORT lines 
	IO0SET = 0x00000004;   	// set bit  RS = 1 	
	//IO0CLR = 0x00010000;  	// R/W=0
	IO0SET = 0x00000008;   	// EN=1
	delay_lcd(25);
	IO0CLR = 0x00000008;
 }



void delay_lcd(unsigned int r1)
{
  	unsigned int r;
  	for(r=0;r<r1;r++);
}


void clr_disp(void)
{
	tmp1 = 0x01;
	lcd_com();
 	delay_lcd(10000);
}


void lcd_puts(unsigned char *lcd_buf)
{
	unsigned int i = 0 ;
    for( i = 0 ; lcd_buf[i]!='\0' ; i++ )
    {
         tmp1 = lcd_buf[i];
     	 lcd_data();
    }
}

