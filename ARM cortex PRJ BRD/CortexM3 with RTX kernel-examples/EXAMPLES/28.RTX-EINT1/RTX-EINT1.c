/*-------------------------------------------------------------------------------
 *      RL-ARMCTX - RTX
 *-------------------------------------------------------------------------------
 *      Name:    RTX-EINT1.c
 *      Purpose: Creating RTX application - creating tasks and do task switching
 *------------------------------------------------------------------------------*/

#include <LPC17xx.h>
#include <RTL.h>
#include <stdio.h>

#define EVT_KEY 0x0001
OS_TID tsk1;

/* Function prototypes */
__task void init_task(void);
__task void task1(void);
void EINT1_IRQHandler(void);
void EINT1_Init(void);

unsigned char cnt1=0,flag=0;

/*-----------------------------------------------------------------------------------
 *   init_task:  RTX Kernel starts this task with os_sys_init_prio(init_task, 10);
 *---------------------------------------------------------------------------------*/
__task void init_task(void) {
	tsk1 = os_tsk_create(task1,1); 				// task1 at priority 1 
	os_tsk_delete_self(); 						// must delete itself before exiting
}

/*----------------------------------------------------------------------------
 *   Task 1:  RTX Kernel starts this task with os_tsk_create (task1,1)
 *---------------------------------------------------------------------------*/
__task void task1(void) {

	while(1) {
		os_evt_wait_or( EVT_KEY , 0xffff);
		cnt1++;

		if(flag == 0x00)					// when flag is 0x00 turn ON the LED
		{
			LPC_GPIO2->FIOSET = 0x00001000;
			flag = 0xFF;
		}									
	
		else							   // when flag is 0xFF turn OFF the LED
		{
			LPC_GPIO2->FIOCLR = 0x00001000;
			flag = 0x00;
		}
	}			 
}

/*----------------------------------------------------------------------------
 *  External Interrupt1 initialisation
 *---------------------------------------------------------------------------*/
void EINT1_Init(void) {
	LPC_PINCON->PINSEL4 &= ~0x00C00000;
	LPC_PINCON->PINSEL4 |= 0x00400000;	// P2.11 as EINT1

	LPC_PINCON->PINSEL4 &= 0xFCFFFFFF;	// P2.12 GPIO for LED
	LPC_GPIO2->FIODIR = 0x00001000;		// P2.12 is assigned output
	
	LPC_SC->EXTINT = 0x00000002;		// writing 1 cleares the interrupt, get set if there is interrupt
	LPC_SC->EXTMODE = 0x00000002;		// EINT1 is initiated as edge senitive, 0 for level sensitive
	LPC_SC->EXTPOLAR = 0x00000000;		// EINT1 is falling edge sensitive, 1 for rising edge
										// above registers, bit0-EINT0, bit1-EINT1, bit2-EINT2,bit3-EINT3	
	NVIC_EnableIRQ(EINT1_IRQn);			
}

/*----------------------------------------------------------------------------
 *  External Interrupt1 Handler 
 *---------------------------------------------------------------------------*/
void EINT1_IRQHandler(void) {
	isr_evt_set(EVT_KEY , tsk1);		// set an event for task1
	LPC_SC->EXTINT = 0x00000002;		// cleares the interrupt
}

/*----------------------------------------------------------------------------
 *        Main: Initialize and start RTX Kernel
 *---------------------------------------------------------------------------*/
int main (void) {
	EINT1_Init();
	os_sys_init_prio(init_task, 10);
}
/*----------------------------------------------------------------------------
 * end of file
 *---------------------------------------------------------------------------*/

