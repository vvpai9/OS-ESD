/*----------------------------------------------------------------------------
 *      RL-ARM - RTX
 *----------------------------------------------------------------------------
 *      Name:    RTX-MULTIPLEX-SEMAPHORE.c
 *      Purpose: RTX example program to demonstrate semaphore.
 *----------------------------------------------------------------------------*/

#include <RTL.h>                      	/* RTX kernel functions & defines      */
#include <LPC17xx.h>                  	/* LPC17xx definitions                 */
#include <stdio.h>

// tsk1, tsk2 will contain task identifications at run-time 
OS_TID tsk1, tsk2, tsk3;
OS_SEM semaphore1;

unsigned int count=0;
char buf[20]; 

// Function prototypes
__task void init_task(void); 
__task void task1 (void);
__task void task2 (void);
__task void task3 (void);
void init_serial(void);
void serial_data(char msg[]);

/*-----------------------------------------------------------------------------------
 *   init_task:  RTX Kernel starts this task with os_sys_init_prio(init_task, 10);
 *---------------------------------------------------------------------------------*/
__task void init_task(void) {
	// Initialize the Semaphore before the first use 
  	os_sem_init (semaphore1,3);
  	// Create an instance of task1 with priority 1 
  	tsk1 = os_tsk_create (task1, 1);
  	// Create an instance of task2 with default priority 1 
  	tsk2 = os_tsk_create (task2, 1);
	// Create an instance of task2 with default priority 1 
  	tsk3 = os_tsk_create (task3, 1);
	os_tsk_delete_self ();
}

/*----------------------------------------------------------------------------
 *    Task 1 
 *---------------------------------------------------------------------------*/
__task void task1 (void) {
	char msg1[]="Task1 Running\r";
	
	while (1) {
		// Wait indefinetly for a free semaphore 
    	os_sem_wait(semaphore1, 0xFFFF);
		serial_data(msg1);
		count += 1;		   // critical section
		sprintf(buf,"count value in task1 = %d\r", count); 
		serial_data(buf);
		os_dly_wait(50);
		// Return a token back to a semaphore container 
    	os_sem_send (semaphore1);
   	}
}	

/*----------------------------------------------------------------------------
 *    Task 2 
 *---------------------------------------------------------------------------*/
__task void task2 (void) {
	char msg2[]="Task2 Running\r";
  	
	while (1) {
    	// Wait indefinetly for a free semaphore 
    	os_sem_wait(semaphore1, 0xFFFF);
		serial_data(msg2);
		count += 2;		 // critical section
		sprintf(buf,"count value in task2 = %d\r", count); 
  		serial_data(buf);
		os_dly_wait(50);
    	// Return a token back to a semaphore container 
    	os_sem_send (semaphore1);
	}
}

/*----------------------------------------------------------------------------
 *    Task 3 
 *---------------------------------------------------------------------------*/
__task void task3 (void) {
	char msg3[]="Task3 Running\r";
  
  	while (1) {
    	// Wait indefinetly for a free semaphore 
    	os_sem_wait(semaphore1, 0xFFFF);
		serial_data(msg3);
		count += 4;		// critical section
		sprintf(buf,"count value in task3 = %d\r", count);   
		serial_data(buf);
    	os_dly_wait(50);
		// Return a token back to a semaphore container
    	os_sem_send (semaphore1);
	}
}

/*----------------------------------------------------------------------------
 *        serial_data
 *---------------------------------------------------------------------------*/
void serial_data(char msg[]) {
	unsigned int i=0;
	while(msg[i] != '\0'){
		while(!(LPC_UART0->LSR & 0x20));
		LPC_UART0->THR = msg[i];
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

/*------------------------------------------------------------------------------
 *        Main: Initialize and start RTX Kernel
 *----------------------------------------------------------------------------*/
int main(void) {
	UART0_Init();	  						// Initialize serial interface
	os_sys_init_prio(init_task, 10);		// Initialize init_task and start RTX Kernel
}
/*---------------------------------------------------------------------------------------------------------------
 * end of file
 *-------------------------------------------------------------------------------------------------------------*/


