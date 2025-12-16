/*-------------------------------------------------------------------------------------------------------
 *      RL-ARMCTX - RTX
 *-------------------------------------------------------------------------------------------------------
 *      Name:    RTX-RLY.C
 *      Purpose: RELAY Controller example program
 *-----------------------------------------------------------------------------------------------------*/
 
#include <RTL.h>                 	// RTX kernel functions & defines 
#include <LPC17xx.H>             	// LPC17xx definitions            

// tsk1, tsk2 will contain task identifications at run-time 
OS_TID tsk1, tsk2;

// Function prototypes
__task void init_task(void); 
__task void task1 (void);
__task void task2 (void);

unsigned int cnt1=0, cnt2=0;
									 
/*------------------------------------------------------------------------------------------------------
 *   init_task:  RTX Kernel starts this task with os_sys_init_prio(init_task, 10);
 *----------------------------------------------------------------------------------------------------*/
__task void init_task(void) {
	tsk1 = os_tsk_create(task1,1); 	// task1 at priority 1 
	tsk2 = os_tsk_create(task2,1); 	// task2 at priority 1
	os_tsk_delete_self(); 			// must delete itself before exiting
}

/*------------------------------------------------------------------------------------------------------
 *   Task 1:  RTX Kernel starts this task with os_tsk_create (task1,1)
 *----------------------------------------------------------------------------------------------------*/
__task void task1 (void) {
	os_itv_set (200);     			// set wait interval:  2second       
	for (;;) {
	    os_itv_wait ();
	    /* do some actions at regular time intervals */
		/* Relay ON for every interval of 2sec */
		cnt1++;	  // testing purpose 
		LPC_GPIO0->FIOSET = 0x002000000;	// relay on
	}
}

/*------------------------------------------------------------------------------------------------------
 *   Task 2:  RTX Kernel starts this task with os_tsk_create (task2,1)
 *----------------------------------------------------------------------------------------------------*/
__task void task2 (void) {
	os_itv_set (200);             	// set wait interval:  2second       
	for (;;) {
	    os_itv_wait ();
	    /* do some actions at regular time intervals */
		/* Relay OFF for every interval of 2sec */
		cnt2++;	  // testing purpose 
		LPC_GPIO0->FIOCLR = 0x002000000;	// relay off
	}
}

/*------------------------------------------------------------------------------------------------------
 *        Main: Initialize and start RTX Kernel
 *----------------------------------------------------------------------------------------------------*/
int main (void) {                     		// program execution starts here       
	LPC_PINCON->PINSEL1 &= 0xFFF3FFFF;		// P0.25 GPIO
	LPC_GPIO0->FIODIR |= 0x002000000;		// P0.25 output
	os_sys_init_prio(init_task, 10);    
} 

/*------------------------------------------------------------------------------------------------------
 *        END
 *----------------------------------------------------------------------------------------------------*/



