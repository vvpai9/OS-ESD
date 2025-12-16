/*-----------------------------------------------------------------------------
 *      RL-ARM - RTX
 *-----------------------------------------------------------------------------
 *      Name:    RTX_DIP_SWITCH.c  
 *      Purpose: RTX_DIP_SWITCH using Mailbox concept
 *----------------------------------------------------------------------------*/

#include <RTL.h>                    // RTX kernel functions & defines      
#include <LPC214x.h>                // LPC21xx definitions                 
#include <stdio.h>
#include <string.h>

// tsk1, tsk2 will contain task identifications at run-time
OS_TID tsk1, tsk2; 

os_mbx_declare(MsgBox,1);           // Declare a message pointers as an array of unsigned int 
									// macro defined int  rtl.h -> U32 name [4 + cnt] 
 
// Format of message slot   
typedef struct {                    // Message object structure            
	char str1[17];
	char str2[15];
}T_MEAS;

_declare_box(mpool,sizeof(T_MEAS),1);	// Reserve a block of memory larg enough to hold 16 message slot 
										// macro defined in rtl.h -> U32 pool[(((size)+3)/4)*(cnt) + 3]

// Input dip switches
#define SW1	0x00100000				// P1.20  dip switch pin1
#define SW2	0x00200000				// P1.21  dip switch pin2
#define SW3	0x00400000				// P1.22  dip switch pin3
#define SW4	0x00800000				// P1.23  dip switch pin4

/* Function prototypes */
__task void init_task(void);
__task void send_task(void);
__task void rec_task(void);
void lcd_init(void);
void wr_cn(void);
void clr_disp(void);
void delay(unsigned int);
void lcd_com(void);						   
void wr_dn(void);
void lcd_data(void);
unsigned int dip_sw_read(void);
void DisplayRow(int row, char *str);

unsigned int i8ch, dip_sw;
unsigned char temp1;
unsigned long int temp,r=0,k=0,cnt1=0,cnt2=0;
        
/*-----------------------------------------------------------------------------------
 *   init_task:  RTX Kernel starts this task with os_sys_init_prio(init_task, 10);
 *---------------------------------------------------------------------------------*/
__task void init_task(void) {
	tsk1 = os_tsk_create(send_task,2); 	// send_task at priority 2
	tsk2 = os_tsk_create(rec_task,1); 	// rec_task at priority 1
	os_tsk_delete_self(); 				// must delete itself before exiting
}

/*----------------------------------------------------------------------------
 *  Task 1: RTX Kernel starts this task with os_tsk_create (send_task, 2)
 *---------------------------------------------------------------------------*/
__task void send_task(void) {
	T_MEAS *mptr;

 	os_mbx_init(MsgBox, sizeof(MsgBox));	// initialize the mailbox             
         
	while(1) {
		cnt1++;
		os_dly_wait(1);

		mptr = _alloc_box(mpool);       
	
		i8ch = dip_sw_read();							// Read dip switch SW20
		strcpy(mptr->str1,"DIP SWITCH TEST");
		sprintf(mptr->str2,"DIP SW = %02X",i8ch);		// Convert keycode into ASCII to display it on LCD

		os_mbx_send(MsgBox, mptr, 0xffff); 			// Send the message to the mailbox     
		os_dly_wait(100);
	}
}

/*----------------------------------------------------------------------------
 *  Task 2: RTX Kernel starts this task with os_tsk_create (rec_task, 1)
 *---------------------------------------------------------------------------*/
__task void rec_task(void) {
	T_MEAS *rptr;

	while(1) {
		cnt2++;
    	os_mbx_wait(MsgBox, (void **)&rptr, 0xffff); 	// wait for the message    
 
		temp1 = 0x80;									// Display starting address	of first line 1 th pos
		lcd_com();
	 
		k=0;
    	while(rptr->str1[k]!='\0') {
   			temp1 = rptr->str1[k];
    	 	lcd_data();
			os_dly_wait(1);
		 	k++;
    	} 
		k=0;

		temp1 = 0xC0;									// Display starting address	of first line 1 th pos
		lcd_com();
	 
		k=0;
    	while(rptr->str2[k]!='\0') {
   			temp1 = rptr->str2[k];
    	 	lcd_data();
			os_dly_wait(1);
		 	k++;
    	} 
		k=0;  
     	_free_box(mpool, rptr);        				// free memory allocated for message  
	}										
}

/*----------------------------------------------------------------------------
 *        Main: Initialize and start RTX Kernel
 *---------------------------------------------------------------------------*/
int main(void) {                     	// Program execution starts here       
   	PINSEL1 &= 0x00000000; 				// Configure P0.16 to P0.31 as GPIO
	IO0DIR |= 0x000000FC;				// Configure o/p lines for lcd 
	IO1DIR |= 0X00000000;				// Configure P1.20 to P1.23 as Input
 	
	delay(3200);						// delay
	lcd_init();            	 			// lcd intialisation
	delay(3200);						// delay
	clr_disp();							// clear display
	delay(500);         			    // delay

	_init_box(mpool, sizeof(mpool), sizeof(T_MEAS)); // The block of memory formatted into required 1 mail slots        
 	os_sys_init_prio(init_task, 10);          
}

unsigned dip_sw_read(void)
{
	if((IOPIN1 & SW1)) {				// Check whether DIP SW20 pin1 is turned on or not
		dip_sw = 0X01;					// <01> will dispaly on LCD 
	}
		
	else if((IOPIN1 & SW2)) {			// Check whether DIP SW20 pin2 is turned on or not
		dip_sw = 0X02;					// <02> will dispaly on LCD 
	}
		
	else if((IOPIN1 & SW3)) {			// Check whether DIP SW20 pin3 is turned on or not
		dip_sw = 0X03;					// <03> will dispaly on LCD 
	}
		
	else if((IOPIN1 & SW4)) {			// Check whether DIP SW20 pin4 is turned on or not
		dip_sw = 0X04;					// <04> will dispaly on LCD 
	}
	
	else {								// If all pins of DIP SW20 are off
		dip_sw = 0X00;					// <00> will display on LCD
	}			
	
	return dip_sw;
}

// lcd initialisation routine.
void lcd_init(void) {
	temp = 0x30;		   
	wr_cn();
	delay(3200);  			
		
	temp = 0x30;
	wr_cn();
	delay(3200);  					 
		
	temp = 0x30;
	wr_cn();
	delay(3200);  				

	temp = 0x20;						// change to 4 bit mode from default 8 bit mode
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
	
	temp1 = 0x80;  // set the cursor to beginning of line 1
	lcd_com();
	delay(800);  				
}

void lcd_com(void) {
	temp = temp1 & 0xf0;
    wr_cn();
    temp = temp1 & 0x0f;
    temp = temp << 4; 
    wr_cn();
    delay(500);
}

// command nibble o/p routine
void wr_cn(void) {                //write command reg
	IO0CLR  = 0x000000FC;		// clear the port lines.
	IO0SET 	= temp;				// Assign the value to the PORT lines    
	IO0CLR  = 0x00000004;		// clear bit  RS = 0
	IO0SET 	= 0x00000008;   	// E=1
	delay(10);
	IO0CLR  = 0x00000008;
}
  
// data nibble o/p routine
void wr_dn(void) {				// write data reg
	IO0CLR = 0x000000FC;		// clear the port lines.
	IO0SET = temp;				// Assign the value to the PORT lines 
	IO0SET = 0x00000004;   		// set bit  RS = 1 	
	IO0SET = 0x00000008;   		// E=1
	delay(10);
	IO0CLR = 0x00000008;
}

// data o/p routine which also outputs high nibble first
// and lower nibble next
void lcd_data(void) {
	temp = temp1 & 0xf0;
    temp = temp;
    wr_dn();
    temp= temp1 & 0x0f;	
    temp= temp << 4;
    wr_dn();
    delay(100);					
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
/*----------------------------------------------------------------------------
 * end of file
 *---------------------------------------------------------------------------*/
