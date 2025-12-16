/*-----------------------------------------------------------------------------------
 *      RL-ARM - RTX
 *-----------------------------------------------------------------------------------
 *      Name:    RTX_MAILBOX_INT_ADC.c  
 *      Purpose: Internal ADC(10 bit) - AD0.4 - P0.25 using Mailbox concept program
 *---------------------------------------------------------------------------------*/

#include <RTL.h>                      	// RTX kernel functions & defines      
#include <LPC214x.h>                  	// LPC21xx definitions                
#include <stdio.h>

// tsk1, tsk2 will contain task identifications at run-time
OS_TID tsk1, tsk2;  

os_mbx_declare(MsgBox,16);              // Declare a message pointers as an array of unsigned int 
										// macro defined int  rtl.h -> U32 name [4 + cnt] 

typedef struct {                      	// Format of each message slot            
	float v_in;                       	// AD result of measured adc input voltage 
  	unsigned int v_out;                 // AD result of measured adc output voltage 
} T_MEAS;

_declare_box(mpool,sizeof(T_MEAS),16);  	// Reserve a block of memory larg enough to hold 16 message slot 
										// macro defined in rtl.h -> U32 pool[(((size)+3)/4)*(cnt) + 3]

#define vol 3.3            			  	// Reference voltage
#define fullscale 0x3ff         	  	// 10 bit adc

char buf[20];
unsigned int i=0, cnt1=0, cnt2=0;
unsigned int adc_value=0,temp_adc=0;
float temp;
      
/* Function prototypes */
__task void init_task(void);
__task void send_task(void);
__task void rec_task(void);
void init_serial(void);

/*----------------------------------------------------------------------------
 *        Main: Initialize and start RTX Kernel
 *---------------------------------------------------------------------------*/
int main(void) {                     	// program execution starts here       
	PINSEL1 = 0X00040000;   			// AD0.4 pin is selected(P0.25)
	init_serial();                     	// initialize the serial interface     
  	_init_box(mpool, sizeof(mpool),    	// The block of memory formatted into required 16 mail slots   
              sizeof(T_MEAS));        	       
	os_sys_init_prio(init_task, 10);    // Initialize and start RTX kernel with priority of 10
										// so that no task switching occures    
}

/*----------------------------------------------------------------------------
 *        Initialize serial interface - UART0
 *---------------------------------------------------------------------------*/
void init_serial(void) {
	PINSEL0 = 0X0000005;        // Enable TXD0-P0.0 & RXD0-P0.1               
  	U0LCR = 0x83;               // 8 bits, no Parity, 1 Stop bit     
  	U0DLL = 0x61;               // 9600 Baud Rate @ 15MHz VPB Clock  
  	U0LCR = 0x03;               // DLAB = 0                          
}

/*-----------------------------------------------------------------------------------
 *   init_task:  RTX Kernel starts this task with os_sys_init_prio(init_task, 10);
 *---------------------------------------------------------------------------------*/
__task void init_task(void) {
	tsk1 = os_tsk_create(send_task,2); 	// send_task at priority 2 	// it will run first
	tsk2 = os_tsk_create(rec_task,1); 	// rec_task at priority 1
	os_tsk_delete_self(); 				// must delete itself before exiting
}

/*----------------------------------------------------------------------------
 *  Task 1:  RTX Kernel starts this task with os_tsk_create (send_task, 2)
 *---------------------------------------------------------------------------*/
__task void send_task(void) {
	T_MEAS *mptr;						 // create pointer of message structure type

   	os_mbx_init(MsgBox, sizeof(MsgBox)); // initialize the mailbox so that it can be used for communication between tasks             
  
 	while(1){
 		cnt1++;							 // debug purpose
  		mptr = _alloc_box(mpool);        // acquire mailbox	 // allocate mail slot
		
   		//Conversion starts
		AD0CR = 0x01200010;       		  //command register for ADC-AD0.4
		while(((temp_adc = AD0GDR) &0x80000000) == 0x00000000);	//to check the interrupt bit	
		adc_value = AD0GDR;          	  //reading the ADC value from AD0GDR 6th bit to 15th bit contains DATA
		adc_value >>= 6;
		adc_value &= 0x000003ff; 		  // 10 bit internal ADC
		temp = ((float)adc_value * (float)vol)/(float)fullscale;
		
		//storing value in mailbox
		mptr->v_in = temp;
		mptr->v_out = adc_value;

		// send message into mailbox this locks mail slot protecting the data and message pointer is transferred to waiting tasks
		os_mbx_send (MsgBox, mptr, 0xffff); 
		
		os_dly_wait (50);	// 0.5sec delay	 enough time to read message by recive task
	}
}

/*----------------------------------------------------------------------------
 *  Task 2: RTX Kernel starts this task with os_tsk_create (rec_task, 1)
 *---------------------------------------------------------------------------*/
__task void rec_task(void) {
	T_MEAS *rptr;			  // create pointer of message structure type

	while(1) {
  		cnt2++;				  // debug purpose
    	
		// this call allows us to nominate the mailbox that we want to use
		// provides pointer to the mail slot buffer and specify a time out
		os_mbx_wait(MsgBox, (void **)&rptr, 0xffff); // if message not avilable wait for the message    
    	
		// manipulate the data 
		sprintf(buf,"\nReading Number : %d\r", cnt2);
		while (buf[i] != '\0') {
		  	while (!(U0LSR & 0x20));	// Check for Transmitter Holding Register Empty
		  	U0THR = buf[i];				// Send 8 bit data
			i++;
		}
		i=0;

		sprintf(buf,"adc_in: %.2f V\r",rptr->v_in);
		while (buf[i] != '\0') {
		  	while (!(U0LSR & 0x20));	// Check for Transmitter Holding Register Empty
		  	U0THR = buf[i];				// Send 8 bit data
			i++;
		}
		i=0;
    	sprintf(buf,"adc_out: %X V\r",rptr->v_out);
		while (buf[i] != '\0') {
		  	while (!(U0LSR & 0x20));
		  	U0THR = buf[i];
			i++;
		}
		i=0;

	   	_free_box(mpool, rptr);         // free mail slot allocated for message
										// so that it can be used to transfer further messages  
	}
}

/*----------------------------------------------------------------------------
 * end of file
 *---------------------------------------------------------------------------*/
