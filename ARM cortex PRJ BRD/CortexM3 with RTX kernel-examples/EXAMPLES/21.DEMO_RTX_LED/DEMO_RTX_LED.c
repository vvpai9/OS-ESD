/*----------------------------------------------------------------------------
 *      RL-ARMCTX - RTX
 *----------------------------------------------------------------------------
 *      Name:    DEMO-RTX-LED.c 
 *      Purpose: RTX example program to show semaphore usage
 *----------------------------------------------------------------------------*/

#include <RTL.h>                      	// RTX kernel functions & defines      
#include <LPC17xx.h>                  	// LPC17xx definitions                 
#include <stdio.h>

OS_TID tsk1, tsk2;
OS_SEM semaphore1;

unsigned char msg1[]="TASK1 : LED's L1 to L4\r", msg2[]="TASK2 : LED's L5 to L8\r";
unsigned int i=0, cnt1=0, cnt2=0, delay=0;

// Function prototypes
__task void init_task(void);
__task void task1 (void);
__task void task2 (void);
void UART0_Init(void);

/*-----------------------------------------------------------------------------------
 *   init_task:  RTX Kernel starts this task with os_sys_init_prio(init_task, 10);
 *---------------------------------------------------------------------------------*/
__task void init_task(void) {
	// Initialize the Semaphore before the first use
    os_sem_init (semaphore1, 1);  // token container = 1
    // Create an instance of task1 with priority 8
    tsk1 = os_tsk_create (task1, 8);
    // Create an instance of task2 with default priority 1
    tsk2 = os_tsk_create (task2, 1);
    // Delete the init task
   	os_tsk_delete_self ();
}

/*----------------------------------------------------------------------------
 *    Task 1 - High Priority -  Wait for the free semaphore
 *---------------------------------------------------------------------------*/
__task void task1 (void) {
	OS_RESULT ret;
   	while (1) {
    	// Wait for the free semaphore 
       	ret = os_sem_wait (semaphore1, 0xFFFF);
	   	cnt1++;

      	if ( ret == OS_R_OK) { //A token was available and the function returned immediately
        	// OK, the serial interface is free now, use it. 
   			while (msg1[i] != '\0'){
  				while (!(LPC_UART0->LSR & 0x20));
  				LPC_UART0->THR = msg1[i];
				i++;
			}
			i=0;

  			LPC_GPIO0->FIOCLR = 0x000000F0;		// CLEAR P0.4 to P0.7, LEDs ON
			os_dly_wait(100);
			LPC_GPIO0->FIOSET = 0x000000F0;		// SET P0.4 to P0.7, LEDs OFF
			os_dly_wait(100);
		}
    
      	// Return a token back to a semaphore 
    	os_sem_send (semaphore1);
	}
}

/*----------------------------------------------------------------------------
 *    Task 2 - Low Priority - looks for a free semaphore and uses the resource
 *                            whenever it is available
 *---------------------------------------------------------------------------*/
__task void task2 (void) {
 	OS_RESULT ret;
	while (1) {
       	//Wait for a free semaphore 
        ret = os_sem_wait (semaphore1, 0x00ff);
		cnt2++;
   
   		if ( ret == OS_R_SEM) {   // The calling task has waited until a semaphore became available.
        	// OK, the serial interface is free now, use it. 
     		while (msg2[i] != '\0'){
  				while (!(LPC_UART0->LSR & 0x20));
  				LPC_UART0->THR = msg2[i];
				i++;
			}
			i=0;
         
			LPC_GPIO0->FIOCLR = 0x00000F00;		// CLEAR P0.8 to P0.11, LEDs ON
			os_dly_wait(100);
			LPC_GPIO0->FIOSET = 0x00000F00;		// SET P0.8 to P0.11, LEDs OFF
			os_dly_wait(100);
		}

      	// Return a token back to a semaphore.
    	os_sem_send (semaphore1);
	}
}

/*----------------------------------------------------------------------------
 *        Initialize serial interface
 *---------------------------------------------------------------------------*/
void UART0_Init(void)
{
	LPC_SC->PCONP |= 0x00000008;			// UART0 peripheral enable
	LPC_PINCON->PINSEL0 &= ~0x000000F0;
	LPC_PINCON->PINSEL0 |= 0x00000050;		// P0.2 as TXD0 and P0.3 as RXD0
	LPC_UART0->LCR = 0x00000083;			// enable divisor latch, parity disable, 1 stop bit, 8bit word length
	LPC_UART0->DLM = 0X00; 
	LPC_UART0->DLL = 0xA2;      			// select baud rate 9600 bps
	LPC_UART0->LCR = 0X00000003;
	LPC_UART0->FCR = 0x07;
	LPC_UART0->IER = 0X03;	   				// select Transmit and receive interrupt
}

/*----------------------------------------------------------------------------
 *    Main: Initialize and start RTX Kernel
 *---------------------------------------------------------------------------*/
int main (void) {
	LPC_PINCON->PINSEL0 &= 0xFF0000FF;		// Select P0.4 to P0.11 as GPIO
	LPC_GPIO0->FIODIR |= 0x00000FF0;		// Make P0.4 to P0.11 as output
	LPC_GPIO0->FIOSET = 0x00000FF0;			// Initially set P0.4 to P0.11 to OFF ALL LEDs
	UART0_Init();
  	// Initialize RTX and start init   
   	os_sys_init_prio(init_task, 10);
}
/*----------------------------------------------------------------------------
 * end of file
 *---------------------------------------------------------------------------*/

