/*----------------------------------------------------------------------------
 *      RL-ARM - RTX
 *----------------------------------------------------------------------------
 *      Name:    RTX-EXCLUSIVE-QUEUE-SEMAPHORE.c
 *      Purpose: RTX example that implements EXCLUSIVE-QUEUE using SEMAPHORE 
 *----------------------------------------------------------------------------*/

#include <RTL.h>                      	/* RTX kernel functions & defines      */
#include <LPC17xx.h>                  	/* LPC17xx definitions                 */
#include <stdio.h>

// tsk1, tsk2 will contain task identifications at run-time 
OS_TID tsk1, tsk2;

OS_SEM mutex, leaderQueue, followerQueue;
unsigned int leaders=0, followers=0, delay=0;
char buf[20];

// Function prototypes
__task void init_task(void); 
__task void leader(void);
__task void follower(void);
void init_serial(void);
void serial_data(char msg[]);

/*-----------------------------------------------------------------------------------------
 *   init_task:  RTX Kernel starts this task with os_sys_init_prio(init_task, 10);
 *----------------------------------------------------------------------------------------*/
__task void init_task(void) {
	os_sem_init(mutex, 1);
	os_sem_init(leaderQueue, 0);
	os_sem_init(followerQueue, 0);
	
  	// Create an instance of task1 with priority 1 
  	tsk1 = os_tsk_create (leader, 1);
  	// Create an instance of task2 with priority 1 
  	tsk2 = os_tsk_create (follower, 1);

	os_tsk_delete_self ();	 // delete this task
}

/*-----------------------------------------------------------------------------------------
 *    Task 1 
 *----------------------------------------------------------------------------------------*/
__task void leader (void) {
	char msg1[]="Decrements a followers, signals a follower and then invokes dance\r";
	//	char msg2[]="leader on dance floor\r";
	while (1) {
		os_sem_wait (mutex, 0xffff); 

		if(followers > 0) {
			sprintf(buf,"follower waiting:%d\r",followers);
			serial_data(buf);
			followers--;
			serial_data(msg1);
			os_sem_send (followerQueue);
		}
		
		else {
			leaders++;
			os_sem_send (mutex);
			os_sem_wait (leaderQueue, 0xffff); 
		}

		//	Invoke Dance
		//	serial_data(msg2);
		LPC_GPIO0->FIOCLR = 0x000000F0;					// CLEAR P0.4 to P0.7,, LEDs L1 - L4 ON
		LPC_GPIO0->FIOSET = 0x00000F00;					// SET P0.8 to P0.11, LEDs L5 - L8 OFF
		for(delay=0;delay<50000000;delay++);		// delay
		os_sem_send (mutex);
	}
}	

/*---------------------------------------------------------------------------------------------
 *    Task 2 																  
 *--------------------------------------------------------------------------------------------*/
__task void follower (void) {
	char msg1[]="Decrements a leaders, signals a leader and then invokes dance\r";
	//char msg2[]="follower on dance floor\r";
	while (1) {
		os_sem_wait (mutex, 0xffff); 

		if(leaders > 0) {
			sprintf(buf,"leaders waiting:%d\r",leaders);
			serial_data(buf);	 
			leaders--;
			serial_data(msg1);
			os_sem_send (leaderQueue);
		}

		else {
			followers++;
			os_sem_send (mutex);
			os_sem_wait (followerQueue, 0xffff); 
		}
		
		//	Invoke Dance
		//	serial_data(msg2);
		LPC_GPIO0->FIOCLR = 0x00000F00;					// CLEAR P0.8 to P0.11, LEDs L5 - L8 are ON
		LPC_GPIO0->FIOSET = 0x000000F0;					// SET P0.4 to P0.7, LEDs OFF, LEDs L1 - L4 are OFF
		for(delay=0;delay<50000000;delay++);		// delay
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
	LPC_PINCON->PINSEL0 &= 0xFF0000FF;		// Select P0.4 to P0.11 as GPIO
	LPC_GPIO0->FIODIR |= 0x00000FF0;		// Make P0.4 to P0.11 as output
	LPC_GPIO0->FIOSET = 0x00000FF0;			// Initially set P0.4 to P0.11 to OFF ALL LEDs
	UART0_Init();	  						// Initialize serial interface
	os_sys_init_prio(init_task, 10);		// Initialize init_task and start RTX Kernel
}
/*---------------------------------------------------------------------------------------------------------------
 * end of file
 *-------------------------------------------------------------------------------------------------------------*/


