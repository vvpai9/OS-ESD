/*--------------------------------------------------------------------------------------------------------------------------------------
 *      RL-ARMCTX - RTX
 *--------------------------------------------------------------------------------------------------------------------------------------
 *      Name:    Mailbox-INT-ADC.c  
 *      Purpose: Internal ADC using Mailbox concept 
 *------------------------------------------------------------------------------------------------------------------------------------*/

#include <RTL.h>                      	// RTX kernel functions & defines      
#include <LPC17xx.h>                  	// LPC21xx definitions                
#include <stdio.h>

OS_TID tsk1;                          	// assigned identification for task 1  
OS_TID tsk2;                          	// assigned identification for task 2  

typedef struct {                      	// Message object structure            
	float v_in;                       	// AD result of measured adc input voltage 
  	int v_out;                        	// AD result of measured adc output voltage 
} T_MEAS;

#define	Ref_Vtg		3.300
#define	Full_Scale	0xFFF				//12 bit ADC

char buf[40];
unsigned int i=0,k=0,cnt1=0, cnt2=0;
unsigned int adc_temp=0,temp_adc=0;
float in_vtg;

os_mbx_declare (MsgBox,1);              // Declare an RTX mailbox              
_declare_box (mpool,sizeof(T_MEAS),1);  // Dynamic memory pool                

/* Function prototypes */
__task void init_task(void);
__task void send_task(void);
__task void rec_task(void);
void UART0_Init(void);

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

   	os_mbx_init (MsgBox, sizeof(MsgBox)); // initialize the mailbox             
   
 	while(1){
		cnt1++;
  		mptr = _alloc_box (mpool);        // Allocate a memory for the message   
	
		LPC_ADC->ADCR = (1<<5)|(1<<21)|(1<<24);		//0x01200001;//ADC0.5, start conversion and operational	
		for(k=0;k<2000;k++);						//delay for conversion
		while((adc_temp = LPC_ADC->ADGDR) == 0x80000000);	//wait till 'done' bit is 1, indicates conversion complete
		adc_temp = LPC_ADC->ADGDR;
		adc_temp >>= 4;
		adc_temp &= 0x00000FFF;						//12 bit ADC
		in_vtg = (((float)adc_temp * (float)Ref_Vtg))/((float)Full_Scale);	//calculating input analog voltage
		
		//storing value in mailbox
		mptr->v_in = in_vtg;
		mptr->v_out = adc_temp;
  		os_mbx_send (MsgBox, mptr, 0xffff); // Send the message to the mailbox     
  		os_dly_wait (100);
	}
}

/*--------------------------------------------------------------------------------------------------------------------------------------
 *  Task 2: RTX Kernel starts this task with os_tsk_create (rec_task, 1)
 *------------------------------------------------------------------------------------------------------------------------------------*/
__task void rec_task (void) {
	T_MEAS *rptr;

	while(1) {
  		cnt2++;
    	os_mbx_wait (MsgBox, (void **)&rptr, 0xffff); // wait for the message    
    	sprintf (buf,"\nadc_in: %.2f V\r",rptr->v_in);
		while (buf[i] != '\0'){
	  		while (!(LPC_UART0->LSR & 0x20));
	  		LPC_UART0->THR = buf[i];
			i++;
		}
		i=0;  
    	sprintf (buf,"adc_out: %X V\r",rptr->v_out);
		while (buf[i] != '\0'){
	  		while (!(LPC_UART0->LSR & 0x20));
	  		LPC_UART0->THR = buf[i];
			i++;
		}
		i=0;  
    	_free_box (mpool, rptr);           // free memory allocated for message  
	}
}

/*--------------------------------------------------------------------------------------------------------------------------------------
 *        Initialize serial interface										 
 *------------------------------------------------------------------------------------------------------------------------------------*/
void UART0_Init(void) {
	LPC_SC->PCONP |= 0x00000008;						// UART0 peripheral enable
	LPC_PINCON->PINSEL0 &= ~0x000000F0;
	LPC_PINCON->PINSEL0 |= 0x00000050;					// P0.2 - TXD0 and P0.3 - RXD0
	LPC_UART0->LCR = 0x00000083;						// enable divisor latch, parity disable, 1 stop bit, 8bit word length
	LPC_UART0->DLM = 0X00; 
	LPC_UART0->DLL = 0xA2;      						// select baud rate 9600 bps @25MHz
	LPC_UART0->LCR = 0X00000003;
	LPC_UART0->FCR = 0x07;
	LPC_UART0->IER = 0X03;	   							// select Transmit and receive interrupt
}

/*--------------------------------------------------------------------------------------------------------------------------------------
 *        Main: Initialize and start RTX Kernel
 *------------------------------------------------------------------------------------------------------------------------------------*/
int main(void) {
	LPC_SC->PCONP |= (1<<15); 							// Power for GPIO block
    LPC_PINCON->PINSEL3 |= 0xC0000000;					// P1.31 as AD0.5
	LPC_SC->PCONP |= (1<<12);							// enable the peripheral ADC  
  	_init_box (mpool, sizeof(mpool),sizeof(T_MEAS));  	// initialize the 'mpool' memory for the membox dynamic allocation        
	UART0_Init();		  								// Initialize serial interface
	os_sys_init_prio(init_task, 10);					// Initialize init_task and start RTX Kernel
}
/*--------------------------------------------------------------------------------------------------------------------------------------
 * end of file
 *------------------------------------------------------------------------------------------------------------------------------------*/
