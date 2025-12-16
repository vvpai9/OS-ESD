#include <LPC21xx.h>
#include <stdio.h>

// Input dip switches
// #define SW1	0x00100000			// P1.20		// made dummy
#define SW2	0x00200000				// P1.21
#define SW3	0x00400000				// P1.22
#define SW4	0x00800000				// P1.23
#define NO_DIP_SWITCH 0x00000000	// no dip switch is turned on

// Function prototypes
void lcd_init(void);
void wr_cn(void);
void clr_disp(void);
void delay(unsigned int);
void lcd_com(void);						   
void wr_dn(void);
void lcd_data(void);
int KBD_rdkbd(void);
void DisplayRow (int row, char *str);

unsigned int KBD_i32keydown;
int i8ch ;
char szTemp[16] ;
unsigned char temp1;
unsigned long int temp,r=0;

int main (void)
{
	IO0DIR = 0x000000FC;				//configure o/p lines for lcd 
	IO1DIR = 0X00000000;
 	
	delay(3200);						//delay
	lcd_init();            	 			//lcd intialisation
	delay(3200);						//delay
	clr_disp();							//clear display
	delay(500);         			    //delay

	DisplayRow(1,"DIP SW INTERFACE");		// Display message on 1st line of LCD 

	while(1)
	{
		i8ch = KBD_rdkbd() ;					// Read Keyboard
		sprintf(szTemp,"KeyCode = %02X",i8ch);	// Convert keycode into ASCII to display it on LCD
		DisplayRow(2,szTemp) ;					// Display keycode on 2nd line of LCD
	}	
}

int KBD_rdkbd(void)
{
/*	if((IOPIN1 & SW1)) {			// made dummy	// Check whether SW1 is pressed or not
		KBD_i32keydown = 0X01;						// '1' will dispaly on LCD if SW1 pressed
	} */
		
	if((IOPIN1 & SW2)) {						// Check whether SW2 is pressed or not
		KBD_i32keydown = 0X01;					// '1' will dispaly on LCD if SW1 pressed
	}
		
	else if((IOPIN1 & SW3)) {					// Check whether SW3 is pressed or not
		KBD_i32keydown = 0X02;					// '2' will dispaly on LCD if SW1 pressed
	}
		
	else if((IOPIN1 & SW4)) {					// Check whether SW4 is pressed or not
		KBD_i32keydown = 0X03;					// '3' will dispaly on LCD if SW1 pressed
	}
	
	else {
		KBD_i32keydown = 0X00;
	}			
	
	return KBD_i32keydown;
}

void lcd_init (void)
{
	temp = 0x30;		   
	wr_cn();
	delay(3200);			
		
	temp = 0x30;
	wr_cn();
	delay(3200);						 
		
	temp = 0x30;
	wr_cn();
	delay(3200);	 				

	temp = 0x20;
	wr_cn();
	delay(3200);	 				

// load command for lcd function setting with lcd in 4 bit mode,
// 2 line and 5x7 matrix display

	temp = 0x28;
	lcd_com();
	delay(3200);					

// load a command for display on, cursor on and blinking off		
	temp1 = 0x0C;		
	lcd_com();
	delay(800);  				
	
// command for cursor increment after data dump	
	temp1 = 0x06;
	lcd_com();
	delay(800);  				
	
	temp1 = 0x80;
	lcd_com();
	delay(800);  		
}

void lcd_data(void)
{   	         
	temp = temp1 & 0xf0;
    wr_dn();
    temp= temp1 & 0x0f;	
    temp= temp << 4;
    wr_dn();
    delay(100);					
} 

void wr_dn(void)					// write data reg
{  	  
	IO0CLR  = 0x000000FC;			// clear the port lines.
	IO0SET = temp;					// Assign the value to the PORT lines 
	IO0SET = 0x00000004;   			// set bit  RS = 1 	
	IO0SET = 0x00000008;   			// E=1
	delay(10);
	IO0CLR = 0x00000008;
}

void lcd_com(void)
{
	temp = temp1 & 0xf0;
    wr_cn();
    temp = temp1 & 0x0f;
    temp = temp << 4; 
    wr_cn();
    delay(500);
}

void wr_cn(void)                //write command reg
{ 
	IO0CLR  = 0x000000FC;		// clear the port lines.
	IO0SET 	= temp;				// Assign the value to the PORT lines    
	IO0CLR  = 0x00000004;		// clear bit  RS = 0
  	IO0SET 	= 0x00000008;   	// E=1
	delay(10);
	IO0CLR  = 0x00000008;
}

void DisplayRow (int row, char *str)
{
/*  pass pointer to 16 character string
	displayes the message on line1 or line2 of LCD, depending on whether row is 1 or 2.
*/
	int k ;

	if (row == 1) 
	{
		temp1=0x80;
		lcd_com() ;	
	}
	else 
	{
		temp1=0xC0;
		lcd_com();
	}

	for(k = 0 ; k < 16 ; k ++)
	{
		if (str[k])
		{
			temp1=str[k];
			lcd_data();	
		}				//	LcdDat(str[k]);
		else
			break ;
	}
	while(k < 16)
	{
		temp1= ' ';
		lcd_data() ;
		k++ ;
	}
}	 
 
void clr_disp(void)
{
// command to clear lcd display
    temp1 = 0x01;
    lcd_com();
    delay(500);					
}  

void delay(unsigned int r1)
{
	for(r=0;r<r1;r++);
} 

