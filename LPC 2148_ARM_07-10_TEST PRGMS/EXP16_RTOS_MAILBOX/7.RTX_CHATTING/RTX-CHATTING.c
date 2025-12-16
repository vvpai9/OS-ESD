/*----------------------------------------------------------------------------------------
 *      RL-ARM - RTX
 *----------------------------------------------------------------------------------------
 *      Name:    RTX-CHATTING.c  
 *      Purpose: RTX Task passing a message from one task to other using Mailbox concept
 *---------------------------------------------------------------------------------------*/

#include <RTL.h>                      		// RTX kernel functions & defines      
#include <LPC214x.h>                  		// LPC21xx definitions                 
#include <stdio.h>
#include <string.h>

// tsk1, tsk2 will contain task identifications at run-time
OS_TID tsk1, tsk2; 
											
typedef struct {                      		// Message object structure            
  	char msg1[20];               			// AD result of measured adc output voltage 
} T_MEAS1;

typedef struct {                      		// Message object structure            
  	char msg2[20];               			// AD result of measured adc output voltage 
} T_MEAS2;

unsigned int i=0, k=0;
unsigned int cnt1=0, cnt2=0;

os_mbx_declare (MsgBox1,2);           		// Declare an RTX mailbox              
_declare_box (mpool1,sizeof(T_MEAS1),2);	// Dynamic memory pool 

os_mbx_declare (MsgBox2,2);           		// Declare an RTX mailbox              
_declare_box (mpool2,sizeof(T_MEAS2),2);	// Dynamic memory pool                

/* Function prototypes */
__task void init_task(void);
__task void send_task (void);
__task void rec_task (void);
void init_serial(void);

/*-----------------------------------------------------------------------------------
 *   init_task:  RTX Kernel starts this task with os_sys_init_prio(init_task, 10);
 *---------------------------------------------------------------------------------*/
__task void init_task(void) {
	tsk1 = os_tsk_create(send_task,2); 		// send_task at priority 2 
	tsk2 = os_tsk_create(rec_task,1); 		// rec_task at priority 1
	os_tsk_delete_self(); 					// must delete itself before exiting
}
/*----------------------------------------------------------------------------
 *  Task 1:  RTX Kernel starts this task with os_tsk_create(send_task,2);
 *---------------------------------------------------------------------------*/
__task void send_task(void) {
	T_MEAS1 *mptr1;
	T_MEAS2 *rptr2;
	unsigned char msg1[] = "Task1: Start Typing Characters from PC Keyboard\r";
	unsigned char msg2[] = "\rTask1: Characters Recieved:\r";
														                          
  	os_mbx_init(MsgBox1, sizeof(MsgBox1));	// initialize the mailbox              
 
  	while(1){
 		cnt1++;
  		mptr1 = _alloc_box (mpool1);      	// Allocate a memory for the message   
	
		while (msg1[k] != '\0')	{
		  	while (!(U0LSR & 0x20));
		  	U0THR = msg1[k];
			k++;
		}
		k=0;
		
		while(1){
	  		while (!(U0LSR & 0x01));
				mptr1->msg1[i] = U0RBR;
			while (!(U0LSR & 0x20));
				U0THR = mptr1->msg1[i];
			if (mptr1->msg1[i] == 0x2A)
				break;
			i++;
		}
		i=0;
		os_mbx_send (MsgBox1, mptr1, 0xffff); // Send the message to the mailbox 
		os_dly_wait (10); 

		os_mbx_wait (MsgBox2, (void **)&rptr2, 0xffff); 	// wait for the message  
		
		while (msg2[k] != '\0') {
		  	while (!(U0LSR & 0x20));
			U0THR = msg2[k];
			k++;
		}
		k=0; 
	
		while (rptr2->msg2[k] != 0x2A) {
		  	while (!(U0LSR & 0x20));
		  	U0THR = rptr2->msg2[k];
			k++;
		}
		k=0;

		while (!(U0LSR & 0x20));
		U0THR = '\r';

		_free_box (mpool2, rptr2);           	// free memory allocated for message 
		os_dly_wait (10);   
	}
}

/*----------------------------------------------------------------------------
 *  Task 2: RTX Kernel starts this task with os_tsk_create (rec_task, 1)
 *---------------------------------------------------------------------------*/
__task void rec_task (void) {
	T_MEAS1 *rptr1;
	T_MEAS2 *mptr2;
	unsigned char msg1[] = "Task2: Start Typing Characters from PC Keyboard\r";
	unsigned char msg2[] = "\rTask2: Characters Recieved:\r";

	while(1) {
  		cnt2++;
    	mptr2 = _alloc_box (mpool2);      	// Allocate a memory for the message 
    	os_mbx_wait (MsgBox1, (void **)&rptr1, 0xffff); 	// wait for the message   
	
		while (msg2[k] != '\0')	{
		  	while (!(U0LSR & 0x20));
		  	U0THR = msg2[k];
			k++;
		}
		k=0;
		
		while (rptr1->msg1[k] != 0x2A) {
		  	while (!(U0LSR & 0x20));
		  	U0THR = rptr1->msg1[k];
			k++;
		}
		k=0;
		while (!(U0LSR & 0x20));
		  	U0THR = '\r';	

		_free_box (mpool1, rptr1);           	// free memory allocated for message  
		
		while (msg1[k] != '\0') {
		  	while (!(U0LSR & 0x20));
		  	U0THR = msg1[k];
			k++;
		}
		k=0;  
	
		i=0;
		while(1){
	  		while (!(U0LSR & 0x01));
				mptr2->msg2[i] = U0RBR;
			while (!(U0LSR & 0x20));
				U0THR = mptr2->msg2[i];
			if (mptr2->msg2[i] == 0x2A)
				break;
			i++;
		}

		os_mbx_send (MsgBox2, mptr2, 0xffff); // Send the message to the mailbox  
		os_dly_wait (10);  
	}
}

/*----------------------------------------------------------------------------
 *        Initialize serial interface - UART0
 *---------------------------------------------------------------------------*/
void init_serial(void) {
	PINSEL0 = 0X0000005;        			// Enable TXD0-P0.0 & RXD0-P0.1               
  	U0LCR = 0x83;               			// 8 bits, no Parity, 1 Stop bit     
  	U0DLL = 0x61;               			// 9600 Baud Rate @ 15MHz VPB Clock  
  	U0LCR = 0x03;               			// DLAB = 0                          
}

/*----------------------------------------------------------------------------
 *        Main: Initialize and start RTX Kernel
 *---------------------------------------------------------------------------*/
int main (void) {                     		// program execution starts here       
	init_serial ();                     	// initialize the serial interface     
   	_init_box (mpool1, sizeof(mpool1),    	// initialize the 'mpool' memory for   
              sizeof(T_MEAS1));        		// the membox dynamic allocation    
	_init_box (mpool2, sizeof(mpool2),    	// initialize the 'mpool1' memory for   
              sizeof(T_MEAS2));        		// the membox dynamic allocation       
 	os_sys_init_prio(init_task, 10);             
}

/*----------------------------------------------------------------------------
 * end of file
 *---------------------------------------------------------------------------*/
