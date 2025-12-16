/*-----------------------------------------------------------------------------
 *      RL-ARM - RTX
 *-----------------------------------------------------------------------------
 *      Name:    RTX_4x4KEY_MAILBOX.c  
 *      Purpose: Implementing 4x4KEY-MATRIX using Mailbox concept
 *----------------------------------------------------------------------------*/

#include <RTL.h>                      		// RTX kernel functions & defines      
#include <LPC214x.h>                  		// LPC21xx definitions                 
#include <stdio.h>
#include <string.h>

// tsk1, tsk2 will contain task identifications at run-time
OS_TID tsk1, tsk2;  

os_mbx_declare(MsgBox,1);           		// Declare a message pointers as an array of unsigned int 
											// macro defined int  rtl.h -> U32 name [4 + cnt] 

typedef struct {                      		// Format of each message slot          
	char str1[17];
	char str2[15];
}T_MEAS;
          
_declare_box (mpool,sizeof(T_MEAS),1);		// Reserve a block of memory larg enough to hold 16 message slot 
											// macro defined in rtl.h -> U32 pool[(((size)+3)/4)*(cnt) + 3]
/* Function prototypes */
__task void init_task(void);
__task void send_task (void);
__task void rec_task (void);
void lcd_init(void);
void clr_disp(void);
void lcd_com(void);
void lcd_data(void);
void wr_cn(void);
void wr_dn(void);
void scan(void);
void get_key(void);
void display(void);
void delay(unsigned int);
void init_port(void);

unsigned long int scan_code[16] = {0x00EE0000,0x00ED0000,0x00EB0000,0x00E70000,
                                  0x00DE0000,0x00DD0000,0x00DB0000,0x00D70000,
                                  0x00BE0000,0x00BD0000,0x00BB0000,0x00B70000,
                                  0x007E0000,0x007D0000,0x007B0000,0x00770000};

unsigned char KEYCODE[16] = {0x00,0x01,0x02,0x03,
                               0x04,0x05,0x06,0x07,
                               0x08,0x09,0x0A,0x0B,
                               0x0C,0x0D,0x0E,0x0F};	 

unsigned char row=0, col=0, i8ch=0;										  									
unsigned char temp=0, flag=0, i=0, result=0, temp1=0, test=0;
unsigned int r=0, r1=0, cnt1=0, cnt2=0, k=0;
unsigned long int var=0, var1=0, var2=0, res1=0, temp2=0, temp3=0, temp4=0;

/*-----------------------------------------------------------------------------------
 *   init_task:  RTX Kernel starts this task with os_sys_init_prio(init_task, 10);
 *---------------------------------------------------------------------------------*/
__task void init_task(void) {
	tsk1 = os_tsk_create(send_task,2); 	// send_task at priority 2 
	tsk2 = os_tsk_create(rec_task,1); 	// rec_task at priority 1
	os_tsk_delete_self(); 				// must delete itself before exiting
}

/*----------------------------------------------------------------------------
 *  Task 1:  RTX Kernel starts this task with os_tsk_create (send_task, 2)
 *---------------------------------------------------------------------------*/
__task void send_task (void) {
	T_MEAS *mptr;
    
  	os_mbx_init (MsgBox, sizeof(MsgBox));			// initialize the mailbox             
       
	while(1) {
		cnt1++;
		os_dly_wait (1);
		mptr = _alloc_box (mpool);              	// Allocate a memory for the message   
		if(test==0)	{
			strcpy(mptr->str1,"4x4 KEYPAD TEST");
			sprintf(mptr->str2,"WAITING      ");	// Convert keycode into ASCII to display it on LCD
			test=0xff;
		}
		
		else {
			get_key();								// Read Keyboard
			strcpy(mptr->str1,"4x4 KEYPAD TEST");
			sprintf(mptr->str2,"KEY = %02X   ",result);	// Convert keycode into ASCII to display it on LCD
			test=0;
		}
		
		os_mbx_send (MsgBox, mptr, 0xffff); 		// Send the message to the mailbox     
		os_dly_wait (100);
	}
}

/*----------------------------------------------------------------------------
 *  Task 2: RTX Kernel starts this task with os_tsk_create (rec_task, 1)
 *---------------------------------------------------------------------------*/
__task void rec_task (void) {
	T_MEAS *rptr;

	while(1) {
		cnt2++;
    	os_mbx_wait (MsgBox, (void **)&rptr, 0xffff); // wait for the message    
 	
		temp1 = 0x80;					// Display starting address	of first line 1 th pos
		lcd_com();
	 
		k=0;
    	while(rptr->str1[k]!='\0') {
   			temp1 = rptr->str1[k];
    	 	lcd_data();
			os_dly_wait (1);
		 	k++;
    	} 
		k=0;

		temp1 = 0xC0;					// Display starting address	of first line 1 th pos
		lcd_com();
	 
		k=0;
    	while(rptr->str2[k]!='\0') {
   			temp1 = rptr->str2[k];
    	 	lcd_data();
			os_dly_wait (1);
		 	k++;
    	} 
		k=0;  
     	_free_box (mpool, rptr);        // free memory allocated for message  
	}										
}

/*----------------------------------------------------------------------------
 *        Main: Initialize and start RTX Kernel
 *---------------------------------------------------------------------------*/
int main (void) { 
	init_port();            			// port intialisation
 	delay(3200);						// delay
	lcd_init();             			// lcd intialisation
	delay(3200);						// delay
    clr_disp();							// clear display
	delay(500);             			// delay

	_init_box (mpool, sizeof(mpool),    // The block of memory formatted into required 16 mail slots      
              sizeof(T_MEAS));        	   
 	os_sys_init_prio(init_task, 10);     
}

void get_key(void) {               			// get the key from the keyboard
	unsigned int  i;	
	flag = 0x00;
    IO1PIN=0x000f0000;        
	while(1) {		
    	for(row=0X00;row<0X04;row++) {		// Writing one for col's 
           	if( row == 0X00) { 
            	temp3=0x00700000; 
		    }
            
			else if(row == 0X01) { 
            	temp3=0x00B00000;
		    }
		    
			else if(row == 0X02) {
            	temp3=0x00D00000;
		    }
            
			else if(row == 0X03) {
            	temp3=0x00E00000;
		    }	
	  	  	
			var1 = temp3;
          	IO1PIN = var1;					// each time var1 value is put to port1
          	IO1CLR =~var1;					// Once again Conforming (clearing all other bits)
          	scan();
          	delay(100);						// delay
          	if(flag == 0xff) 
          	break;
       	} // end of for
		if(flag == 0xff)
			break;
	}  // end of while	
  
    for(i=0;i<16;i++) {
    	if(scan_code[i] == res1) {     		// equate the scan_code with res1
    		result =  KEYCODE[i];    		// same position value of ascii code
			break;                      	// is assigned to result
      	}
    }
}// end of get_key(); 

void scan(void) {	
    unsigned long int t;
    temp2 = IO1PIN;                     	// status of port1
    temp2 = temp2 & 0x000F0000;         	// Verifying column key
    if(temp2 != 0x000F0000) {            	// Check for Key Press or Not
       	delay(1000);                        // delay(100)//give debounce delay check again
    	temp2 = IO1PIN; 		
      	temp2 = temp2 & 0x000F0000;         // changed condition is same 

        if(temp2 != 0x000F0000) {             // store the value in res1
          	flag = 0xff;
          	res1 = temp2; 
          	t = (temp3 & 0x00F00000);       // Verfying Row Write
          	res1 = res1 | t;                // final scan value is stored in res1
        }
        else {
        	flag = 0x00;
        }
    }
}  // end of scan()

void lcd_init (void) {
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

void lcd_data(void)	{   	         
    temp = temp1 & 0xf0;
    wr_dn();
    temp= temp1 & 0x0f;	
    temp= temp << 4;
    wr_dn();
    delay(100);					
} 

void wr_dn(void) {				// write data reg
	IO0CLR  = 0x000000FC;		// clear the port lines.
	IO0SET = temp;				// Assign the value to the PORT lines 
	IO0SET = 0x00000004;   		// set bit  RS = 1 	
	IO0SET = 0x00000008;   		// E=1
	delay(10);
	IO0CLR = 0x00000008;
}

void lcd_com(void) {
    temp = temp1 & 0xf0;
    wr_cn();
    temp = temp1 & 0x0f;
    temp = temp << 4; 
    wr_cn();
    delay(500);
}

void wr_cn(void) {               // write command reg
	IO0CLR  = 0x000000FC;		// clear the port lines.
	IO0SET 	= temp;				// Assign the value to the PORT lines    
	IO0CLR  = 0x00000004;		// clear bit  RS = 0
  	IO0SET 	= 0x00000008;   	// E=1
	delay(10);
	IO0CLR  = 0x00000008;
}
	
void clr_disp(void) {
// command to clear lcd display
    temp1 = 0x01;
    lcd_com();
    delay(500);					
}  

void delay(unsigned int r1) {
	for(r=0;r<r1;r++);
} 
      										 
void init_port() {    
    IO0DIR = 0x000080FC;		// configure o/p lines for lcd 
	IO1DIR = 0XFFF0FFFF;
	IO0CLR =  0X00008000; 		// in adc0809 (P0.15) start low
}
/*----------------------------------------------------------------------------
 * end of file
 *---------------------------------------------------------------------------*/
