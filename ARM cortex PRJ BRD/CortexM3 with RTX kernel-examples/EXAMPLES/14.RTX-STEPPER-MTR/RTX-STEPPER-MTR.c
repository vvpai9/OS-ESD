/*--------------------------------------------------------------------------------------------------------------------------------------
 *      RL-ARMCTX - RTX
 *--------------------------------------------------------------------------------------------------------------------------------------
 *      Name:    RTX-STEPPER-MTR.c
 *      Purpose: RTX example program to rotate stepper motor using pre-emptive task
 *------------------------------------------------------------------------------------------------------------------------------------*/

#include <rtl.h>
#include <lpc17xx.h>

// tsk1, tsk2, tsk3 will contain task identifications at run-time
OS_TID tsk1,tsk2,tsk3;

unsigned char msg1[]= "CLOCKWISE     (8sec)\r";
unsigned char msg2[]= "ANTICLOCKWISE (8sec)\r";
unsigned long int var1=0,var2=0;
unsigned int i=0,j=0,k=0,cnt1=0,cnt2=0;
unsigned char flag=0;

/* Function prototypes */
__task void init_task(void);
__task void task1(void);
__task void task2(void);
__task void task3(void);
void init_serial(void);

/*--------------------------------------------------------------------------------------------------------------------------------------
 *   init_task:  RTX Kernel starts this task with os_sys_init_prio(init_task, 10);
 *------------------------------------------------------------------------------------------------------------------------------------*/
__task void init_task(void) {
	tsk1 = os_tsk_create(task1,2); 	// task1 at priority 2 
	tsk2 = os_tsk_create(task2,1); 	// task2 at priority 1
	tsk3 = os_tsk_create(task3,1);	// task2 at priority 1
	os_tsk_delete_self(); 			// must delete itself before exiting
}

/*--------------------------------------------------------------------------------------------------------------------------------------
 *   Task 1:  RTX Kernel starts this task with os_tsk_create (task1,2)
 *------------------------------------------------------------------------------------------------------------------------------------*/
__task void task1 (void) {
	while (1) {
		os_evt_wait_or(0x0001, 0xffff);
		while (msg1[k] != '\0'){
	  		while (!(LPC_UART0->LSR & 0x20));
	  		LPC_UART0->THR = msg1[k];
			k++;
		}
		k=0;  
		
		var1 = 0x80000000;	   				// For clockwise
    	for(j=0; j < 200; j++){
			var2 = 0x00000000;      	
   			for(i=28;i<=31;i++)         	// for A B C D Stepping
			{
				var2 = var1 >> i;
   	   			LPC_GPIO2->FIOPIN = ~var2;
				os_dly_wait(1); 
			}
		} 	
	}
}

/*--------------------------------------------------------------------------------------------------------------------------------------
 *   Task 2:  RTX Kernel starts this task with os_tsk_create (task2,1)
 *------------------------------------------------------------------------------------------------------------------------------------*/
__task void task2 (void) {
	while (1) {
	   	os_evt_wait_or(0x0002, 0xffff);
		while (msg2[k] != '\0'){
	  		while (!(LPC_UART0->LSR & 0x20));
	  		LPC_UART0->THR = msg2[k];
			k++;
		}
		k=0;  
	
		var1 = 0x00000001; 					// For Anticlockwise
		for(j=0; j < 200; j++){
			var2 = 0x00000000;         
   			for(i=0;i<=3;i++)         		// for A B C D Stepping
			{
				var2 = var1 << i;
   	   			LPC_GPIO2->FIOPIN = ~var2;
				os_dly_wait(1); 
			}
		}	
	}
}

/*--------------------------------------------------------------------------------------------------------------------------------------
 *   Task 3:  RTX Kernel starts this task with os_tsk_create (task3,1)
 *------------------------------------------------------------------------------------------------------------------------------------*/
__task void task3 (void) {
	while (1) {
		if(flag){
			os_dly_wait(2);
			cnt1++;
			if(cnt1==2){
				cnt1 = 0;
				cnt2++;
				if(cnt2==250){
					cnt2=0;
					flag=0;
					os_evt_set(0x0001,tsk1); 		// set an event for task1
				}
			}
		}

		if(!flag){
			os_dly_wait(2);
			cnt1++;
			if(cnt1==2){
				cnt1 = 0;
				cnt2++;
				if(cnt2==250){
					cnt2=0;
					flag=1;
					os_evt_set(0x0002,tsk2);		// set an event for task2
				}
			}
		}
	}
}

/*--------------------------------------------------------------------------------------------------------------------------------------
 *        Initialize serial interface										 
 *------------------------------------------------------------------------------------------------------------------------------------*/
void UART0_Init(void) {
	LPC_SC->PCONP |= 0x00000008;				// UART0 peripheral enable
	LPC_PINCON->PINSEL0 &= ~0x000000F0;
	LPC_PINCON->PINSEL0 |= 0x00000050;			// P0.2 - TXD0 and P0.3 - RXD0
	LPC_UART0->LCR = 0x00000083;				// enable divisor latch, parity disable, 1 stop bit, 8bit word length
	LPC_UART0->DLM = 0X00; 
	LPC_UART0->DLL = 0xA2;      				// select baud rate 9600 bps @25MHz
	LPC_UART0->LCR = 0X00000003;
	LPC_UART0->FCR = 0x07;
	LPC_UART0->IER = 0X03;	   					// select Transmit and receive interrupt
}

/*--------------------------------------------------------------------------------------------------------------------------------------
 *        Main: Initialize and start RTX Kernel
 *------------------------------------------------------------------------------------------------------------------------------------*/
int main(void) {
	LPC_PINCON->PINSEL4 = 0xFFFFFF00;			// P2.0 to P2.3 GPIO
	LPC_GPIO2->FIODIR = 0x0000000F;				// P2.0 to P2.3 output
	UART0_Init();		  						// Initialize serial interface
	os_sys_init_prio(init_task, 10);			// Initialize init_task and start RTX Kernel
}
/*--------------------------------------------------------------------------------------------------------------------------------------
 * end of file
 *------------------------------------------------------------------------------------------------------------------------------------*/
