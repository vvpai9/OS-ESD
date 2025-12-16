/*----------------------------------------------------------------------------
 *      RL-ARM - RTX
 *----------------------------------------------------------------------------
 *      Name:    Mailbox.c
 *      Purpose: RTX example program to show mailbox usage
 *----------------------------------------------------------------------------*/

#include <RTL.h>                        // RTX kernel functions & defines      
#include <LPC21xx.h>                    // LPC21xx definitions                 
#include <stdio.h>

// tsk1, tsk2 will contain task identifications at run-time
OS_TID tsk1, tsk2;  

os_mbx_declare(MsgBox,3);             	// Declare a message pointers as an array of unsigned int 
										// macro defined int  rtl.h -> U32 name [4 + cnt] 
// Format of each message slot
typedef struct {                        // Message object structure            
  	float voltage;                      // AD result of measured voltage       
  	float current;                      // AD result of measured current       
  	U32   counter;                      // A counter value                    
} T_MEAS;

_declare_box (mpool,sizeof(T_MEAS),3); 	// Reserve a block of memory larg enough to hold 16 message slot 
										// macro defined in rtl.h -> U32 pool[(((size)+3)/4)*(cnt) + 3]

char buf[40];
unsigned int i=0;

/* Function prototypes */
__task void init_task(void);
__task void send_task(void);
__task void rec_task(void);
void init_serial(void);

/*-----------------------------------------------------------------------------------
 *   init_task:  RTX Kernel starts this task with os_sys_init_prio(init_task, 10);
 *---------------------------------------------------------------------------------*/
__task void init_task(void) {
	tsk1 = os_tsk_create(send_task,2); 	// send_task at priority 2 
	tsk2 = os_tsk_create(rec_task,1); 	// rec_task at priority 1
	os_tsk_delete_self(); 				// must delete itself before exiting
}

/*----------------------------------------------------------------------------
 *  Task 1:  RTX Kernel starts this task with os_tsk_create (send_task, 1)
 *---------------------------------------------------------------------------*/
__task void send_task(void) {
  	T_MEAS *mptr;
                  
  	os_mbx_init(MsgBox, sizeof(MsgBox));// initialize the mailbox             
  
	mptr = _alloc_box(mpool);          	// Allocate a memory for the message   
	mptr->voltage = 223.72;             // Set the message content             
	mptr->current = 17.54;
	mptr->counter = 120786;
	os_mbx_send(MsgBox, mptr, 0xffff); // Send the message to the mailbox     
	IOCLR0 |= 0x00010000;				// L1 is turned on after sending 1st msg
	os_dly_wait(100);
  	
  	mptr = _alloc_box(mpool);
  	mptr->voltage = 227.23;             // Prepare a 2nd message               
  	mptr->current = 12.41;
  	mptr->counter = 170823;
  	os_mbx_send(MsgBox, mptr, 0xffff); // And send it.                        
   	IOCLR0 |= 0x00020000;				// L2 is turned on after sending 1st msg
  	os_dly_wait(100);

  	mptr = _alloc_box(mpool);
  	mptr->voltage = 229.44;             // Prepare a 3rd message               
  	mptr->current = 11.89;
  	mptr->counter = 237178;
  	os_mbx_send(MsgBox, mptr, 0xffff); 	// And send it.                       
   	IOCLR0 |= 0x00040000;				// L3 is turned on after sending 1st msg
 	os_dly_wait(100);					  

	os_tsk_delete_self();              // We are done here, delete this task  
}

/*----------------------------------------------------------------------------
 *  Task 2: RTX Kernel starts this task with os_tsk_create (rec_task, 1)
 *---------------------------------------------------------------------------*/
__task void rec_task(void) {
  	T_MEAS *rptr;

  	while(1) {
    	os_mbx_wait (MsgBox, (void **)&rptr, 0xffff); // wait for the message    
    	sprintf (buf,"\nVoltage: %.2f V\r",rptr->voltage);
		while(buf[i] != '\0') {
		  	while (!(U0LSR & 0x20));
		  	U0THR = buf[i];
			i++;
		}
		i=0;
    	sprintf (buf,"Current: %.2f A\r",rptr->current);
		while(buf[i] != '\0') {
		  	while (!(U0LSR & 0x20));
		  	U0THR = buf[i];
			i++;
		}
		i=0;
    	sprintf (buf,"Number of cycles: %d\r",rptr->counter);
		while(buf[i] != '\0') {
		  	while (!(U0LSR & 0x20));
		  	U0THR = buf[i];
			i++;
		}
		i=0;  
    	_free_box(mpool, rptr);        // free memory allocated for message  
	}
}

/*----------------------------------------------------------------------------
 *        Initialize serial interface - UART0
 *---------------------------------------------------------------------------*/
void init_serial(void) {
	PINSEL0 = 0X0000005;        		// Enable TXD0-P0.0 & RXD0-P0.1               
  	U0LCR = 0x83;              			// 8 bits, no Parity, 1 Stop bit     
  	U0DLL = 0x61;               		// 9600 Baud Rate @ 15MHz VPB Clock  
  	U0LCR = 0x03;               		// DLAB = 0                          
}

/*----------------------------------------------------------------------------
 *        Main: Initialize and start RTX Kernel
 *---------------------------------------------------------------------------*/
int main(void) {                       // program execution starts here       
	PINSEL1 &= 0xFFFFFF00;
	IODIR0 |= 0x000F0000;               // P0.16 to P0.23 defined as Outputs        
  	IOSET0 = 0x000F0000;				// Initially turn OFF all LED
  	init_serial();                     // initialize the serial interface     
  	_init_box(mpool, sizeof(mpool),    // The block of memory formatted into required 3 mail slots   
              sizeof(T_MEAS));        	      
	os_sys_init_prio(init_task, 10);      
}

/*----------------------------------------------------------------------------
 * end of file
 *---------------------------------------------------------------------------*/
