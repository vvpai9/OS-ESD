/*----------------------------------------------------------------------------
 *      RL-ARM - RTX
 *----------------------------------------------------------------------------
 *      Name:    RTX-BARRIER-SEMAPHORE.c
 *      Purpose: RTX example program to demonstrate RTX-BARRIER-SEMAPHORE 
 *----------------------------------------------------------------------------*/

#include <RTL.h>                      	/* RTX kernel functions & defines      */
#include <LPC17xx.h>                  	/* LPC17xx definitions                 */
#include <stdio.h>

// tsk1, tsk2 will contain task identifications at run-time 
OS_TID tsk1, tsk2, tsk3, tsk4;

OS_SEM Mutex, Barrier;
unsigned int count=0, n=4;

// Function prototypes
__task void init_task(void); 
__task void task1 (void);
__task void task2 (void);
__task void task3 (void);
__task void task4 (void);
void init_serial(void);
void serial_data(unsigned char msg[]);

/*-----------------------------------------------------------------------------------
 *   init_task:  RTX Kernel starts this task with os_sys_init_prio(init_task, 10);
 *---------------------------------------------------------------------------------*/
__task void init_task(void) {
	os_sem_init(Mutex, 1);
	os_sem_init(Barrier, 0);
	
  	// Create an instance of task1 with priority 1 
  	tsk1 = os_tsk_create (task1, 1);
  	// Create an instance of task2 with priority 1 
  	tsk2 = os_tsk_create (task2, 1);
	// Create an instance of task3 with priority 1 
  	tsk3 = os_tsk_create (task3, 1);	
	// Create an instance of task4 with priority 1 
  	tsk4 = os_tsk_create (task4, 1);

	os_tsk_delete_self ();	 // delete this task
}

/*----------------------------------------------------------------------------
 *    Task 1 
 *---------------------------------------------------------------------------*/
__task void task1 (void) {
	unsigned char msg1[]="Task1 Running\r", arr1[]="Task1 Blocked at Barrier\r";

	while (1) {
		os_sem_wait (Mutex, 0xffff); 
		count = count+1;
		os_sem_send (Mutex); 
		
		if (count==(n-(n-1))) {
			serial_data(arr1);		
		}
		
		// task1 is at the Barrier 
		os_sem_wait (Barrier, 0xffff); // 
		serial_data(msg1);

		// critical point
		os_sem_send (Barrier);
		os_dly_wait(10);
	}
}	

/*----------------------------------------------------------------------------
 *    Task 2 
 *---------------------------------------------------------------------------*/
__task void task2 (void) {
	unsigned char msg2[]="Task2 Running\r", arr2[]="Task2 Blocked at Barrier\r";;
	while (1) {
		os_sem_wait (Mutex, 0xffff); 
		count = count+1;
		os_sem_send (Mutex); 
		
		if (count==(n-(n-2))) {
			serial_data(arr2);
		}
	
		// task2 is at the Barrier 
		os_sem_wait (Barrier, 0xffff); // 
		os_sem_send (Barrier);
		
		// critical point
		serial_data(msg2);
		os_dly_wait(10);
	}
}

/*----------------------------------------------------------------------------
 *    Task 3 
 *---------------------------------------------------------------------------*/
__task void task3 (void) {
	unsigned char msg3[]="Task3 Running\r", arr3[]="Task3 Blocked at Barrier\r";;
	while (1) {
		os_sem_wait (Mutex, 0xffff); 
		count = count+1;
		os_sem_send (Mutex); 
		
		if (count==(n-(n-3))) {
			serial_data(arr3);
		}
	
		// task3 is at the Barrier 
		os_sem_wait (Barrier, 0xffff); // 
		os_sem_send (Barrier);
		
		// critical point
		serial_data(msg3);
		os_dly_wait(10);
	}
}

/*----------------------------------------------------------------------------
 *    Task 4 
 *---------------------------------------------------------------------------*/
__task void task4 (void) {
	unsigned char msg4[]="Task4 Running\r", arr4[]="Task4 Blocked at Barrier\r";;
	while (1) {
		os_sem_wait (Mutex, 0xffff); 
		count = count+1;
		os_sem_send (Mutex); 
		
		if (count==(n-(n-4))) {
			os_sem_send (Barrier);
			serial_data(arr4);
		}
		// task4 is at the Barrier and releases the Barrier for all the task
		os_sem_wait (Barrier, 0xffff); // 
		os_sem_send (Barrier);
		
		// critical point
		serial_data(msg4);
		os_dly_wait(10);
	}
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


