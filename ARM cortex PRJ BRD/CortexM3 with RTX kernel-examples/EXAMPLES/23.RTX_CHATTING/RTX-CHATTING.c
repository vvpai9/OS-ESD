/*----------------------------------------------------------------------------------------
 *      RL-ARM - RTX
 *----------------------------------------------------------------------------------------
 *      Name:    RTX-CHATTING.c  
 *      Purpose: RTX Task passing a message from one task to other using Mailbox concept
 *---------------------------------------------------------------------------------------*/

#include <RTL.h>                      		// RTX kernel functions & defines      
#include <LPC17xx.h>                  		// LPC17xx definitions                 
#include <stdio.h>
#include <string.h>

OS_TID tsk1;                          		// assigned identification for send_task - task1  
OS_TID tsk2;                          		// assigned identification for rec_task - task 2  
											
typedef struct {                      		// Message object structure            
  	char msg1[50];               			// array to store data message  
} T_MEAS1;

typedef struct {                      		// Message object structure            
  	char msg2[50];               			// array to store data message 
} T_MEAS2;

unsigned int i=0,j=0,k=0;
char buf[10],r_buf[10],t_buf[10];
unsigned int cnt1=0, cnt2=0,size=0;

os_mbx_declare(MsgBox1,1);           		// Declare an RTX mailbox              
_declare_box(mpool1,sizeof(T_MEAS1),1);		// Dynamic memory pool  

os_mbx_declare(MsgBox2,1);           		// Declare an RTX mailbox              
_declare_box(mpool2,sizeof(T_MEAS2),1);		// Dynamic memory pool               

/* Function prototypes */
__task void init_task(void);
__task void send_task (void);
__task void rec_task (void);
void UART0_Init(void);
void uart_interrupt(void)__irq;

/*-----------------------------------------------------------------------------------
 *   init_task:  RTX Kernel starts this task with os_sys_init_prio(init_task, 10);
 *---------------------------------------------------------------------------------*/
__task void init_task(void) {
	tsk1 = os_tsk_create(send_task,2); 	// send_task at priority 2 
	tsk2 = os_tsk_create(rec_task,1); 	// rec_task at priority 1
	os_tsk_delete_self(); 			// must delete itself before exiting
}

/*----------------------------------------------------------------------------
 *  Task 1:  RTX Kernel starts this task with os_tsk_create(send_task,2);
 *---------------------------------------------------------------------------*/
__task void send_task (void) {
	T_MEAS1 *mptr1;
	T_MEAS2 *rptr2;
	unsigned char msg1[] = "Task1: Start Typing Characters from PC Keyboard\r";
	unsigned char msg2[] = "\rTask1: Characters Recieved:\r";
                 
  	os_mbx_init (MsgBox1, sizeof(MsgBox1));	// initialize the mailbox              
 
  	while(1){
 		cnt1++;
  		mptr1 = _alloc_box (mpool1);      	// Allocate a memory for the message 
		
		while (msg1[k] != '\0') {
		  	while (!(LPC_UART0->LSR & 0x20));
		  	LPC_UART0->THR = msg1[k];
			k++;
		}
		k=0;  
	
		i=0;
		while(1){
	  		while (!(LPC_UART0->LSR & 0x01));
				mptr1->msg1[i] = LPC_UART0->RBR;
			while (!(LPC_UART0->LSR & 0x20));
				LPC_UART0->THR = mptr1->msg1[i];
			if (mptr1->msg1[i] == 0x2A)
				break;
			i++;
		}

		os_mbx_send (MsgBox1, mptr1, 0xffff); // Send the message to the mailbox 
		os_dly_wait (10); 

		os_mbx_wait (MsgBox2, (void **)&rptr2, 0xffff); 	// wait for the message  
		
		while (msg2[k] != '\0') {
		  	while (!(LPC_UART0->LSR & 0x20));
		  	LPC_UART0->THR = msg2[k];
			k++;
		}
		k=0; 
	
		while (rptr2->msg2[k] != 0x2A) {
		  	while (!(LPC_UART0->LSR & 0x20));
		  	LPC_UART0->THR = rptr2->msg2[k];
			k++;
		}
		k=0;

		while (!(LPC_UART0->LSR & 0x20));
		LPC_UART0->THR = '\r';

		_free_box (mpool2, rptr2);           	// free memory allocated for message 
		os_dly_wait (10);   
	}
}

/*----------------------------------------------------------------------------
 *  Task 2: RTX Kernel starts this task with os_tsk_create (rec_task, 2)
 *---------------------------------------------------------------------------*/
__task void rec_task (void) {
	T_MEAS1 *rptr1;
	T_MEAS2 *mptr2;
	unsigned char msg1[] = "Task2: Start Typing Characters from PC Keyboard\r";
	unsigned char msg2[] = "\rTask2: Characters Recieved:\r";

	os_mbx_init (MsgBox2, sizeof(MsgBox2));	// initialize the mailbox   
	while(1) {
  		cnt2++;
		mptr2 = _alloc_box (mpool2);      	// Allocate a memory for the message 
    	os_mbx_wait (MsgBox1, (void **)&rptr1, 0xffff); 	// wait for the message   
		
		while (msg2[k] != '\0') {
		  	while (!(LPC_UART0->LSR & 0x20));
		  	LPC_UART0->THR = msg2[k];
			k++;
		}
		k=0; 
	
		while (rptr1->msg1[k] != 0x2A) {
		  	while (!(LPC_UART0->LSR & 0x20));
		  	LPC_UART0->THR = rptr1->msg1[k];
			k++;
		}
		k=0;

		while (!(LPC_UART0->LSR & 0x20));
		LPC_UART0->THR = '\r';

		_free_box (mpool1, rptr1);           	// free memory allocated for message  
		os_dly_wait (10);
	

		while (msg1[k] != '\0') {
		  	while (!(LPC_UART0->LSR & 0x20));
		  	LPC_UART0->THR = msg1[k];
			k++;
		}
		k=0;  
	
		i=0;
		while(1){
	  		while (!(LPC_UART0->LSR & 0x01));
				mptr2->msg2[i] = LPC_UART0->RBR;
			while (!(LPC_UART0->LSR & 0x20));
				LPC_UART0->THR = mptr2->msg2[i];
			if (mptr2->msg2[i] == 0x2A)
				break;
			i++;
		}

		os_mbx_send (MsgBox2, mptr2, 0xffff); // Send the message to the mailbox  
		os_dly_wait (10); 
	}
}

/*--------------------------------------------------------------------------------------------------------------
 *        Initialize serial interface										 
 *------------------------------------------------------------------------------------------------------------*/
void UART0_Init(void) {
	LPC_SC->PCONP |= 0x00000008;			// UART0 peripheral enable
	LPC_PINCON->PINSEL0 &= ~0x000000F0;
	LPC_PINCON->PINSEL0 |= 0x00000050;		// P0.2 - TXD0 and P0.3 - RXD0
	LPC_UART0->LCR = 0x00000083;			// enable divisor latch, parity disable, 1 stop bit, 8bit word length
	LPC_UART0->DLM = 0X00; 
	LPC_UART0->DLL = 0xA2;      			// select baud rate 9600 bps @25MHz
	LPC_UART0->LCR = 0X00000003;
	LPC_UART0->FCR = 0x07;
	LPC_UART0->IER = 0X03;	   				// select Transmit and receive interrupt
}

/*------------------------------------------------------------------------------
 *        Main: Initialize and start RTX Kernel
 *----------------------------------------------------------------------------*/
int main(void) {
 	UART0_Init ();                     		// initialize the serial interface     
   	_init_box (mpool1, sizeof(mpool1),    	// initialize the 'mpool' memory for   
              sizeof(T_MEAS1));        		// the membox dynamic allocation    
	_init_box (mpool2, sizeof(mpool2),    	// initialize the 'mpool1' memory for   
              sizeof(T_MEAS2));        		// the membox dynamic allocation  
	os_sys_init_prio(init_task, 10);		// Initialize init_task and start RTX Kernel
}
/*---------------------------------------------------------------------------------------------------------------
 * end of file
 *-------------------------------------------------------------------------------------------------------------*/

