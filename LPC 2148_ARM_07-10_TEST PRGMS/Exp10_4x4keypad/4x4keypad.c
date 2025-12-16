// 4x4 Key Matrix INTERFACING.
//----------------------------------------------------------------
// CONTROLLER 		: LPC-2148  	
// DATE 			: December - 2015
// Developed By 	: Advanced Electronic Systems Bangalore,India
//----------------------------------------------------------------
// Program to demonstrate keyboard operation.
// Takes a key from key pad and displays it on LCD
//----------------------------------------------------------------

#include<lpc21xx.h>
#include<stdio.h>

void scan(void);
void get_key(void);
void display(void);
void delay(unsigned int);
void init_port(void);

void lcd_init(void);   
void clr_disp(void);
void lcd_com(void);		// LCD routines 
void lcd_data(void);
void wr_cn(void);
void wr_dn(void);

unsigned long int scan_code[16]= { 0x00EE0000,0x00ED0000,0x00EB0000,0x00E70000 ,
                                   0x00DE0000,0x00DD0000,0x00DB0000,0x00D70000 ,
                                   0x00BE0000,0x00BD0000,0x00BB0000,0x00B70000 ,
                                   0x007E0000,0x007D0000,0x007B0000,0x00770000 };


unsigned char ASCII_CODE[16]= {'0','1','2','3',
                               '4','5','6','7',
                               '8','9','A','B',
                               'C','D','E','F'};	 

unsigned char  row,col;										  									
unsigned char temp,flag,i,result,temp1;
unsigned int r,r1;
unsigned long int var,var1,var2,res1,temp2,temp3,temp4;
unsigned char *ptr,disp[] = "4X4 KEYPAD";
unsigned char disp0[] = "KEYPAD TESTING";
unsigned char disp1[] = "KEY = ";
int main()
{	
	init_port();            //port intialisation
 	delay(3200);			//delay
	lcd_init();             //lcd intialisation
	delay(3200);			//delay
    clr_disp();				//clear display
	delay(500);             //delay

	ptr = disp;
	temp1 = 0x81;			// Display at starting address
	lcd_com();
	delay(800); 
    
	while(*ptr!='\0')
    {
    	temp1 = *ptr;
        lcd_data();
	    ptr ++;
    } 
	delay(50000);  

  	while(1)	
	{	         
    	get_key();
        display();		
	} 
      
}  //end of main()	

void get_key(void)                //get the key from the keyboard
{
	unsigned int  i;	
	flag = 0x00;
    IO1PIN=0x000f0000;
	        
	while(1)
	{		
    	for(row=0X00;row<0X04;row++)	//Writing one for col's 
        {					
        	if( row == 0X00)
            { 
            	temp3=0x00700000; 
		    }
            else if(row == 0X01)
            { 
            	temp3=0x00B00000;
		    }
		    else if(row == 0X02)
		    {
            	temp3=0x00D00000;
		    }
            else if(row == 0X03)
		    {
            	temp3=0x00E00000;
		    }	
	  	  	var1 = temp3;
          	IO1PIN = var1;		// each time var1 value is put to port1
          	IO1CLR =~var1;		// Once again Conforming (clearing all other bits)

          	scan();
          	delay(100);			//delay

          	if(flag == 0xff) 
          		break;

       	} // end of for loop

		if(flag == 0xff)
			break;
	}  // end of while	
  
    for(i=0;i<16;i++)
    {
    	if(scan_code[i] == res1)      //equate the scan_code with res1
    	{
			result =  ASCII_CODE[i];    //same position value of ascii code
			break;                      //is assigned to result
      	}
    }
}// end of get_key(); 

void scan(void)
{	
    unsigned long int t;
    temp2 = IO1PIN;                             // status of port1
    temp2 = temp2 & 0x000F0000;                 // Verifying column key
    if(temp2 != 0x000F0000)                     // Check for Key Press or Not
    {
    	delay(1000);                              //delay(100)//give debounce delay check again
    	temp2 = IO1PIN; 		
      	temp2 = temp2 & 0x000F0000;              //changed condition is same 

        if(temp2 != 0x000F0000)                // store the value in res1
        {
        	flag = 0xff;
          	res1 = temp2; 
          	t = (temp3 & 0x00F00000);           //Verfying Row Write
          	res1 = res1 | t;                    //final scan value is stored in res1
        }
        else
        {
        	flag = 0x00;
        }
    }
}  // end of scan()
			 	 	 	    	 
void display(void)
{	  
    ptr = disp0;
	temp1 = 0x80;			// Display starting address of first line
	lcd_com();
    
	while(*ptr!='\0')
    {
    	temp1 = *ptr;
      	lcd_data();
	  	ptr ++;
    } 

    ptr = disp1;
	temp1 = 0xC0;			// Display starting address of second line
	lcd_com();
        
	while(*ptr!='\0')
    {
    	temp1 = *ptr;
        lcd_data();
	  	ptr ++;
    }
    temp1 = 0xC6;			//display  address for key value
	lcd_com();
    temp1 = result;    
    lcd_data();	
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

	temp = 0x28;	// load command for lcd function setting with lcd in 4 bit mode,
	lcd_com();	 	// 2 line and 5x7 matrix display
	delay(3200);					
		
	temp1 = 0x0C;	// load a command for display on, cursor on and blinking off		
	lcd_com();
	delay(800);  				
	
	temp1 = 0x06; 	// command for cursor increment after data dump	
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

void wr_dn(void)			////write data reg
{  	  
	IO0CLR  = 0x000000FC;	// clear the port lines.
	IO0SET = temp;			// Assign the value to the PORT lines 
	IO0SET = 0x00000004;   	// set bit  RS = 1 	
	IO0SET = 0x00000008;   	// E=1
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
	
void clr_disp(void)
{
    temp1 = 0x01;  		// command to clear lcd display
    lcd_com();
    delay(500);					
}  

void delay(unsigned int r1)
{
	for(r=0;r<r1;r++);
} 
      										 
void init_port()
{    
    IO0DIR = 0x000080FC;	//configure o/p lines for lcd 
	IO1DIR = 0XFFF0FFFF;

 	IO0CLR =  0X00008000; 			// start low
}
