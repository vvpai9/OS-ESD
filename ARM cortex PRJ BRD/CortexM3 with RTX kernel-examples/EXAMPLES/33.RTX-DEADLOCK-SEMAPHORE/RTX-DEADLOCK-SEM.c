/*----------------------------------------------------------------------------
 *      RL-ARM - RTX
 *----------------------------------------------------------------------------
 *      Name:    RTX-DEADLOCK-SEM.c
 *      Purpose: RTX example program to demonstrate RTX-DEADLOCK semaphore.
 *----------------------------------------------------------------------------*/

#include <RTL.h>                      	/* RTX kernel functions & defines      */
#include <LPC17xx.h>                  	/* LPC17xx definitions                 */
#include <stdio.h>

// tsk1, tsk2 will contain task identifications at run-time 
OS_TID tsk1, tsk2;

OS_SEM Arrived1, Arrived2;

// Function prototypes
__task void init_task(void); 
__task void task1 (void);
__task void task2 (void);
void init_serial(void);
void funcA1(void);
void funcA2(void);
void funcB1(void);
void funcB2(void);
void serial_data(unsigned char msg[]);

/*-----------------------------------------------------------------------------------
 *   init_task:  RTX Kernel starts this task with os_sys_init_prio(init_task, 10);
 *---------------------------------------------------------------------------------*/
__task void init_task(void) {
	// Initialize the Semaphore before the first use 
  	os_sem_init (Arrived1, 0);
	os_sem_init (Arrived2, 0);
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
	unsigned char msg[]="task1 arrived at deadlock situation!\r";
	
	while (1) {
    	funcA1();
		serial_data(msg);
		os_sem_wait (Arrived2, 0xFFFF);	  // deadlock
		os_sem_send (Arrived1);
		funcA2();
		os_dly_wait(100);
   	}
}	

/*----------------------------------------------------------------------------
 *    Task 2 
 *---------------------------------------------------------------------------*/
__task void task2 (void) {
	unsigned char msg[]="task2 arrived at deadlock situation!\r";

	while (1) {
    	funcB1();
		serial_data(msg);
		os_sem_wait (Arrived1, 0xFFFF);	    // deadlock
		os_sem_send (Arrived2);
		funcB2();
		os_dly_wait(100);
	}
}

/*----------------------------------------------------------------------------
 *        funcA1
 *---------------------------------------------------------------------------*/
void funcA1(void) {
	unsigned int i=0;
	unsigned char msg1[]="task1 funcA1()\r";
	while(msg1[i] != '\0'){
		while(!(LPC_UART0->LSR & 0x20));
		LPC_UART0->THR = msg1[i];
		i++;
	}
	i=0;                     
}

/*----------------------------------------------------------------------------
 *        funcA2
 *---------------------------------------------------------------------------*/
void funcA2(void) {
	unsigned int i=0;
	unsigned char msg1[]="task1 funcA2()\r";
	while(msg1[i] != '\0'){
		while(!(LPC_UART0->LSR & 0x20));
		LPC_UART0->THR = msg1[i];
		i++;
	}
	i=0;                     
}

/*----------------------------------------------------------------------------
 *        funcB1
 *---------------------------------------------------------------------------*/
void funcB1(void) {
	unsigned int i=0;
	unsigned char msg2[]="task2 funcB1()\r";
	while(msg2[i] != '\0'){
  		while(!(LPC_UART0->LSR & 0x20));
  		LPC_UART0->THR = msg2[i];
		i++;
	}
	i=0;                      
}

/*----------------------------------------------------------------------------
 *        funcB2
 *---------------------------------------------------------------------------*/
void funcB2(void) {
	unsigned int i=0;
	unsigned char msg2[]="task2 funcB2()\r";
 	while(msg2[i] != '\0'){
		while(!(LPC_UART0->LSR & 0x20));
		LPC_UART0->THR = msg2[i];
		i++;
	}
	i=0;                    
}

/*----------------------------------------------------------------------------
 *        serial_data
 *---------------------------------------------------------------------------*/
void serial_data(unsigned char msg[]) {
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


