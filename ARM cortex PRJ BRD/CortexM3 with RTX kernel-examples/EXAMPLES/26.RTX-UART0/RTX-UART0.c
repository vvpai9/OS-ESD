/*--------------------------------------------------------------------------------------------------
 *      RL-ARMCTX - RTX
 *--------------------------------------------------------------------------------------------------
 *      Name:    RTX-UART0.c
 *      Purpose: Creating RTX application - creating tasks and setting an event for the task in isr  
 *-------------------------------------------------------------------------------------------------*/

#include <LPC17xx.h>
#include <RTL.h>
#include <stdio.h>

#define EVT_KEY 0x0001
OS_TID tsk1;			 

/* Function prototypes */
__task void init_task(void);
__task void task1(void);

unsigned char cnt1=0,rx_data=0,flag=0;
unsigned char rx_flag=0,tx_flag=0;

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
__task void task1( void ) {

	while(1) {
		os_evt_wait_or( EVT_KEY , 0xffff);

		cnt1++;

		// send recieved characters to hyper terminal 
		if( rx_flag == 0xFF){
			LPC_UART0->THR = rx_data;
			rx_flag = 0x00;
		}

		if(flag==0)
		{
			LPC_GPIO0->FIOSET = 0x002000000;	// relay on
			flag=1;
		}
		else
		{
			LPC_GPIO0->FIOCLR = 0x002000000;	// relay off
			flag=0;
		} 
	}			 
}

/*----------------------------------------------------------------------------
 *        Initialize serial interface	- UART0
 *---------------------------------------------------------------------------*/
void UART0_Init(void)
{
	LPC_SC->PCONP |= 0x00000008;				// UART0 peripheral enable
	LPC_PINCON->PINSEL0 &= ~0x000000F0;
	LPC_PINCON->PINSEL0 |= 0x00000050;			// P0.2 - TXD0 and P0.3 - RXD0
	LPC_UART0->LCR = 0x00000083;				// enable divisor latch, parity disable, 1 stop bit, 8bit word length
	LPC_UART0->DLM = 0X00; 
	LPC_UART0->DLL = 0xA2;      				// select baud rate 9600 bps
	LPC_UART0->LCR = 0X00000003;
	LPC_UART0->FCR = 0x07;
	LPC_UART0->IER = 0X03;	   					// select Transmit and receive interrupt

	NVIC_EnableIRQ(UART0_IRQn);					// Assigning channel
}

/*----------------------------------------------------------------------------
 *        uart0 isr 
 *---------------------------------------------------------------------------*/
void UART0_IRQHandler(void)
{
	unsigned long Int_Stat;
	Int_Stat = LPC_UART0->IIR;					// reading the data from interrupt identification register
	Int_Stat = Int_Stat & 0x06;					// masking other than txmit int & rcve data indicator

	if((Int_Stat & 0x02)== 0x02)				// transmit interrupt
	{
		tx_flag = 0xff;
	}
	else if( (Int_Stat & 0x04) == 0x04)  		// recve data availble
	{
		isr_evt_set(EVT_KEY , tsk1);
		rx_data = LPC_UART0->RBR;
		rx_flag = 0xff;
	}
}

/*----------------------------------------------------------------------------
 *        Main: Initialize and start RTX Kernel
 *---------------------------------------------------------------------------*/
int main ( void ) {
	LPC_PINCON->PINSEL1 &= 0xFFF3FFFF;			// P0.25 GPIO
	LPC_GPIO0->FIODIR |= 0x002000000;			// P0.25 output
	UART0_Init();
	/* Initialize RTX and start init   */
	os_sys_init_prio(init_task, 10);
}
/*----------------------------------------------------------------------------
 * end of file
 *---------------------------------------------------------------------------*/
