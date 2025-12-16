/*----------------------------------------------------------------------------
 *      RL-ARMCTX - RTX
 *----------------------------------------------------------------------------
 *      Name:    RTX-BUZZER.C
 *      Purpose: BUZZER Controlling example program
 *----------------------------------------------------------------------------*/
 
#include <RTL.h>                 	// RTX kernel functions & defines 
#include <LPC17xx.H>             	// LPC17xx definitions            

// tsk1, tsk2 will contain task identifications at run-time 
OS_TID tsk1, tsk2;

// Function prototypes
__task void init_task(void); 
__task void task1 (void);
__task void task2 (void);

unsigned int cnt1=0, cnt2=0;

/*-----------------------------------------------------------------------------------
 *   init_task:  RTX Kernel starts this task with os_sys_init_prio(init_task, 10);
 *---------------------------------------------------------------------------------*/
__task void init_task(void) {
	tsk1 = os_tsk_create(task1,1); 	// task1 at priority 1 
	tsk2 = os_tsk_create(task2,1); 	// task2 at priority 1
	os_tsk_delete_self(); 			// must delete itself before exiting
}

/*----------------------------------------------------------------------------
 *   Task 1:  RTX Kernel starts this task with os_tsk_create (task1,1)
 *---------------------------------------------------------------------------*/									 
__task void task1 (void) {
	os_itv_set (100);     			// set wait interval:  1 second       
	for (;;) {
	    os_itv_wait ();
	    /* do some actions at regular time intervals */
		/* Buzzer ON for 1sec */
		cnt1++;
		LPC_GPIO0->FIOSET = 0x01000000;	//P0.24 made high to turn ON the buzzer
	}
}

/*----------------------------------------------------------------------------
 *   Task 2:  RTX Kernel starts this task with os_tsk_create (task2,1)
 *---------------------------------------------------------------------------*/
__task void task2 (void) {
	os_itv_set (100);             	// set wait interval:  1 second       
	for (;;) {
	    os_itv_wait ();
	    /* do some actions at regular time intervals */
		/* Buzzer OFF for 1sec */
		cnt2++;
		LPC_GPIO0->FIOCLR = 0x01000000;	//P0.24 made low to turn OFF the buzzer
	}
}

/*----------------------------------------------------------------------------
 *        Main: Initialize and start RTX Kernel
 *---------------------------------------------------------------------------*/
int main (void) {                     /* program execution starts here       */
	LPC_PINCON->PINSEL1 &= 0xFFFCFFFF;	//P0.24 is GPIO
	LPC_GPIO0->FIODIR |= 0x01000000;	//P0.24 is output
	os_sys_init_prio(init_task, 10); 
} 
/*----------------------------------------------------------------------------
 *        END
 *---------------------------------------------------------------------------*/



