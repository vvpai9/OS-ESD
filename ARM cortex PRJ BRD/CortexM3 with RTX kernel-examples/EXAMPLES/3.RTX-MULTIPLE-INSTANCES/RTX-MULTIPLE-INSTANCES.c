/*--------------------------------------------------------------------------------------------------------------------------------------
 *      RL-ARMCTX - RTX
 *--------------------------------------------------------------------------------------------------------------------------------------
 *      Name:    RTX-MULTIPLE-INSTANCES.c
 *      Purpose: Creating multiple instances of same task and also passing an argument 
 *      	     to task, An argument to a task can be useful to differentiate between multiple
 *               instances of the same task.
 *------------------------------------------------------------------------------------------------------------------------------------*/
#include <RTL.h>                      /* RTX kernel functions & defines      */
#include <LPC17xx.H> 
#include <stdio.h>

/* tid, tsk_1, tsk2_1, tsk2_2 will contain task identifications at run-time */
OS_TID tid, tsk1, tsk_1, tsk2_1, tsk2_2;

/* Function prototypes */
__task void init_task(void);
__task void task1 (void);
__task void task2(void *argv);

char buf[50];
int param[2] = {0, 1};
unsigned int i=0,cnt1=0,cnt2=0;

/*------------------------------------------------------------------------------------------------------------------------------------
 *   init_task:  RTX Kernel starts this task with os_sys_init_prio(init_task, 10);
 *------------------------------------------------------------------------------------------------------------------------------------*/
__task void init_task(void) {
	tsk1 = os_tsk_create(task1,1); 	// task1 at priority 1 
	os_tsk_delete_self(); 			// must delete itself before exiting
}
 
/*------------------------------------------------------------------------------------------------------------------------------------
 *   Task 1:  RTX Kernel starts this task with os_tsk_create (task1,1)
 *------------------------------------------------------------------------------------------------------------------------------------*/
__task void task1 (void){
	cnt1++;
	/* This task will create 2 instances of task2 */
	tsk2_1 = os_tsk_create_ex (task2, 1, &param[0]);
  	tsk2_2 = os_tsk_create_ex (task2, 1, &param[1]);
  	/* The job is done, delete 'task1' */
	os_tsk_delete_self ();
}

/*--------------------------------------------------------------------------------------------------------------------------------------
 *   Task 2:  RTX Kernel starts this task with os_tsk_create_ex (task2, 1, &param[0]) and also os_tsk_create_ex (task2, 1, &param[1]);;
 *------------------------------------------------------------------------------------------------------------------------------------*/
__task void task2(void *argv) {
	/* ... place code for task2 activity here ... */
	cnt2++;

	switch (*(int *)argv) {
    	case 0:
	  			tid = os_tsk_self();	// get TID of task
				sprintf (buf, "This is a FIRST instance of task2:OS-TID = %d\r",tid);
				while (buf[i] != '\0'){
	  				while (!(LPC_UART0->LSR & 0x20));
	  				LPC_UART0->THR = buf[i];
					i++;
				}
				i=0;;
      			break;
    	
		case 1:
      			tid = os_tsk_self();	// get TID of task
				sprintf (buf, "This is a SECOND instance of task2:OS-TID = %d\r",tid);
				while (buf[i] != '\0'){
	  				while (!(LPC_UART0->LSR & 0x20));
	  				LPC_UART0->THR = buf[i];
					i++;
				}
				i=0;
				break;
	}
	/* The job is done, delete 'task2' */
	os_tsk_delete_self (); 
}

/*--------------------------------------------------------------------------------------------------------------------------------------
 *        Initialize serial interface										 
 *------------------------------------------------------------------------------------------------------------------------------------*/
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

/*--------------------------------------------------------------------------------------------------------------------------------------
 *        Main: Initialize and start RTX Kernel
 *------------------------------------------------------------------------------------------------------------------------------------*/
int main(void) {
	UART0_Init();		  					// Initialize serial interface
	os_sys_init_prio(init_task, 10);		// Initialize init_task and start RTX Kernel
}
/*--------------------------------------------------------------------------------------------------------------------------------------
 * end of file
 *------------------------------------------------------------------------------------------------------------------------------------*/


