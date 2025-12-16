/*--------------------------------------------------------------------------------------------------------------------------------------
 *      RL-ARMCTX - RTX
 *--------------------------------------------------------------------------------------------------------------------------------------
 *      Name:    RTX_Co-op-task.c
 *      Purpose: RTX example program to show co-operative task switching
 *------------------------------------------------------------------------------------------------------------------------------------*/

#include <RTL.h>                      		// RTX kernel functions & defines      
#include <lpc17xx.h>
#include <stdio.h>

// tsk1, tsk2 will contain task identifications at run-time 
OS_TID tsk1, tsk2;

U16 counter1;                          		// counter for tasks 1             
U16 counter2;                          		// counter for tasks 2             

char buf[20];
unsigned int i=0;

// Function prototypes
__task void init_task(void); 
__task void task1 (void);
__task void task2 (void);
void UART0_Init(void);

/*--------------------------------------------------------------------------------------------------------------------------------------
 *   init_task:  RTX Kernel starts this task with os_sys_init_prio(init_task, 10);
 *------------------------------------------------------------------------------------------------------------------------------------*/
__task void init_task(void) {
	tsk1 = os_tsk_create(task1,1); 				// task1 at priority 1 
	tsk2 = os_tsk_create(task2,1); 				// task2 at priority 1
	os_tsk_delete_self(); 						// must delete itself before exiting
}
 
/*--------------------------------------------------------------------------------------------------------------------------------------
 *   Task 1:  RTX Kernel starts this task with os_tsk_create (task1,1)
 *------------------------------------------------------------------------------------------------------------------------------------*/
__task void task1 (void) {
	while (1) {
    	counter1++;              				// increment  counter1   
		sprintf(buf,"Task-1, counter1 : %d",counter1);
		
		while (buf[i] != '\0'){
	  		while (!(LPC_UART0->LSR & 0x20));
	  		LPC_UART0->THR = buf[i];
			i++;
		}
		i=0;  

		while (!(LPC_UART0->LSR & 0x20));
	  		LPC_UART0->THR = '\r';	
		os_tsk_pass ();          				// switch to 'task2' 
  	}
}

/*--------------------------------------------------------------------------------------------------------------------------------------
 *   Task 2:  RTX Kernel starts this task with os_tsk_create (task2,1)
 *-----------------------------------------------------------------------------------------------------------------------------------*/
__task void task2 (void) {
  	while (1) {
    	counter2++;           					// increments counter2   
		sprintf(buf,"Task-2, counter2 : %d",counter2);
		
		while (buf[i] != '\0'){
	  		while (!(LPC_UART0->LSR & 0x20));
	  		LPC_UART0->THR = buf[i];
			i++;
		}
		i=0;  

		while (!(LPC_UART0->LSR & 0x20));
	  		LPC_UART0->THR = '\r';
  		os_tsk_pass ();          				// switch to 'task1' 
	}
}

/*--------------------------------------------------------------------------------------------------------------------------------------
 *        Initialize serial interface										 
 *------------------------------------------------------------------------------------------------------------------------------------*/
void UART0_Init(void) {
	LPC_SC->PCONP |= 0x00000008;				// UART0 peripheral enable
	LPC_PINCON->PINSEL0 &= ~0x000000F0;
	LPC_PINCON->PINSEL0 |= 0x00000050;			// P0.2 - TXD0 and P0.3 - RXD0
	LPC_UART0->LCR = 0x00000083;				// enable divisor latch, parity disable, 1 stop bit, 8bit word length
	LPC_UART0->DLM = 0X00; 
	LPC_UART0->DLL = 0xA2;      				// select baud rate 9600 bps @25MHz
	LPC_UART0->LCR = 0X00000003;
	LPC_UART0->FCR = 0x07;
	LPC_UART0->IER = 0X03;	   					// select Transmit and receive interrupt
}

/*--------------------------------------------------------------------------------------------------------------------------------------
 *        Main: Initialize and start RTX Kernel
 *------------------------------------------------------------------------------------------------------------------------------------*/
int main(void) {
	UART0_Init();		  						// Initialize serial interface
	os_sys_init_prio(init_task, 10);			// Initialize init_task and start RTX Kernel
}
/*--------------------------------------------------------------------------------------------------------------------------------------
 * end of file
 *------------------------------------------------------------------------------------------------------------------------------------*/

