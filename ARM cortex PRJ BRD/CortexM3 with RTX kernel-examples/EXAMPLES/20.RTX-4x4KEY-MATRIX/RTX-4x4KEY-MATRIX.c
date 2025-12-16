/*-----------------------------------------------------------------------------
 *      RL-ARM - RTX
 *-----------------------------------------------------------------------------
 *      Name:    RTX-4x4KEY-MATRIX.c  
 *      Purpose: Implementing 4x4KEY-MATRIX using Mailbox concept
 *----------------------------------------------------------------------------*/

#include <RTL.h>                      		// RTX kernel functions & defines      
#include <LPC17xx.h>                  		// LPC21xx definitions                 
#include <stdio.h>
#include <string.h>

OS_TID tsk1;                          		// assigned identification for task 1  
OS_TID tsk2;                          		// assigned identification for task 2 

#define RS_CTRL  0x00000010  //P2.4
#define EN_CTRL  0x00000020  //P2.5
#define DT_CTRL  0x000003C0  //P2.6 to P2.9 data lines 

typedef struct {                      		// Message object structure            
	char str1[17];
	char str2[15];
}T_MEAS;

os_mbx_declare (MsgBox,1);           		// Declare an RTX mailbox             
_declare_box (mpool,sizeof(T_MEAS),1);		// Dynamic memory pool                

// Function prototypes
__task void init_task(void);
__task void send_task (void);
__task void rec_task (void);
void lcd_init(void);
void wr_cn(void);
void clr_disp(void);
void delay_lcd(unsigned int);
void lcd_com(void);						   
void wr_dn(void);
void lcd_data(void);
void clear_ports(void);
void scan(void);
void get_key(void);

unsigned char test=0;
unsigned int cnt1=0,cnt2=0,k=0;
unsigned char col,row,var,flag,key,*ptr;
unsigned long int i,var1,temp,temp3,temp1=0,temp2=0;
unsigned char SCAN_CODE[16] = {0x1E,0x1D,0x1B,0x17,
 							  	0x2E,0x2D,0x2B,0x27,
							  	0x4E,0x4D,0x4B,0x47,
							  	0x8E,0x8D,0x8B,0x87};

unsigned char KEYCODE[16]= {0x00,0x01,0x02,0x03,
                               0x04,0x05,0x06,0x07,
                               0x08,0x09,0x0A,0x0B,
                               0x0C,0x0D,0x0E,0x0F};	 

/*-----------------------------------------------------------------------------------
 *   init_task:  RTX Kernel starts this task with os_sys_init_prio(init_task, 10);
 *---------------------------------------------------------------------------------*/
__task void init_task(void) {
	tsk1 = os_tsk_create(send_task,2); 	// send_task at priority 2 
	tsk2 = os_tsk_create(rec_task,1); 	// rec_task at priority 1
	os_tsk_delete_self(); 			// must delete itself before exiting
}

/*----------------------------------------------------------------------------
 *  Task 1:  RTX Kernel starts this task with os_tsk_create (send_task, 2)
 *---------------------------------------------------------------------------*/
__task void send_task (void) {
	T_MEAS *mptr;

  	tsk1 = os_tsk_self ();              			// get own task identification number 
  	tsk2 = os_tsk_create (rec_task, 0); 			// start task 2                       
  	os_mbx_init (MsgBox, sizeof(MsgBox));			// initialize the mailbox             
       
	while(1)
	{
		cnt1++;
		os_dly_wait (1);
		
		mptr = _alloc_box (mpool);              	// Allocate a memory for the message   
		if(test==0)  
 		{
			strcpy(mptr->str1,"KEYPAD TESTING");
			sprintf(mptr->str2,"WAITING      ");	// Convert keycode into ASCII to display it on LCD
			test=0xff;
		}
		else
		{
			get_key();								// Read Keyboard
			strcpy(mptr->str1,"KEYPAD TESTING");
			sprintf(mptr->str2,"KEY = %02X   ",key);	// Convert keycode into ASCII to display it on LCD
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
    	while(rptr->str1[k]!='\0')
    	{
   			temp1 = rptr->str1[k];
    	 	lcd_data();
			os_dly_wait (1);
		 	k++;
    	} 
		k=0;

		temp1 = 0xC0;					// Display starting address	of first line 1 th pos
		lcd_com();
	 
		k=0;
    	while(rptr->str2[k]!='\0')
    	{
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
	LPC_PINCON->PINSEL3 &= 0xFFFF00FF; 			//P1.20 to P1.23 MADE GPIO
	LPC_PINCON->PINSEL0 &= 0x3FFFFFFF;  		//P0.15 as GPIO
	LPC_PINCON->PINSEL1 &= 0xFFFFFFC0; 			//P0.16 t0 P0.18 made GPIO
	LPC_GPIO0->FIODIR &= ~0x00078000; 			//made INput P0.15 to P0.18 (cols)
	LPC_GPIO1->FIODIR |= 0x00F00000; 			//made output P1.20 to P1.23 (rows)
	LPC_GPIO1->FIOSET = 0x00F00000;  

	lcd_init();							//initialise LCD
	delay_lcd(3200);

	_init_box (mpool, sizeof(mpool),    // initialize the 'mpool' memory for   
              sizeof(T_MEAS));        	// the membox dynamic allocation       
 	 os_sys_init_prio(init_task, 10);   // initialize and start init_task
}

void get_key(void)                			// get the key from the keyboard
{
	while(1)
	{
		for(row=1;row<5;row++)
		{
			if(row == 1)
			var1 = 0x00100000;
			else if(row == 2)
			var1 = 0x00200000;
			else if(row == 3)
			var1 = 0x00400000;
			else if(row == 4)
			var1 = 0x00800000;
			
			temp = var1;
			LPC_GPIO1->FIOSET = 0x00F00000;
			LPC_GPIO1->FIOCLR = var1;

			flag = 0;
			scan();
			if(flag == 1)
				break;
		} 							//end for(row=1;row<5;row++)
		if(flag == 1)
			break;
	}							 	//2nd while(1)

	for(i=0;i<16;i++)
	{
		if(key == SCAN_CODE[i])
		{
			key = KEYCODE[i];
			break;
		} 							//end if(key == SCAN_CODE[i])
	}
}// end of get_key(); 

void scan(void)
{
 	unsigned long temp3;

	temp3 = LPC_GPIO0->FIOPIN;	
	temp3 &= 0x00078000;
	if(temp3 != 0x00078000)
	{
		for(i=0;i<500;i++);
		temp3 = LPC_GPIO0->FIOPIN;	
		temp3 &= 0x00078000;
		if(temp3 != 0x00078000)
		{
			flag = 1;
			temp3 >>= 15;					//Shifted to come at LN of byte
			temp >>= 16;					//shifted to come at HN of byte
			key = temp3|temp;	
		}									//2nd if(temp3 != 0x00000000)
	}										//1st if(temp3 != 0x00000000)
}											//end scan

//lcd initialization
void lcd_init()
{
	/* Ports initialized as GPIO */
    LPC_PINCON->PINSEL4 &= 0xFFF000FF;  //P2.4 to P2.9

	/* Setting the directions as output */
    LPC_GPIO2->FIODIR |= DT_CTRL;	
	LPC_GPIO2->FIODIR |= RS_CTRL;	
	LPC_GPIO2->FIODIR |= EN_CTRL;	
        
    clear_ports();
	delay_lcd(3200);

	temp2 = (0x30<<2);
	wr_cn();	   
	delay_lcd(30000); 
		
	temp2 = (0x30<<2);
	wr_cn();
	delay_lcd(30000);	 
		
	temp2 = (0x30<<2);
	wr_cn();
	delay_lcd(30000);

	temp2 = (0x20<<2);
	wr_cn();
	delay_lcd(30000);

	temp1 = 0x28;
	lcd_com();
	delay_lcd(30000);
		
	temp1 = 0x0c;		
	lcd_com();
	delay_lcd(800);
	
	temp1 = 0x06;
	lcd_com();
	delay_lcd(800);
	
	temp1 = 0x01;
	lcd_com();
 	delay_lcd(10000);
	
	temp1 = 0x80;
	lcd_com();
	delay_lcd(800);
    return;
}

void lcd_com(void)
{
	temp2 = temp1 & 0xf0;				//move data (26-8+1) times : 26 - HN place, 4 - Bits
	temp2 = temp2 << 2;					//data lines from 23 to 26
	wr_cn();
	temp2 = temp1 & 0x0f; 				//26-4+1
	temp2 = temp2 << 6; 
	wr_cn();
	delay_lcd(1000);
    return;
}

// command nibble o/p routine
void wr_cn(void)                        //write command reg
{ 	 
	clear_ports();
	LPC_GPIO2->FIOPIN = temp2;			//Assign the value to the data lines    
    LPC_GPIO2->FIOCLR = RS_CTRL;		//clear bit RS
	LPC_GPIO2->FIOSET = EN_CTRL;   		//EN=1
	delay_lcd(25);
	LPC_GPIO2->FIOCLR = EN_CTRL;		//EN =0
    return;
}

// data o/p routine which also outputs high nibble first
// and lower nibble next
void lcd_data(void)
{             
    temp2 = temp1 & 0xf0;
    temp2 = temp2 << 2;
    wr_dn();
    temp2= temp1 & 0x0f;	
    temp2= temp2 << 6;
    wr_dn();
    delay_lcd(1000);	
    return;
} 

// data nibble o/p routine
void wr_dn(void)
{  	  
	clear_ports();

	LPC_GPIO2->FIOPIN = temp2;			//Assign the value to the data lines    
	LPC_GPIO2->FIOSET = RS_CTRL;		//set bit  RS
	LPC_GPIO2->FIOSET = EN_CTRL;   		//EN=1
	delay_lcd(25);
	LPC_GPIO2->FIOCLR = EN_CTRL;		//EN =0
    return;
}

void delay_lcd(unsigned int r1)
{
  	unsigned int r;
  	for(r=0;r<r1;r++);
    return;
}

void clr_disp(void)
{
	temp1 = 0x01;
	lcd_com();
 	delay_lcd(10000);
    return;
}

void clear_ports(void)
{
    /* Clearing the lines at power on */
	LPC_GPIO2->FIOCLR = DT_CTRL; 		//Clearing data lines
	LPC_GPIO2->FIOCLR = RS_CTRL;  		//Clearing RS line
	LPC_GPIO2->FIOCLR = EN_CTRL; 		//Clearing Enable line
    return;
}

/*----------------------------------------------------------------------------
 * end of file
 *---------------------------------------------------------------------------*/
