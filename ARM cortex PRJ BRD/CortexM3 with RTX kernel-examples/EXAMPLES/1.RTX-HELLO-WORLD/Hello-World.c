/*---------------------------------------------------------------------------------------------------------
 *      RL-ARMCTX - RTX
 *---------------------------------------------------------------------------------------------------------
 *      Name:    Hello-World.c
 *      Purpose: Creating tasks using os_tsk_create(); function
 *--------------------------------------------------------------------------------------------------------*/
#include <LPC17xx.h>
#include <RTL.h>
#include <stdio.h>

// tsk1, tsk2 will contain task identifications at run-time
OS_TID tsk1, tsk2;

// Function prototypes 
__task void init_task(void);
__task void task1(void);
__task void task2(void);
void UART0_Init(void);

char msg1[10];
char msg2[] = "Hello world!\r";
unsigned int i=0, cnt1=0, cnt2=0;

/*------------------------------------------------------------------------------------------------------------
 *   init_task:  RTX Kernel starts this task with os_sys_init_prio(init_task, 10);
 *----------------------------------------------------------------------------------------------------------*/
__task void init_task(void) {
	tsk1 = os_tsk_create(task1,2); 	// task1 at priority 2 
	tsk2 = os_tsk_create(task2,1); 	// task2 at priority 1
	os_tsk_delete_self(); 			// must delete itself before exiting
}

/*------------------------------------------------------------------------------------------------------------
 *   Task 1:  RTX Kernel starts this task with os_tsk_create (task1,1)
 *----------------------------------------------------------------------------------------------------------*/
__task void task1(void) {
	while(1) {
		cnt1++;
		sprintf( msg1,"Task 1: %d\r", cnt1 );
		while(msg1[i] != '\0'){
	  		while (!(LPC_UART0->LSR & 0x20));
	  		LPC_UART0->THR = msg1[i];
			i++;
		}
		i=0;  
		os_dly_wait(100);					// 1sec os delay
	}
}

/*-------------------------------------------------------------------------------------------------------------
 *   Task 2:  RTX Kernel starts this task with os_tsk_create (task2,2)
 *-----------------------------------------------------------------------------------------------------------*/
__task void task2( void ) {
	while(1) {
		cnt2++;
		while(msg2[i] != '\0'){
	  		while(!(LPC_UART0->LSR & 0x20));
	  		LPC_UART0->THR = msg2[i];
			i++;
		}
		i=0;  
		os_dly_wait(100);					// 1sec os delay
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
	UART0_Init();	  						// Initialize serial interface
	os_sys_init_prio(init_task, 10);		// Initialize init_task and start RTX Kernel
}
/*---------------------------------------------------------------------------------------------------------------
 * end of file
 *-------------------------------------------------------------------------------------------------------------*/
