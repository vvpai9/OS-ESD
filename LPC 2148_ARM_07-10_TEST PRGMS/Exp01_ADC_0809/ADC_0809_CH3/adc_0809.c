// ADC 0809 INTERFACING
//--------------------------------------------------------------
// CONTROLLER 		: LPC-2148  	
// DATE 			: December - 2015
// Developed By 	: Advanced Electronic Systems Bangalore,India
//----------------------------------------------------------------
//----------------------------------------------------------
// This project is to study ADC 0809, code is developed to 
// compatible  ALS-SDA-ARM7-08 REV00. 
// implementation done for channel 3
//----------------------------------------------------------

#include<lpc21xx.h>
#include"lcd.h"
#include<stdio.h>

#define VREF 		5.0
#define FULLSCALE 	0xFF

void delay(void);
void adc_val_disp(unsigned char );

unsigned char lcd_msg1[] = "WELCOME TO ALS" ;
unsigned char lcd_msg2[] = "BANGALORE" ;
unsigned char lcd_msg3[] = "ADC_0809" ;
unsigned char lcd_msg6[] = "INTERFACING" ;
unsigned char lcd_msg4[] = "DIGITL O/P=" ;
unsigned char lcd_msg5[] = "ANALOG I/P=" ;

	 
float ana_output ;
unsigned int temp1 = 0 , ana_temp = 0 ;

unsigned long int tmp1 , tmp2 , data_temp  ;
unsigned char data = 0x00 , ana_op[5] ;

int main()
{
	unsigned int i , j ;

	PINSEL0 = 0X00000000 ; 		// all p0.0 to p0.15  made as GPIO
	PINSEL1 = 0X00000000 ;		// all p0.15 to p0.31 made as GPIO

	IO0DIR =  0X0000C000 ;		// 	P0.14(adr2) AND P0.15(start) MADE OUTPUTS , PO.16 TO P0.23 MADE AS INPUTS.
	IO1DIR =  0X00070000 ;		// 	P1.16(adr0),P1.17(adr1),P1.18(out_en)  MADE OUTPUTS & p1.19(EOC) made input
								
	lcd_init();
	for( i = 0 ; i < 2500 ; i++ );
	
	tmp1 = 0x80 ;
	lcd_com() ;
	delay() ;
	lcd_puts(lcd_msg1);			 	// display message  lcd_msg1[] = "WELCOME TO ALS" ;
	delay() ;

	tmp1 = 0xC2 ;
	lcd_com() ;
	delay() ;					 	// display message lcd_msg2[] = "BANGALORE" ;
	lcd_puts(lcd_msg2);
	delay() ;

	for( i = 0 ; i < 250 ; i++ )			// delay of around 1sec.
		for( j = 0 ; j < 10000 ; j++ );		  	

	clr_disp() ;

	tmp1 = 0x84 ;
	lcd_com() ;
	delay() ;
	lcd_puts(lcd_msg3);		   			// display message lcd_msg3[] =  "ADC_0809" ;
	delay() ;

	tmp1 = 0xC2 ;
	lcd_com() ;
	delay() ;
	lcd_puts(lcd_msg6);		  			 	// display message lcd_msg6[] = "INTERFACING" ;
	delay() ;

	for( i = 0 ; i < 250 ; i++ )			 // delay of around 1sec.
		for( j = 0 ; j < 10000 ; j++ )	;
	

	clr_disp() ;

	tmp1 = 0x80 ;
	lcd_com() ;
	delay() ;						// display message lcd_msg4[] = "DIGITL O/P=" ;
	lcd_puts(lcd_msg4);
	delay() ;

	tmp1 = 0xC0 ;
	lcd_com() ;					   // display message lcd_msg5[] = "ANALOG I/P=" ;
	delay() ;
	lcd_puts(lcd_msg5);
	delay() ;
	
	while(1)
	{
	
		IO0CLR = 0X00004000 ; 		// addr2 - 0 		  	 // clear all addrs bits
		IO1CLR = 0X00030000 ;		// addr1 = addr0 = 0 
		IO1SET = 0X00030000 ; 		// set addr0 = addr1 =  1 // FOR  CHANNEL 3  = (011) 

		IO0SET = 0X00008000;  			// start high
     	for( i = 0 ; i < 100 ; i++ ); 	 				 // start conversion.
 		IO0CLR =  0X00008000; 			// start low

		do
		{
			temp1=IO1PIN;
        	temp1 &= 0x00080000;		// wait till EOC line goes high ,it indicates end of conversion.
		} while(temp1 == 0x00000000);  	

		IO1SET = 0X00040000;  			// output enable made high
   		for( i = 0 ; i < 100 ; i++ ); 		// Delay
 
		data_temp = IO0PIN ;		   // read data
		data_temp &= 0x00ff0000 ;	   // extract 8 bit
		data_temp >>= 16 ; 
		data = data_temp ;			   // get the 16 bit adc value

		IO1CLR =  0X00040000; 		   // clear output_EN

		tmp1 = 0x8B ;
		lcd_com() ;
		delay() ;	

		adc_val_disp(data);	   			// display digital data

		ana_temp = data_temp ;

		ana_output = (float)((5.05/(float)256)*ana_temp);

		sprintf(ana_op ,"%.2f" ,ana_output);

		tmp1 = 0xCB ;
		lcd_com() ;
		delay() ;	

		lcd_puts(ana_op);	   	// display analog voltage.

		for( i = 0 ; i < 50000 ; i++ );

	}
}


void delay()
{
	unsigned int i;
	for(i=0;i<=200;i++) ;
}


void adc_val_disp(unsigned char disp)
{
	unsigned char temp = 0x00 ;

	temp = (disp & 0xf0) >> 4 ;

	if(temp <= 9 )
		tmp1 = temp + 0x30 ;
	else
		tmp1 = temp + 0x37 ;

	lcd_data();	

	temp = disp & 0x0f ;

	if(temp <= 9 )
		tmp1 = temp + 0x30 ;
	else
		tmp1 = temp + 0x37 ;

	lcd_data();	
}

