/*--------------------------------------------------------------------------------------------------------------------------------------
 *      RL-ARMCTX - RTX
 *--------------------------------------------------------------------------------------------------------------------------------------
 *      Name:    RTX-RLY-LED.c 
 *      Purpose: RTX example program to show task switching by changing priority
 *------------------------------------------------------------------------------------------------------------------------------------*/
#include <RTL.h>                    		/* RTX kernel functions & defines */
#include <LPC17xx.h>                		/* LPC17xx definitions            */
#include <stdio.h>

// tsk1, tsk2 will contain task identifications at run-time 
OS_TID tsk1, tsk2;

// Function prototypes
__task void init_task(void); 
__task void task1 (void);
__task void task2 (void);

unsigned char msg1[]= "[1]Task1 Enters, Changes Priority of Task2\r", msg2[]= "[2]Task2 Priority Increased, Running\r";
unsigned char msg3[]= "[3]Task2 Decreases its Priority, Task1 is Now at Higher Priority\r";
unsigned int i=0, cnt1=0, cnt2=0;

/*--------------------------------------------------------------------------------------------------------------------------------------
 *   init_task:  RTX Kernel starts this task with os_sys_init_prio(init_task, 10);
 *------------------------------------------------------------------------------------------------------------------------------------*/
__task void init_task(void) {
	tsk1 = os_tsk_create(task1,5); 	// task1 at priority 5 
	tsk2 = os_tsk_create(task2,2); 	// task2 at priority 2
	os_tsk_delete_self(); 			// must delete itself before exiting
}

/*--------------------------------------------------------------------------------------------------------------------------------------
 *   Task 1:  RTX Kernel starts this task with os_tsk_create (task1,5)
 *------------------------------------------------------------------------------------------------------------------------------------*/
__task void task1 (void) {
	while (1) {
    	cnt1++;
		while (msg1[i] != '\0'){
  			while (!(LPC_UART0->LSR & 0x20));
  			LPC_UART0->THR = msg1[i];
			i++;
		}
		i=0;
		
		// Changing the priority of task2 will cause a task switch. 
		os_tsk_prio(tsk2, 10);
	
		while (msg3[i] != '\0'){
  			while (!(LPC_UART0->LSR & 0x20));
  			LPC_UART0->THR = msg3[i];
			i++;
		}
		i=0;
  	}
}

/*--------------------------------------------------------------------------------------------------------------------------------------
 *   Task 2:  RTX Kernel starts this task with os_tsk_create (task2,2)
 *------------------------------------------------------------------------------------------------------------------------------------*/
__task void task2 (void) {
  	while (1) {
    	cnt2++;
		while (msg2[i] != '\0'){
  			while (!(LPC_UART0->LSR & 0x20));
  			LPC_UART0->THR = msg2[i];
			i++;
		}
		i=0;
	
		// 	Change priority of this task will cause task switch. 
		os_tsk_prio_self (2);
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

