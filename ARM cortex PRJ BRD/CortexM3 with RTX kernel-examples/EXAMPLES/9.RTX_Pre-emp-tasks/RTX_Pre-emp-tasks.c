/*--------------------------------------------------------------------------------------------------------------------------------------
 *      RL-ARMCTX - RTX
 *--------------------------------------------------------------------------------------------------------------------------------------
 *      Name:    RTX_Pre-emp-tasks.c
 *      Purpose: RTX example to show pre-emptive scheduling
 *------------------------------------------------------------------------------------------------------------------------------------*/
#include <rtl.h>
#include <lpc17xx.h>

// tsk1, tsk2 will contain task identifications at run-time 
OS_TID tsk1, tsk2;

// Function prototypes
__task void init_task(void); 
__task void task1 (void);
__task void task2 (void);
void UART0_Init(void);
unsigned char hex_ascii(unsigned int x);

U16 counter1=0,counter2=0;
unsigned char arr[6], msg[] = "counter1 : ";
unsigned char arr1[6], msg1[] = "counter2 : ";
U16 i=0,temp=0,temp1=0;

/*------ hex_to_ascii fn -------*/
unsigned char hex_ascii(unsigned int x);
unsigned char hex_ascii(unsigned int x)
{
	unsigned char ch;
	ch = (unsigned char)x;
	
	if(ch > 9)
		ch+=0x37;
	else
		ch+=0x30;
	return ch;
}

/*--------------------------------------------------------------------------------------------------------------------------------------
 *   init_task:  RTX Kernel starts this task with os_sys_init_prio(init_task, 10);
 *------------------------------------------------------------------------------------------------------------------------------------*/
__task void init_task(void) {
	tsk1 = os_tsk_create(task1,2); 	// task1 at priority 2 
	tsk2 = os_tsk_create(task2,1); 	// task2 at priority 1
	os_tsk_delete_self(); 			// must delete itself before exiting
}

/*--------------------------------------------------------------------------------------------------------------------------------------
 *   Task 1:  RTX Kernel starts this task with priority 2
 *------------------------------------------------------------------------------------------------------------------------------------*/
__task void task1 (void) {
   	while (1) {
    	os_evt_wait_or(0x0001, 0xFFFF);
    	counter1++;
		temp = counter1;		  
		temp &= 0xf000;
		temp >>= 12;
		arr[0]=hex_ascii(temp);
	
		temp = counter1;
		temp &= 0x0f00;
		temp >>= 8;
		arr[1]=hex_ascii(temp);
	
		temp = counter1;
		temp &= 0x00f0;
		temp >>= 4;
		arr[2]=hex_ascii(temp);
	
		temp = counter1;
		temp &= 0x000f;
		arr[3]=hex_ascii(temp);
	
		arr[4] = '\0';

		while (msg[i] != '\0'){
	  		while (!(LPC_UART0->LSR & 0x20));
	  		LPC_UART0->THR = msg[i];
			i++;
		}
		i=0;  
		
		while (arr[i] != '\0'){
	  		while (!(LPC_UART0->LSR & 0x20));
	  		LPC_UART0->THR = arr[i];
			i++;
		}
		i=0;  

		while (!(LPC_UART0->LSR & 0x20));
	  		LPC_UART0->THR = '\r';		
  	}
}

/*--------------------------------------------------------------------------------------------------------------------------------------
 *   Task 2:  RTX Kernel starts this task with priority 1
 *------------------------------------------------------------------------------------------------------------------------------------*/
__task void task2 (void) {
  	while (1) {
       	counter2++;
		temp1 = counter2;
		temp1 &= 0xf000;
		temp1 >>= 12;
		arr1[0]=hex_ascii(temp1);
	
		temp1 = counter2;
		temp1 &= 0x0f00;
		temp1 >>= 8;
		arr1[1]=hex_ascii(temp1);
	
		temp1 = counter2;
		temp1 &= 0x00f0;
		temp1 >>= 4;
		arr1[2]=hex_ascii(temp1);
		
		temp1 = counter2;
		temp1 &= 0x000f;
		arr1[3]=hex_ascii(temp1);
	
		arr1[4] = '\0';

		while (msg1[i] != '\0'){
	  		while (!(LPC_UART0->LSR & 0x20));
	  		LPC_UART0->THR = msg1[i];
			i++;
		}
		i=0;  
		
		while (arr1[i] != '\0'){
	  		while (!(LPC_UART0->LSR & 0x20));
	  		LPC_UART0->THR = arr1[i];
			i++;
		}
		i=0;  

		while (!(LPC_UART0->LSR & 0x20));
	  		LPC_UART0->THR = '\r';

		// set an event for task1
		os_evt_set (0x0001, tsk1);	 // task switching takes place
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
	UART0_Init();		  						// Initialize serial interface
	os_sys_init_prio(init_task, 10);			// Initialize init_task and start RTX Kernel
}
/*--------------------------------------------------------------------------------------------------------------------------------------
 * end of file
 *------------------------------------------------------------------------------------------------------------------------------------*/
