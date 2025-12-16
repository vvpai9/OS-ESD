/*--------------------------------------------------------------------------------------------------------------------------------------
 *      RL-ARMCTX - RTX
 *--------------------------------------------------------------------------------------------------------------------------------------
 *      Name:    RoundRobin.C
 *      Purpose: Demonstration of RoundRobin Task switching
 *------------------------------------------------------------------------------------------------------------------------------------*/
#include <RTL.h>                      // RTX kernel functions & defines      
#include <LPC17xx.H> 
#include <stdio.h>

// tsk1, tsk2, tsk3, tsk4 will contain task identifications at run-time 
OS_TID tsk1, tsk2, tsk3, tsk4;
															  
unsigned short counter1;                         // counter for task 1                  
unsigned short counter2;                         // counter for task 2                  
unsigned short counter3;                         // counter for task 3                  
unsigned short counter4;                         // counter for task 3                  

char buf[20],i=0;

// Function prototypes
void UART0_Init(void);
__task void init_task(void); 
__task void task1 (void);
__task void task2 (void);
__task void task3 (void);
__task void task4 (void);

/*--------------------------------------------------------------------------------------------------------------------------------------
 *   init_task:  RTX Kernel starts this task with 	os_sys_init_prio(init_task, 10);
 *------------------------------------------------------------------------------------------------------------------------------------*/
__task void init_task(void) {
	tsk1 = os_tsk_create(task1,2); 	// task1 at priority 2 
	tsk2 = os_tsk_create(task2,2); 	// task2 at priority 2
	tsk3 = os_tsk_create(task3,4); 	// task1 at priority 4 
	tsk4 = os_tsk_create(task4,1); 	// task2 at priority 1
	os_tsk_delete_self(); 			// must delete itself before exiting
}

/*--------------------------------------------------------------------------------------------------------------------------------------
 *   Task 1:  RTX Kernel starts this task with os_tsk_create (task1,2) 
 *------------------------------------------------------------------------------------------------------------------------------------*/
__task void task1 (void) {
  	while (1)  {                        // endless loop                        
  		counter1++; 
		sprintf(buf,"counter1:%d",counter1);
	  	while (buf[i] != '\0'){
			while (!(LPC_UART0->LSR & 0x20));
			LPC_UART0->THR = buf[i];
			i++;
		}
		i=0;  
	
		while (!(LPC_UART0->LSR & 0x20));
  		LPC_UART0->THR = '\r';		  
  	}
}
 
/*--------------------------------------------------------------------------------------------------------------------------------------
 *   Task 2:  RTX Kernel starts this task with os_tsk_create (task2,2)
 *------------------------------------------------------------------------------------------------------------------------------------*/
__task void task2 (void) {
  	while (1)  {                        /* endless loop                        */
  		counter2++;                       /* increment counter 2                 */
		sprintf(buf,"counter2:%d",counter2);
    	while (buf[i] != '\0'){
			while (!(LPC_UART0->LSR & 0x20));
		  	LPC_UART0->THR = buf[i];
			i++;
		}
		i=0;  
	
		while (!(LPC_UART0->LSR & 0x20));
  		LPC_UART0->THR = '\r';		  
    
		if ((counter2 & 0x00FF) == 0x00ff) {
			counter2 = 0;
    		os_evt_set (0x0004, tsk3);
    	}
  	}
}

/*--------------------------------------------------------------------------------------------------------------------------------------
 *   Task 3:  RTX Kernel starts this task with os_tsk_create (task3,4)
 *------------------------------------------------------------------------------------------------------------------------------------*/
__task void task3 (void) {
  	while (1) {                         /* endless loop                        */
    	os_evt_wait_or (0x0004, 0x00ff);
		counter3++;                       /* increment counter 3                 */
		sprintf(buf,"counter3:%d",counter3);
    	while (buf[i] != '\0'){
			while (!(LPC_UART0->LSR & 0x20));
			LPC_UART0->THR = buf[i];
			i++;
		}
		i=0;  
		while (!(LPC_UART0->LSR & 0x20));
  		LPC_UART0->THR = '\r';		  
  	}
}
 
/*--------------------------------------------------------------------------------------------------------------------------------------
 *   Task 4:  RTX Kernel starts this task with os_tsk_create (task4,1)
 *------------------------------------------------------------------------------------------------------------------------------------*/
__task void task4 (void) {
	/* This task is never executed. After starting it stays ready forever. */
  	while (1)  {                        /* endless loop                        */
  		counter4++;
  	}
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


