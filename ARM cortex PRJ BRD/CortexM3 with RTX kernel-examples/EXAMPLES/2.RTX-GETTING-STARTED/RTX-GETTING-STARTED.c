/*-----------------------------------------------------------------------------------------------------------
 *      RL-ARMCTX - RTX
 *-----------------------------------------------------------------------------------------------------------
 *      Name:    RTX-GETTING-STARTED.c
 *      Purpose: Creating tasks and do task switching using os_evt_set() function
 *----------------------------------------------------------------------------------------------------------*/

#include <RTL.h>                      // RTX kernel functions & defines      
#include <LPC17xx.H> 
#include <stdio.h>

// tsk1, tsk2 will contain task identifications at run-time
OS_TID tsk1, tsk2;

/* Function prototypes */
__task void init_task(void);
__task void task1(void);
__task void task2(void);
void UART0_Init(void);

unsigned char msg1[]= "Task1 Created\r";
unsigned char msg2[]= "Task2 Created\r";
unsigned int i=0,cnt1=0,cnt2=0;

/*--------------------------------------------------------------------------------------------------------------
 *   init_task:  RTX Kernel starts this task with os_sys_init_prio(init_task, 10);
 *------------------------------------------------------------------------------------------------------------*/
__task void init_task(void) {
	tsk1 = os_tsk_create(task1,2); 	// task1 at priority 2 
	tsk2 = os_tsk_create(task2,1); 	// task2 at priority 1
	os_tsk_delete_self(); 			// must delete itself before exiting
}

/*--------------------------------------------------------------------------------------------------------------
 *   Task 1:  RTX Kernel starts this task with os_tsk_create (task1,2)
 *------------------------------------------------------------------------------------------------------------*/
__task void task1 (void){
	while (1) {                        // endless loop                        
		// ... place code for task1 activity here ... 
		cnt1++;		 // testing purpose
		while (msg1[i] != '\0'){
	  		while (!(LPC_UART0->LSR & 0x20));
	  		LPC_UART0->THR = msg1[i];
			i++;
		}
		i=0;
		
		// Signal to task2 that task1 has compelted 
		os_evt_set(0x0004, tsk2);

		// Wait for completion of task2 activity. 
		// 0xFFFF makes it wait without timeout. 
		// 0x0004 represents bit 2. 
    	os_evt_wait_or(0x0004, 0xFFFF);

		// Wait for 50 ms before restarting task1 activity. 
		os_dly_wait(5);
	}
}

/*-----------------------------------------------------------------------------------------------------------------
 *   Task 2:  RTX Kernel starts this task with os_tsk_create (task2,1)
 *---------------------------------------------------------------------------------------------------------------*/
__task void task2 (void) {
	while (1){                        // endless loop                        
		// Wait for completion of task1 activity. 
		// 0xFFFF makes it wait without timeout. 
		// 0x0004 represents bit 2. 
		os_evt_wait_or(0x0004, 0xFFFF);

		// Wait for 20 ms before starting task2 activity. 
   		os_dly_wait(2);

		// ... place code for task2 activity here ... 
		cnt2++;
		while (msg2[i] != '\0'){
  			while (!(LPC_UART0->LSR & 0x20));
  			LPC_UART0->THR = msg2[i];
			i++;
		}
		i=0;
		// Signal to task1 that task2 has compelted 
    	os_evt_set(0x0004, tsk1);
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

/*------------------------------------------------------------------------------------------------------------------
 *        Main: Initialize and start RTX Kernel
 *----------------------------------------------------------------------------------------------------------------*/
int main(void) {
	UART0_Init();		  					// Initialize serial interface
	os_sys_init_prio(init_task, 10);		// Initialize init_task and start RTX Kernel
}
/*------------------------------------------------------------------------------------------------------------------
 * end of file
 *-----------------------------------------------------------------------------------------------------------------*/


