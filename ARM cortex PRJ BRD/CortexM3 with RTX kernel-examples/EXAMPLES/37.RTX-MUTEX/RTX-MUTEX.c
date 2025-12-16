/*------------------------------------------------------------------------------
 *      RL-ARMCTX - RTX								  
 *------------------------------------------------------------------------------
 *      Name:    RTX-MUTEX.c 
 *      Purpose: RTX example to understand mutex concept
 *----------------------------------------------------------------------------*/

#include <rtl.h>
#include <lpc17xx.h>
#include <stdio.h>

OS_TID tsk1,tsk2;
OS_MUT mutex1;

unsigned int i, m_var=0;
char buf1[15], buf2[15];

/* Function prototypes */
__task void init_task(void);
__task void task1(void);
__task void task2(void);
void init_serial(void);
void f1(void);
void f2(void);

/*-----------------------------------------------------------------------------------
 *   init_task:  RTX Kernel starts this task with os_sys_init_prio(init_task, 10);
 *---------------------------------------------------------------------------------*/
__task void init_task(void) {
	os_mut_init(mutex1);
	tsk1 = os_tsk_create(task1,1); 	// task1 at priority 1 
	tsk2 = os_tsk_create(task2,1); 	// task2 at priority 1
	os_tsk_delete_self(); 			// must delete itself before exiting
}

/*----------------------------------------------------------------------------
 *        f1() definition
 *---------------------------------------------------------------------------*/
void f1 (void) {
  	os_mut_wait (mutex1, 0xffff);
  	// ..
  	/* Critical region 1 */
	m_var++;	// global variable
  	sprintf(buf1,"m_var value is now: %d\r", m_var);
	while(buf1[i] != '\0'){		  		// Displaying on hyper terminal
		while(!(LPC_UART0->LSR & 0x20));
	  	LPC_UART0->THR = buf1[i];		  		
		i++;
	}
	i=0;	 
  	/* f2() will not block the task1 */
	sprintf(buf1,"f1(); calling f2();\r");
	while(buf1[i] != '\0'){		  		// Displaying on hyper terminal
		while(!(LPC_UART0->LSR & 0x20));
	  	LPC_UART0->THR = buf1[i];		  		
		i++;
	}
	i=0;
  	f2();
	os_mut_release (mutex1);
}

/*----------------------------------------------------------------------------
 *        f2() definition
 *---------------------------------------------------------------------------*/
void f2 (void) {
  	os_mut_wait (mutex1, 0xffff);
  	// ..
  	/* Critical region 2 */
  	m_var += 4;  	// global variable
  	sprintf(buf2,"m_var value is now: %d\r", m_var);
	while(buf2[i] != '\0'){		  		// Displaying on hyper terminal
		while(!(LPC_UART0->LSR & 0x20));
	  	LPC_UART0->THR = buf2[i];;		  		
		i++;
	}
	i=0;
	os_mut_release (mutex1);
}

/*----------------------------------------------------------------------------
 *   Task 1:  RTX Kernel starts this task with os_tsk_create (task1,1)
 *---------------------------------------------------------------------------*/
__task void task1(void) {
	while(1) {
		sprintf(buf2,"Task1 calling f1();\r");
		while(buf2[i] != '\0'){		  		// Displaying on hyper terminal
			while(!(LPC_UART0->LSR & 0x20));
	  		LPC_UART0->THR = buf2[i];		  		
			i++;
		}
		i=0;
		f1();
		os_dly_wait(50);
	}
}

/*----------------------------------------------------------------------------
 *   Task 2:  RTX Kernel starts this task with os_tsk_create (task2,1)
 *---------------------------------------------------------------------------*/
__task void task2(void) {
	while(1) {
		sprintf(buf2,"Task2 calling f2();\r");
		while(buf2[i] != '\0'){		  		// Displaying on hyper terminal
			while(!(LPC_UART0->LSR & 0x20));
	  		LPC_UART0->THR = buf2[i];		  		
			i++;
		}
		i=0;
 		f2();
		os_dly_wait(50);
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
