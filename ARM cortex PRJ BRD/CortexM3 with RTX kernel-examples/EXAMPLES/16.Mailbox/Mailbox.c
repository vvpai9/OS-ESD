/*--------------------------------------------------------------------------------------------------------------------------------------
 *      RL-ARMCTX - RTX
 *--------------------------------------------------------------------------------------------------------------------------------------
 *      Name:    Mailbox.c
 *      Purpose: RTX example to show mailbox usage
 *------------------------------------------------------------------------------------------------------------------------------------*/

#include <RTL.h>                        // RTX kernel functions & defines      
#include <LPC17xx.h>                    // LPC17xx definitions                 
#include <stdio.h>

// tsk1, tsk2 will contain task identifications at run-time
OS_TID tsk1, tsk2;  

typedef struct {                        // Message object structure            
  	float voltage;                      // AD result of measured voltage       
  	float current;                      // AD result of measured current       
  	U32   counter;                      // A counter value                    
} T_MEAS;

char buf[40];
unsigned int i=0;

os_mbx_declare (MsgBox,1);             // Declare an RTX mailbox             
_declare_box (mpool,sizeof(T_MEAS),1); // Dynamic memory pool                

/* Function prototypes */
__task void init_task(void);
__task void send_task (void);
__task void rec_task (void);
void init_serial(void);

/*--------------------------------------------------------------------------------------------------------------------------------------
 *   init_task:  RTX Kernel starts this task with os_sys_init_prio(init_task, 10);
 *------------------------------------------------------------------------------------------------------------------------------------*/
__task void init_task(void) {
	tsk1 = os_tsk_create(send_task,2); 	// send_task at priority 2 
	tsk2 = os_tsk_create(rec_task,1); 	// rec_task at priority 1
	os_tsk_delete_self(); 			// must delete itself before exiting
}

/*--------------------------------------------------------------------------------------------------------------------------------------
 *  Task 1:  RTX Kernel starts this task with os_tsk_create (send_task, 2)
 *------------------------------------------------------------------------------------------------------------------------------------*/
__task void send_task (void) {
  	T_MEAS *mptr;
       
	os_mbx_init (MsgBox, sizeof(MsgBox));// initialize the mailbox    
	
   	mptr = _alloc_box (mpool);          // Allocate a memory for the message   
	mptr->voltage = 223.72;             // Set the message content             
	mptr->current = 17.54;
	mptr->counter = 120786;
	os_mbx_send (MsgBox, mptr, 0xffff); // Send the message to the mailbox     
  	LPC_GPIO0->FIOCLR = 0x00000010;
	os_dly_wait (100);
  	
 	mptr = _alloc_box (mpool);
 	mptr->voltage = 227.23;             // Prepare a 2nd message               
 	mptr->current = 12.41;
 	mptr->counter = 170823;
 	os_mbx_send (MsgBox, mptr, 0xffff); // And send it.                        
  	os_tsk_pass ();                     // Cooperative multitasking            
  	LPC_GPIO0->FIOCLR = 0x00000020;
 	os_dly_wait (100);

 	mptr = _alloc_box (mpool);
 	mptr->voltage = 229.44;             // Prepare a 3rd message               
 	mptr->current = 11.89;
 	mptr->counter = 237178;
 	os_mbx_send (MsgBox, mptr, 0xffff); // And send it.                       
  	LPC_GPIO0->FIOCLR = 0x00000040;
 	os_dly_wait (100);

	os_tsk_delete_self ();              // We are done here, delete this task  */
}

/*--------------------------------------------------------------------------------------------------------------------------------------
 *  Task 2: RTX Kernel starts this task with os_tsk_create (rec_task, 1)
 *------------------------------------------------------------------------------------------------------------------------------------*/
__task void rec_task (void) {
  	T_MEAS *rptr;

  	for (;;) {
    	os_mbx_wait (MsgBox, (void **)&rptr, 0xffff); // wait for the message    
    	sprintf (buf,"\nVoltage: %.2f V\r",rptr->voltage);
		while (buf[i] != '\0'){		  		// Displaying on hyper terminal
	  		while (!(LPC_UART0->LSR & 0x20));
			LPC_UART0->THR = buf[i];		  	// task2 is running
			i++;
		}
		i=0;
    	sprintf (buf,"Current: %.2f A\r",rptr->current);
		while (buf[i] != '\0'){		  		// Displaying on hyper terminal
	  		while (!(LPC_UART0->LSR & 0x20));
			LPC_UART0->THR = buf[i];		  	// task2 is running
			i++;
		}
		i=0;
    	sprintf (buf,"Number of cycles: %d\r",rptr->counter);
		while (buf[i] != '\0'){		  		// Displaying on hyper terminal
	  		while (!(LPC_UART0->LSR & 0x20));
			LPC_UART0->THR = buf[i];		  	// task2 is running
			i++;
		}
		i=0;
    	_free_box (mpool, rptr);        // free memory allocated for message  
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
	LPC_PINCON->PINSEL0 &= 0xFF0000FF;
	LPC_GPIO0->FIODIR |= 0x00000FF0;
	LPC_GPIO0->FIOSET = 0x00000FF0;
  	_init_box (mpool, sizeof(mpool),sizeof(T_MEAS));  	// initialize the 'mpool' memory for the membox dynamic allocation        
	UART0_Init();		  								// Initialize serial interface
	os_sys_init_prio(init_task, 10);					// Initialize init_task and start RTX Kernel
}
/*--------------------------------------------------------------------------------------------------------------------------------------
 * end of file
 *------------------------------------------------------------------------------------------------------------------------------------*/

