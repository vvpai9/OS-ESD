// ADC 0809 INTERFACING
//----------------------------------------------------------------
// CONTROLLER 		: LPC-2148  	
// DATE 			: DECEMBER - 2015
// Developed By 	: Advanced Electronic Systems Bangalore,India
//----------------------------------------------------------------
//----------------------------------------------------------------
// This project is to study ADC 0809 and LM335 temprature sensor
// code is developed to compatible ALS-SDA-ARM7-08 REV00. 
// Implementations done for 2nd  channel(channel 2).
// temprature value in degree C will be displayed on 
// ALS-SDA-ARM7-08 REV00 board lCD.
//----------------------------------------------------------------

#include<lpc21xx.h>
#include"lcd.h"
#include<stdio.h>

#define supply_V	5.2

void delay(void);
void adc_val_disp(unsigned char );

unsigned char lcd_msg1[] = "WELCOME TO ALS" ;
unsigned char lcd_msg2[] = "BANGALORE" ;
unsigned char lcd_msg3[] = "TEMP_SENSOR" ;
unsigned char lcd_msg6[] = "INTERFACING" ;
unsigned char lcd_msg4[] = "TEMP(C)=" ;

	 
float ana_output , adc_out , temp_out ,avg_out = 0.0 ;
unsigned int temp1 = 0 , ana_temp = 0  ;

unsigned long int tmp1 , tmp2 , data_temp  ;
unsigned char  ana_op[5] , first_flg = 0xff ;
float ana_val[10]  ;

int main()
{
	int i , j ;
	
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
	delay() ;					 			// display message lcd_msg2[] = "BANGALORE" ;
	lcd_puts(lcd_msg2);
	delay() ;

	for( i = 0 ; i < 250 ; i++ )			// delay of around 1sec.
		for( j = 0 ; j < 10000 ; j++ );		  	

	clr_disp() ;

	tmp1 = 0x82 ;
	lcd_com() ;
	delay() ;
	lcd_puts(lcd_msg3);		   				// display message lcd_msg3[] =  "ADC_0809" ;
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

	
	while(1)
	{
		IO0CLR = 0X00004000 ; 		// addr2 - 0 		  	// clear address bits	
		IO1CLR = 0X00030000 ;		// addr1 = addr0 = 0 
		IO1SET = 0X00020000 ;		// set addr1 = 1		// Channel 1  = (010)

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
	
		IO1CLR =  0X00040000; 		   // clear output_EN

		ana_temp = data_temp ;

		ana_output = (float)((supply_V/(float)255)*ana_temp); // calculating analog volatge value.

		if(first_flg == 0xff)					   ///////
		{
			for( i = 0 ; i < 10 ; i++ )
			{
				ana_val[i] = ana_output ;
			}									   // taking running sum..
			first_flg = 0x00 ; 	
	   	}
		else 
		{
			for( i = 8 ; i >= 0 ; i-- )
			{
				ana_val[i+1] = ana_val[i] ;
			}	
			ana_val[i+1] = ana_output ;			 ///////
		}

		avg_out = 0.0 ;
		for(i = 0 ; i < 10 ; i++ )
		{
			avg_out += ana_val[i] ;
		}	

		adc_out = (float)( avg_out / 10.0 );
		adc_out = (adc_out - 2.7315);
		temp_out = adc_out * 100;			// calculate temparature value
		sprintf(ana_op,"%.2f",temp_out);	

		tmp1 = 0x88 ;
		lcd_com() ;
		delay() ;	

		lcd_puts(ana_op);  		// display temparature value.

	   	for( i = 0 ; i < 100 ; i++ )	
			for( j = 0 ; j < 10000 ; j++ )	;
	}	   	
}

void delay()
{
	unsigned int i;
	for(i=0;i<=200;i++) ;
}
