/*----------------------------------------------------------------------------
 *      RL-ARMCTX - RTX
 *----------------------------------------------------------------------------
 *      Name:    RTX_PWM.c
 *      Purpose: RTX example to show PWM operation
 *----------------------------------------------------------------------------*/

#include <rtl.h>
#include <lpc17xx.h>

// tsk1 will contain task identifications at run-time
OS_TID tsk1;

unsigned int i, cnt1=0;
unsigned char flag=0,flag1;

/* Function prototypes */
__task void init_task(void);
__task void task1(void);
void pwm_init(void);
void PWM1_IRQHandler(void);

/*-----------------------------------------------------------------------------------
 *   init_task:  RTX Kernel starts this task with os_sys_init_prio(init_task, 10);
 *---------------------------------------------------------------------------------*/
__task void init_task(void) {
	tsk1 = os_tsk_create(task1,1); 			// task1 at priority 1 
	os_tsk_delete_self(); 					// must delete itself before exiting
}

/*----------------------------------------------------------------------------
 *   Task 1:  RTX Kernel starts this task with os_tsk_create (task1,2)
 *---------------------------------------------------------------------------*/
__task void task1 (void) {
	os_itv_set (50);     					// set wait interval:  0.5 second 
  	while (1) {
    	os_evt_wait_or(0x0001, 0xFFFF);
	//	cnt1++;		 // Only for software debug
	 	os_itv_wait ();
	    /* do some actions at regular time intervals */
	    if(flag == 0x00) {		   
			LPC_PWM1->MR2 = LPC_PWM1->MR2+25000;	// increment ON time 1msec 
		
	    	if(LPC_PWM1->MR2 >= 250000)	    // turn on LED when 10msec ON time reaches 
	    	{
	    		LPC_PWM1->MR2 = 250000;
				LPC_GPIO2->FIOSET = 0x00001000;
		   	    flag = 0xff;
			}
		}
		
		if(flag == 0xFF) {		   
			LPC_PWM1->MR2 = LPC_PWM1->MR2-25000;	// decrement ON time by 1msec
				
	    	if(LPC_PWM1->MR2 <= 25000)		// turn OFF LED when 1msec ON time reaches 
	    	{
	    		LPC_PWM1->MR2 = 25000;
				LPC_GPIO2->FIOCLR = 0x00001000;	
		   	    flag = 0x00;
			}
		}
	}
}

/*----------------------------------------------------------------------------
 *       Initialize PWM1.2
 *---------------------------------------------------------------------------*/
void pwm_init(void) {
    LPC_SC->PCONP |= (1<<6);				// PWM1 is powered
	LPC_PINCON->PINSEL7 |= 0x000C0000;		// pwm1.2 is selected for the pin P3.25
	  
	LPC_PWM1->PR  = 0x00000000;      		// Count frequency : Fpclk 
	LPC_PWM1->PCR = 0x00000400;      		// select PWM1.2 single edge 
	LPC_PWM1->MCR = 0x00000003;      		// Reset and interrupt on PWMMR0
	LPC_PWM1->MR0 = 250000;           		// setup match register 0 count 
	LPC_PWM1->MR2 = 25000;      			// setup match register MR2 
	LPC_PWM1->LER = 0x000000FF;      		// enable shadow copy register
	LPC_PWM1->TCR = 0x00000002;      		// RESET COUNTER AND PRESCALER
	LPC_PWM1->TCR = 0x00000009;      		// enable PWM and counter
	
	NVIC_EnableIRQ(PWM1_IRQn);
}   

/*----------------------------------------------------------------------------
 *       PWM Handler
 *---------------------------------------------------------------------------*/	                     
void PWM1_IRQHandler(void) {
	isr_evt_set(0x0001, tsk1);
	LPC_PWM1->IR = 0xff; 					// clear the interrupts
	LPC_PWM1->LER = 0x000000FF;
}

/*----------------------------------------------------------------------------
 *       Main: Initialize and start RTX Kernel
 *---------------------------------------------------------------------------*/ 
int main (void) {
	pwm_init();
	LPC_PINCON->PINSEL4 &= 0xFCFFFFFF;		// P2.12 GPIO for LED
	LPC_GPIO2->FIODIR = 0x00001000;			// P2.12 is assigned output
	os_sys_init_prio(init_task, 10);
}
/*----------------------------------------------------------------------------
 * end of file
 *---------------------------------------------------------------------------*/
