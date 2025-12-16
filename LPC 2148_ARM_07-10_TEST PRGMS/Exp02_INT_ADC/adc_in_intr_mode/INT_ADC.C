// INTERNAL ADC INTERFACING(Interrupt mode).
//----------------------------------------------------------------
// CONTROLLER 		: LPC-2148  	
// DATE 			: December - 2015
// Developed By 	: Advanced Electronic Systems Bangalore,India
//----------------------------------------------------------------
// 10-bit internal ADC , AIN0 pin is selected.
// We can change the channel by changing PINSEL1 and ADCR value.
//----------------------------------------------------------------

#include <lpc214x.h>
#include <Stdio.h> 

#define vol 3.3   				// Reference voltage
#define fullscale 0x3ff         // 10 bit adc

void lcd_init(void);
void wr_cn(void);
void clr_disp(void);
void delay(unsigned int);
void lcd_com(void);						   
void wr_dn(void);
void lcd_data(void);

void adc_int_ISR (void) __irq ;

unsigned int data_lcd=0;
unsigned  int adc_value=0,dummy_read ,temp_adc=0,temp1,temp2;
float temp;
char var[15],var1[15],adc_int_flg = 0;
char *ptr,arr[]= "ADC O/P= ";
char *ptr1,dis[]="A I/P  = ";

int main()
{					   
	PINSEL1 = 0X00040000;    		   	//AD0.4 pin is selected(P0.25)
    IO0DIR = 0x000000FC;      			//configure o/p lines for lcd  
	IO1DIR |=0X02000000;				// for LED
	
	VICVectAddr1 = (unsigned long) adc_int_ISR;   
    VICVectCntl1 = 0x20 | 18;   	   	//Assign the VIC channel ADC0 to interrupt priority 1
    VICIntEnable |= 0x00040000;	    	//Enable the ADC0 interrupt
	
	delay(3200);
	lcd_init();	 						//LCD initialization  
    delay(3200);
	clr_disp();	 						//clear display
	delay(3200);					

	ptr = dis;
	temp1 = 0x80;						// Display starting address	of first line 1 th pos
	lcd_com();
	
    while(*ptr!='\0')
    {
   		temp1 = *ptr;
    	lcd_data();
		ptr ++;
	} 

	ptr1 = arr;
	temp1 = 0xC0;						// Display starting address of second line 4 th pos
	lcd_com();

    while(*ptr1!='\0')
    {
    	temp1 = *ptr1;
    	lcd_data();
		ptr1 ++;
	}

	adc_int_flg = 0x0;
	AD0INTEN =0x00000010;   			// enable interrupt
    
	while(1)
    {  	
		IO1SET = 0x02000000 ;			// led 
   		//command register for ADC-AD0.4
		AD0CR = 0x01200010;
		  
	    while(adc_int_flg == 0x00);	// wait for conversion
		adc_int_flg = 0x0;
	
		adc_value >>=6;
		adc_value &= 0x000003ff;
		temp = ((float)adc_value * (float)vol)/(float)fullscale;
		sprintf(var1,"%4.2fV",temp); 	// analog value
		sprintf(var,"%3x",adc_value); 	// digital value
			 
	 	temp1 = 0x89;
	 	lcd_com();
	 	 
	 	ptr = var1;
        while(*ptr!='\0')
        {	        
  			temp1=*ptr;
			lcd_data();	
            ptr++;
		} 
                 
        temp1 = 0xc9;
        lcd_com();
               
		ptr1 = var;
        while(*ptr1!='\0')
        {	 	
        	temp1=*ptr1;
			lcd_data();
            ptr1++;
		}       
	}	 // end of while(1)		
} //end of main()
  
//lcd initialization
void lcd_init()
{
	temp2=0x30;		   
	wr_cn();
	delay(800); 
		
	temp2=0x30;
	wr_cn();
	delay(800);	 
		
	temp2=0x30;
	wr_cn();
	delay(800);

	temp2=0x20;
	wr_cn();
	delay(800);

	temp1 = 0x28;
	lcd_com();
	delay(800);
		
	temp1 = 0x0c;		
	lcd_com();
	delay(800);
	
	temp1 = 0x06;
	lcd_com();
	delay(800);
	
	temp1 = 0x80;
	lcd_com();
	delay(800);
}

void lcd_com(void)
{
	temp2= temp1 & 0xf0;
	wr_cn();
	temp2 = temp1 & 0x0f;
	temp2 = temp2 << 4; 
	wr_cn();
    delay(500);
}

// command nibble o/p routine
void wr_cn(void)                        //write command reg
{ 	 
	IO0CLR  = 0x000000FC;				// clear the port lines.
	IO0SET 	= temp2;					// Assign the value to the PORT lines    
	IO0CLR  = 0x00000004;				// clear bit  RS = 0
	IO0SET 	= 0x00000008;   			// E=1
	delay(10);
	IO0CLR  = 0x00000008;
}

// data nibble o/p routine
void wr_dn(void)
{  	  
	IO0CLR  = 0x000000FC;				// clear the port lines.
	IO0SET = temp2;						// Assign the value to the PORT lines 
	IO0SET = 0x00000004;   				// set bit  RS = 1 	
	IO0SET = 0x00000008;   				// E=1
	delay(10);
	IO0CLR = 0x00000008;
 }

// data o/p routine which also outputs high nibble first
// and lower nibble next
void lcd_data(void)
{  
	temp2 = temp1 & 0xf0;
    wr_dn();
    temp2= temp1 & 0x0f;	
    temp2= temp2 << 4;
    wr_dn();
    delay(100);					
} 

void delay(unsigned int r1)
{
  	unsigned int r;
  	for(r=0;r<r1;r++);
}

void clr_disp(void)
{
	temp1 = 0x01;
	lcd_com();
 	delay(500);
}

void adc_int_ISR (void) __irq
{
	adc_int_flg = 0xff;
	IO1CLR = 0x02000000 ; 					//led OFF	 // for debug
	  
	adc_value = AD0GDR;          			//reading the ADC value
	dummy_read = AD0DR4;	 				// very important - interrupt does not get reset otherwise
	VICVectAddr=0;
}

