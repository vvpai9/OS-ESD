/*------------------------------------------------------------------------------
 *      RL-ARM - RTX
 *------------------------------------------------------------------------------
 *      Name	:	RTX_LCD_MAILBOX.c  
 *      Purpose	:	Mailbox concept 
 *----------------------------------------------------------------------------*/

#include <RTL.h>                      	// RTX kernel functions & defines      
#include <LPC214x.h>                  	// LPC21xx definitions                 
#include <stdio.h>
#include <string.h>

// tsk1, tsk2 will contain task identifications at run-time
OS_TID tsk1, tsk2;  

os_mbx_declare (MsgBox,1);           	// Declare a message pointers as an array of unsigned int 
										// macro defined int  rtl.h -> U32 name [4 + cnt] 
// Format of each message slot 
typedef struct {                      	// Message object structure            
	char str1[17];
	char str2[15];
}T_MEAS;

_declare_box (mpool,sizeof(T_MEAS),1); 	// Reserve a block of memory larg enough to hold 16 message slot 
										// macro defined in rtl.h -> U32 pool[(((size)+3)/4)*(cnt) + 3]
unsigned char temp1,test=0;
unsigned long int temp=0,r=0;
unsigned int cnt1=0,cnt2=0,k=0;

//Function prototypes
__task void init_task(void);
__task void send_task (void);
__task void rec_task (void);
void lcd_init(void);
void wr_cn(void);
void clr_disp(void);
void delay(unsigned int);
void lcd_com(void);						   
void wr_dn(void);
void lcd_data(void);

/*-----------------------------------------------------------------------------------
 *   init_task:  RTX Kernel starts this task with os_sys_init_prio(init_task, 10);
 *---------------------------------------------------------------------------------*/
__task void init_task(void) {
	tsk1 = os_tsk_create(send_task,2); 	// send_task at priority 2 
	tsk2 = os_tsk_create(rec_task,1); 	// rec_task at priority 1
	os_tsk_delete_self(); 			// must delete itself before exiting
}

/*----------------------------------------------------------------------------
 *  Task 1:  RTX Kernel starts this task with os_tsk_create(send_task, 2)
 *---------------------------------------------------------------------------*/
__task void send_task (void) {
	T_MEAS *mptr;
               
  	os_mbx_init (MsgBox, sizeof(MsgBox));		// initialize the mailbox             
    
	while(1){     
		cnt1++;
		os_dly_wait (1);
		mptr = _alloc_box (mpool);              // Allocate a memory for the message 
		if(test==0)	{
			strcpy(mptr->str1,"ALS,R&D SECTION,");
			strcpy(mptr->str2,"BENGALURU-58   ,");
			test=0xff;
		}

		else {
			strcpy(mptr->str1,"BENGALURU-58   ,");
			strcpy(mptr->str2,"ALS,R&D SECTION,");
			test=0;
		}

		os_mbx_send (MsgBox, mptr, 0xffff); 	// Send the message to the mailbox     
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
 
		temp1 = 0x80;						// Display starting address	of first line 1 th pos
		lcd_com();
	 
		k=0;
    	while(rptr->str1[k]!='\0') 	{
   			temp1 = rptr->str1[k];
    	 	lcd_data();
			os_dly_wait (1);
		 	k++;
    	} 
		k=0;

		temp1 = 0xC0;						// Display starting address	of first line 1 th pos
		lcd_com();
	 
		k=0;
    	while(rptr->str2[k]!='\0') {
   			temp1 = rptr->str2[k];
    	 	lcd_data();
			os_dly_wait (1);
		 	k++;
    	} 
		k=0;
     	_free_box (mpool, rptr);           	// free memory allocated for message  
	}										
}

/*----------------------------------------------------------------------------
 *        Main: Initialize and start RTX Kernel
 *---------------------------------------------------------------------------*/
int main (void) {                     		// program execution starts here       
   	IO0DIR = 0x000000FC;					//configure o/p lines for lcd 
 	IO0PIN = 0X00000000;

	delay(3200);							//delay
	lcd_init();             				//lcd intialisation
	delay(3200);							//delay
    clr_disp();								//clear display
	delay(3200);            				//delay

	_init_box (mpool, sizeof(mpool),    	// The block of memory formatted into required 1 mail slots   
              sizeof(T_MEAS));        		    
 	os_sys_init_prio(init_task, 10);          
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

	temp = 0x20;	// change to 4 bit mode from default 8 bit mode
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
void wr_cn(void) {               //write command reg
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
    temp = temp ;
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
