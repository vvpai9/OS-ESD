/*----------------------------------------------------------------------------
 *      RL-ARMCTX - RTX
 *----------------------------------------------------------------------------
 *      Name:    RTX-SIGNAL-SEMAPHORE.c
 *      Purpose: RTX example program to demonstrate signaling semaphore 
 *----------------------------------------------------------------------------*/
#include <RTL.h>                      	/* RTX kernel functions & defines      */
#include <LPC17xx.h>                  	/* LPC17xx definitions                 */
#include <stdio.h>

// tsk1, tsk2 will contain task identifications at run-time 
OS_TID tsk1, tsk2;
OS_SEM semaphore1;

// Function prototypes
__task void init_task(void); 
__task void task1(void);
__task void task2(void);
void UART0_Init(void);
void funcA(void);
void funcB(void);

/*-----------------------------------------------------------------------------------
 *   init_task:  RTX Kernel starts this task with os_sys_init_prio(init_task, 10);
 *---------------------------------------------------------------------------------*/
__task void init_task(void) {
	// Initialize the Semaphore before the first use 
  	os_sem_init (semaphore1,0);
  	// Create an instance of task1 with priority 2 
  	tsk1 = os_tsk_create (task1, 2);
  	// Create an instance of task2 with default priority 1 
  	tsk2 = os_tsk_create (task2, 1);
	os_tsk_delete_self ();
}

/*----------------------------------------------------------------------------
 *    Task 1 
 *---------------------------------------------------------------------------*/
__task void task1 (void) {

	while (1) {
    	// increament token in a semaphore container 
    	os_sem_send (semaphore1);
		// Call funcA
		funcA();
		os_dly_wait(10);
   	}
}	

/*----------------------------------------------------------------------------
 *    Task 2 
 *---------------------------------------------------------------------------*/
__task void task2 (void) {
 
  	while (1) {
    	// Wait indefinetly for a free semaphore 
    	os_sem_wait (semaphore1, 0xFFFF);
 		// Call funcB
		funcB();
		os_dly_wait(10);
  	}
}

/*----------------------------------------------------------------------------
 *        funcA
 *---------------------------------------------------------------------------*/
void funcA(void) {
	unsigned int i=0;
	unsigned char msg1[]="task1 funcA()\r";
    while(msg1[i] != '\0'){
		while (!(LPC_UART0->LSR & 0x20));
	  	LPC_UART0->THR = msg1[i];
		i++;
	}
	i=0;           
}

/*----------------------------------------------------------------------------
 *        funcB
 *---------------------------------------------------------------------------*/
void funcB(void) {
	unsigned int i=0;
	unsigned char msg2[]="task2 funcB()\r";
 	while(msg2[i] != '\0'){
		while(!(LPC_UART0->LSR & 0x20));
		LPC_UART0->THR = msg2[i];
		i++;
	}
	i=0;                       
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

/*----------------------------------------------------------------------------
 *        Main: Initialize and start RTX Kernel
 *---------------------------------------------------------------------------*/
int main (void) {
	UART0_Init();	 
  	os_sys_init_prio(init_task, 10);
}
/*----------------------------------------------------------------------------
 * end of file
 *---------------------------------------------------------------------------*/


