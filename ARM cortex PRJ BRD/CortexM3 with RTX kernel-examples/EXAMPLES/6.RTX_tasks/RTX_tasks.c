/*--------------------------------------------------------------------------------------------------------------------------------------
 *      RL-ARMCTX - RTX
 *--------------------------------------------------------------------------------------------------------------------------------------
 *      Name:    RTX_tasks.c
 *      Purpose: RTX example to show task switching
 *------------------------------------------------------------------------------------------------------------------------------------*/

#include <RTL.h>		// RTX kernel functions & defines      
#include <lpc17xx.h>
#include <stdio.h>

// tsk1, tsk2, tsk3, tsk4 will contain task identifications at run-time 
OS_TID tsk1, tsk2, tsk3, tsk4;

U16 counter1;          // counter for task 1                  
U16 counter2;          // counter for task 2                  
U16 counter3;          // counter for task 3                  
U16 counter4;          // counter for task 4                  

char arr1[20],arr2[20],arr3[20],arr4[20];
unsigned int i = 0;

// Function prototypes
__task void init_task(void); 
__task void task1 (void);
__task void task2 (void);
__task void task3 (void);
__task void task4 (void);
void UART0_Init(void);

/*--------------------------------------------------------------------------------------------------------------------------------------
 *   init_task:  RTX Kernel starts this task with 	os_sys_init_prio(init_task, 10);
 *------------------------------------------------------------------------------------------------------------------------------------*/
__task void init_task(void) {
	tsk1 = os_tsk_create(task1,2); 			// task1 at priority 2 
	tsk2 = os_tsk_create(task2,2); 			// task2 at priority 2
	tsk3 = os_tsk_create(task3,1); 			// task3 at priority 1 
	tsk4 = os_tsk_create(task4,1); 			// task4 at priority 1
	os_tsk_delete_self(); 					// must delete itself before exiting
}

/*--------------------------------------------------------------------------------------------------------------------------------------
 *   Task 1:  RTX Kernel starts this task with os_tsk_create (task1,2) 
 *------------------------------------------------------------------------------------------------------------------------------------*/
__task void task1 (void) {
	while (1) {                         	// endless loop                       
    	counter1++;                       	// increment counter 1                
		sprintf(arr1,"counter1:%d\r",counter1);
  		while (arr1[i] != '\0'){
		  	while (!(LPC_UART0->LSR & 0x20));
			LPC_UART0->THR = arr1[i];
			i++;
		}
		i=0;
    	os_dly_wait (5);                  	// wait for timeout: 5 ticks           
	}
}

/*--------------------------------------------------------------------------------------------------------------------------------------
 *   Task 2 :  RTX Kernel starts this task with os_tsk_create (task2,2)
 *------------------------------------------------------------------------------------------------------------------------------------*/
__task void task2 (void) {             		// higher priority to preempt job3     
	while (1) {                         	// endless loop                        
    	counter2++;                       	// increment counter 2                 
		sprintf(arr2,"counter2:%d\r",counter2);
  		while (arr2[i] != '\0'){
		  	while (!(LPC_UART0->LSR & 0x20));
			LPC_UART0->THR = arr2[i];
			i++;
		}
		i=0;
    	os_dly_wait (10);                 	// wait for timeout: 10 ticks          
  	}
}

/*--------------------------------------------------------------------------------------------------------------------------------------
 *   Task 3 :  RTX Kernel starts this task with os_tsk_create (task3,1)
 *------------------------------------------------------------------------------------------------------------------------------------*/
__task void task3 (void) {
	while (1)  {                        	// endless loop                        
    	counter3++;                       	// increment counter 3                 
		sprintf(arr3,"counter3:%d\r",counter3);
  		while (arr3[i] != '\0'){
		  	while (!(LPC_UART0->LSR & 0x20));
			LPC_UART0->THR = arr3[i];
			i++;
		}
		i=0;
    	if (counter3 == 0x00ff) {           // signal overflow of counter 3       
			counter3 = 0;	
    		os_evt_set (0x0001,tsk4);       // to task 4   
			os_tsk_pass ();               	// because of same priority task3                         
    	}
  	}
}

/*--------------------------------------------------------------------------------------------------------------------------------------
 *   Task 4 :  RTX Kernel starts this task with os_tsk_create (task4,1)
 *------------------------------------------------------------------------------------------------------------------------------------*/
__task void task4 (void) {
	while (1) {                         	// endless loop                        
    	os_evt_wait_or (0x0001, 0x00ff);  	// wait for signal event               
    	counter4++;                       	// process overflow from counter 3     
		sprintf(arr4,"counter4:%d\r",counter4);
  		while (arr4[i] != '\0'){
		  	while (!(LPC_UART0->LSR & 0x20));
			LPC_UART0->THR = arr4[i];
			i++;
		}
		i=0;
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

