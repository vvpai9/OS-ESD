/*------------------------------------------------------------------------------
 *      RL-ARM - RTX
 *------------------------------------------------------------------------------
 *      Name	:	RTX_ADC0809_MAILBOX.c  
 *      Purpose	:	ADC0809 implementation using Mailbox 
 *----------------------------------------------------------------------------*/

#include <RTL.h>                      	// RTX kernel functions & defines      
#include <LPC214x.h>                  	// LPC21xx definitions                 
#include <stdio.h>
#include <string.h>

// tsk1, tsk2 will contain task identifications at run-time
OS_TID tsk1, tsk2;

#define VREF 		5.0
#define FULLSCALE 	0xFF

typedef struct {                      	// Message object structure            
	char data;
	char ana_op[5];
}T_MEAS;

char lcd_msg1[] = "DIGITL O/P=";
char lcd_msg2[] = "ANALOG I/P=";
float ana_output;
unsigned int temp1=0, ana_temp=0;
unsigned int i=0, j=0;
unsigned long int tmp1=0, tmp2=0, data_temp=0;

os_mbx_declare (MsgBox,1);           	// Declare an RTX mailbox             
_declare_box (mpool,sizeof(T_MEAS),1);	// Dynamic memory pool                

//Function prototypes
__task void init_task(void);
__task void send_task(void);
__task void rec_task(void);
void delay(void);
void wr_dn(void);
void wr_cn(void);
void lcd_init(void);
void clr_disp(void);
void lcd_com(void);						   
void lcd_data(void);
void lcd_puts(char *);
void delay_lcd(unsigned int);
void adc_val_disp(unsigned char);

/*-----------------------------------------------------------------------------------
 *   init_task:  RTX Kernel starts this task with os_sys_init_prio(init_task, 10);
 *---------------------------------------------------------------------------------*/
__task void init_task(void) {
	tsk1 = os_tsk_create(send_task,2); 			// send_task at priority 2 
	tsk2 = os_tsk_create(rec_task,1); 			// rec_task at priority 1
	os_tsk_delete_self(); 						// must delete itself before exiting
}

/*----------------------------------------------------------------------------
 *  Task 1:  RTX Kernel starts this task with os_tsk_create(send_task, 2);
 *---------------------------------------------------------------------------*/
__task void send_task(void) {
	T_MEAS *mptr;           
  	os_mbx_init(MsgBox, sizeof(MsgBox));		// initialize the mailbox             
    
	while(1){     
		// cnt1++;
		os_dly_wait(1);
		mptr = _alloc_box(mpool);              // Allocate a memory for the message 

		tmp1 = 0x80;
		lcd_com();
		delay();								// display message lcd_msg2[] = "DIGITL O/P=" ;
		lcd_puts(lcd_msg1);
		delay();
		
		tmp1 = 0xC0;
		lcd_com();					   			// display message lcd_msg2[] = "ANALOG I/P=" ;
		delay();
		lcd_puts(lcd_msg2);
		delay();
	
		IO0CLR = 0X00004000; 					// addr2 - 0 		  	 // select channel 0;
		IO1CLR = 0X00030000;					// addr1 = addr0 = 0 

		IO0SET = 0X00008000;  					// start high
     	for(i=0; i<100; i++); 	 				// start conversion.
 		IO0CLR =  0X00008000; 					// start low

		do{
			temp1=IO1PIN;
        	temp1 &= 0x00080000;				// wait till EOC line goes high ,it indicates end of conversion.
		} while(temp1 == 0x00000000);  	

		IO1SET = 0X00040000;  					// output enable made high
   		for(i=0; i<100; i++); 					// Delay
 
		data_temp = IO0PIN;		   				// read data
		data_temp &= 0x00ff0000;	   			// extract 16 bit
		data_temp >>= 16; 
		mptr->data = data_temp;			   		// get the 16 bit adc value

		IO1CLR = 0X00040000; 		   			// clear output_EN

		ana_temp = data_temp;

		ana_output = (float)((5.05/(float)256)*ana_temp);

		sprintf(mptr->ana_op ,"%.2f" ,ana_output);

		os_mbx_send(MsgBox, mptr, 0xffff); 		// Send the message to the mailbox     
		os_dly_wait(100);
	}	 
}

/*----------------------------------------------------------------------------
 *  Task 2: RTX Kernel starts this task with os_tsk_create (rec_task, 1)
 *---------------------------------------------------------------------------*/
__task void rec_task(void) {
	T_MEAS *rptr;

	while(1) {
		// cnt2++;
    	os_mbx_wait(MsgBox, (void **)&rptr, 0xffff); 	// wait for the message    

		tmp1 = 0x8B;
		lcd_com();
		delay();	

		adc_val_disp(rptr->data);	   					// display digital data

		tmp1 = 0xCB;
		lcd_com();
		delay();	

		lcd_puts(rptr->ana_op);	   						// display analog voltage.

		for(i=0; i<50000; i++);
 
    	_free_box (mpool, rptr);           				// free memory allocated for message  
	}										
}

/*----------------------------------------------------------------------------
 *	Main: Initialize and start RTX Kernel
 *---------------------------------------------------------------------------*/
int main (void) {                     		// program execution starts here       
   	PINSEL0 = 0X00000000; 					// all p0.0 to p0.15  made as GPIO
	PINSEL1 = 0X00000000;					// all p0.15 to p0.31 made as GPIO

	IO0DIR = 0X0000C000;					// P0.14(adr2) AND P0.15(start) MADE OUTPUTS , PO.16 TO P0.23 MADE AS INPUTS.
	IO1DIR = 0X00070000;					// P1.16(adr0),P1.17(adr1),P1.18(out_en)  MADE OUTPUTS & p1.19(EOC) made input
								
	lcd_init();
	for(i=0; i<2500; i++);
	clr_disp();

	_init_box (mpool, sizeof(mpool),    	// initialize the 'mpool' memory for   
              sizeof(T_MEAS));        		// the membox dynamic allocation       
 	os_sys_init_prio(init_task, 10);          
}

/*----------------------------------------------------------------------------
 *  LCD related function definitions
 *---------------------------------------------------------------------------*/
void delay() {
	unsigned int i;
	for(i=0; i<=200;i++) ;
}

void adc_val_disp(unsigned char disp) {
	unsigned char temp = 0x00;

	temp = (disp & 0xf0) >> 4;

	if(temp <= 9 )
		tmp1 = temp + 0x30;
	else
		tmp1 = temp + 0x37;

	lcd_data();	

	temp = disp & 0x0f;

	if(temp <= 9 )
		tmp1 = temp + 0x30;
	else
		tmp1 = temp + 0x37;

	lcd_data();	
}

void lcd_init() {
	IO0DIR |= 0x000000fc;  				// ( port line p0.2,p0.3 , p0.4 to p0.7 are made o/p)
	                                    // lcd RS , EN , D4 to D7 
	IO0CLR = 0x000000fc;  				// clear the LCD lines
	 
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

void lcd_com(void) {
	tmp2= tmp1 & 0xf0;
	wr_cn();
	tmp2 = tmp1 & 0x0f;
	tmp2 = tmp2 << 4; 
	wr_cn();
    delay_lcd(1000);
}

// command nibble o/p routine
void wr_cn(void) {                  // write command reg
	IO0CLR  = 0x000000fc;			// clear the port lines.
  	IO0SET 	= tmp2;					// Assign the value to the PORT lines    
	IO0CLR  = 0x00000004;			// clear bit  RS = 0
  	IO0SET 	= 0x00000008;   		// EN=1
	delay_lcd(25);
	IO0CLR  = 0x00000008;		 	// EN =0
}

// data o/p routine which also outputs high nibble first
// and lower nibble next
void lcd_data(void) {  
    tmp2 = tmp1 & 0xf0;
    wr_dn();
    tmp2= tmp1 & 0x0f;	
    tmp2= tmp2 << 4;
    wr_dn();
    delay_lcd(1000);					
} 

// data nibble o/p routine
void wr_dn(void) {  	  
	IO0CLR = 0x000000fc;			// clear the port lines.
	IO0SET = tmp2;					// Assign the value to the PORT lines 
	IO0SET = 0x00000004;   			// set bit  RS = 1 	
	IO0SET = 0x00000008;   			// EN=1
	delay_lcd(25);
	IO0CLR = 0x00000008;
}

void delay_lcd(unsigned int r1) {
  	unsigned int r;
  	for(r=0;r<r1;r++);
}

void clr_disp(void) {
	tmp1 = 0x01;
	lcd_com();
 	delay_lcd(10000);
}

void lcd_puts(char *lcd_buf) {
	unsigned int i = 0 ;
    for(i=0; lcd_buf[i]!='\0'; i++) {
         tmp1 = lcd_buf[i];
     	 lcd_data();
    }
}
	
/*----------------------------------------------------------------------------
 * end of file
 *---------------------------------------------------------------------------*/
