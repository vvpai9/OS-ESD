/*------------------------------------------------------------------------------
 *      RL-ARM - RTX
 *------------------------------------------------------------------------------
 *      Name:    RTX-TIMER1.c 
 *      Purpose: RTX example program to show TIMER1 Interrupt usage
 *----------------------------------------------------------------------------*/

#include <LPC17xx.H>
#include <RTL.H>

#define EVT_KEY 0x0001		 // event key for task1
OS_TID tsk1;				 // task identification for task1

/* Function prototypes */
__task void init_task(void);
__task void task1(void);
void TIMER1_IRQHandler(void); 
void timer1_init(void);	

unsigned char flag=0;

/*-----------------------------------------------------------------------------------
 *   init_task:  RTX Kernel starts this task with os_sys_init_prio(init_task, 10);
 *---------------------------------------------------------------------------------*/
__task void init_task(void) {
	tsk1 = os_tsk_create(task1,1); 	// task1 at priority 1 
	os_tsk_delete_self(); 			// must delete itself before exiting
}

/*----------------------------------------------------------------------------
 *   Task 1:  RTX Kernel starts this task with os_tsk_create (task1,1)
 *---------------------------------------------------------------------------*/
__task void task1(void) {
	while(1){
		os_evt_wait_or( EVT_KEY , 0xffff);	  // waiting for an event to set
 		//if event is set in timer isr do this
		if(flag == 0){
			LPC_GPIO0->FIOSET = 0x002000000;		// RLY ON for 2sec 
		    flag = 1;
		}
		else {
			LPC_GPIO0->FIOCLR = 0x002000000;		// RLY OFF for 2sec
			flag = 0;
		}
	}
}

/*----------------------------------------------------------------------------
 * Initialise timer0 
 *---------------------------------------------------------------------------*/
void timer1_init(void)
{
  	LPC_SC->PCONP |= (2<<1);    					// powers the T1
	LPC_TIM1->MR0 = 25000*2000;						// 25000 counts for 1msec          	
  	LPC_TIM1->MCR = 0x00000003;        				// Interrupt and Reset on MR0
  	LPC_TIM1->TCR = 0x00000001;                 	// Timer1 Enable
	NVIC_EnableIRQ(TIMER1_IRQn);
}

/*----------------------------------------------------------------------------
 *  Timer1 Handler 
 *---------------------------------------------------------------------------*/
void TIMER1_IRQHandler(void)
{
	LPC_TIM1->IR |=0x01;
	isr_evt_set( EVT_KEY , tsk1); 					// event is set for task1
}

/*----------------------------------------------------------------------------
 *  Main: Initialize and start RTX Kernel
 *---------------------------------------------------------------------------*/
int main (void) {
	LPC_PINCON->PINSEL1 &= 0xFFF3FFFF;			// P0.25 GPIO
	LPC_GPIO0->FIODIR |= 0x002000000;			// P0.25 output
	timer1_init();  							// initialise timer1  
	/* Initialize RTX and start init   */
  	os_sys_init_prio(init_task, 10);
}
/*----------------------------------------------------------------------------
 * end of file
 *---------------------------------------------------------------------------*/







