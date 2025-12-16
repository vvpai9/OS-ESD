/*------------------------------------------------------------------------------------------------------------
 *      RL-ARM - RTX
 *------------------------------------------------------------------------------------------------------------
 *      Name:    Mailbox-INT-ADC.c  
 *      Purpose: SPI Temperature Sensor using Mailbox concept 
 *----------------------------------------------------------------------------------------------------------*/

#include <RTL.h>                      		// RTX kernel functions & defines      
#include <LPC17xx.h>                  		// LPC17xx definitions                 
#include <stdio.h>

OS_TID tsk1;                          		// assigned identification for task 1  
OS_TID tsk2;                          		// assigned identification for task 2  

typedef struct {                      		// Message object structure            
	float tempc;                      		// AD result of measured adc input voltage 
  	int adc_out;                      		// AD result of measured adc output voltage 
} T_MEAS;

os_mbx_declare (MsgBox,1);           		// Declare an RTX mailbox             
_declare_box (mpool,sizeof(T_MEAS),1);		// Dynamic memory pool  

#define VREF		5110
#define FULLSCALE	4095 
#define SSEL		0x00010000
#define EVT_KEY 	0x0001

unsigned char temp1=0x00;
unsigned int up4bits,low8bits;
float analog_val,temp_val;
unsigned int i=0,j=0,k=0;
unsigned int avg,temp;
unsigned int cnt1=0, cnt2=0;
char buf[20];              

// Function prototypes 
__task void init_task(void);
__task void send_task (void);
__task void rec_task (void);
void SPI_Init(void);
unsigned char hex_ascii(unsigned int x);
void UART0_Init(void);
void SPI_IRQHandler(void);

/*------------------------------------------------------------------------------------------------------------
 *   init_task:  RTX Kernel starts this task with os_sys_init_prio(init_task, 10);
 *----------------------------------------------------------------------------------------------------------*/
__task void init_task(void) {
	tsk1 = os_tsk_create(send_task,2); 		// send_task at priority 2 
	tsk2 = os_tsk_create(rec_task,1); 		// rec_task at priority 1
	os_tsk_delete_self(); 					// must delete itself before exiting
};

/*--------------------------------------------------------------------------------------------------------------
 *  Task 1: RTX Kernel starts this task with os_tsk_create (send_task, 2)
 *------------------------------------------------------------------------------------------------------------*/
__task void send_task (void) {
	T_MEAS *mptr;

    os_mbx_init (MsgBox, sizeof(MsgBox));	// initialize the mailbox             
         
 	while(1){
 		cnt1++;
  		mptr = _alloc_box (mpool);          // Allocate a memory for the message   

   		for(i=0;i<8;i++)
		{							
			LPC_GPIO0->FIOCLR = SSEL;		// make SS low
	
        	LPC_SPI->SPDR = 0x01;			// Send the start bit
		
		   	os_evt_wait_or( EVT_KEY , 0xffff);
		   	LPC_SPI->SPDR = 0xC0;			// Select channel1 in single ended mode & MSB first format
        
			os_evt_wait_or( EVT_KEY , 0xffff);
        	up4bits = temp;					// temp has SPI data register value 
		       
        	LPC_SPI->SPDR = 0x00;

			os_evt_wait_or( EVT_KEY , 0xffff);
        	low8bits = temp;				// SPI Data Register.
			up4bits = up4bits & 0x0f;      	        
			up4bits <<= 8;       			// shift to the MSB side
			low8bits|=up4bits; 				// combine the lower bits and upper bits
			avg += low8bits;		 

			LPC_GPIO0->FIOSET = SSEL;		// make SS low
		}
	  	
		avg >>= 3;							// right shifted to get the avg of 8 
		avg <<= 1;							// left shifted to multiply by 2 

        analog_val = ((float)avg * (float)VREF)/(float)FULLSCALE; 
        temp_val = (float)(analog_val - 2731.4)/100.0;

    	mptr->adc_out = avg;
		mptr->tempc = temp_val;
			
		avg = 0x00;		

  		os_mbx_send (MsgBox, mptr, 0xffff); // Send the message to the mailbox     
  		os_dly_wait (100);
	}
}

/*--------------------------------------------------------------------------------------------------------------
 *  Task 2: RTX Kernel starts this task with os_tsk_create (rec_task, 1)
 *-------------------------------------------------------------------------------------------------------------*/
__task void rec_task (void) {
	T_MEAS *rptr;

	while(1) {
  		cnt2++;
    	os_mbx_wait (MsgBox, (void **)&rptr, 0xffff); // wait for the message    
    	sprintf (buf,"\nadc_out: %X\r",rptr->adc_out);
		while (buf[k] != '\0'){
	  		while (!(LPC_UART0->LSR & 0x20));
	  		LPC_UART0->THR = buf[k];
			k++;
		}
		k=0; 
    	
		sprintf (buf,"tempc: %4.2f\r",rptr->tempc);
		while (buf[k] != '\0'){
	  		while (!(LPC_UART0->LSR & 0x20));
	  		LPC_UART0->THR = buf[k];
			k++;
		}
		k=0; 
    	
		_free_box (mpool, rptr);           // free memory allocated for message  
	}
}

/*----------------------------------------------------------------------------
 *        Main: Initialize and start RTX Kernel
 *---------------------------------------------------------------------------*/
int main (void) {                     	// program execution starts here       
	LPC_PINCON->PINSEL1 &= 0xFFFFFFFC;	// P0.16 GPIO
	LPC_GPIO0->FIODIR |= SSEL;			// CHIPSELECT LINE MADE AS O/P P0.7
	LPC_GPIO0->FIOSET = SSEL;			// if device is powered up with CS low, set high before comm

  	UART0_Init();						// Initializes UART0 
	SPI_Init();							// Initializes SPI
  	_init_box (mpool, sizeof(mpool),    // initialize the 'mpool' memory for   
              sizeof(T_MEAS));        	// the membox dynamic allocation       
	os_sys_init_prio(init_task, 10);    // initialize and start task1         
}

/*--------------------------------------------------------------------------------------------------------------
 *        Initialize serial interface										 
 *------------------------------------------------------------------------------------------------------------*/
void UART0_Init(void) {
	LPC_SC->PCONP |= 0x00000008;			// UART0 peripheral enable
	LPC_PINCON->PINSEL0 &= ~0x000000F0;
	LPC_PINCON->PINSEL0 |= 0x00000050;		// P0.2 - TXD0 and P0.3 - RXD0
	LPC_UART0->LCR = 0x00000083;			// enable divisor latch, parity disable, 1 stop bit, 8bit word length
	LPC_UART0->DLM = 0X00; 
	LPC_UART0->DLL = 0xA2;      			// select baud rate 9600 bps @25MHz
	LPC_UART0->LCR = 0X00000003;
	LPC_UART0->FCR = 0x07;						
	LPC_UART0->IER = 0X03;	   				// select Transmit and receive interrupt
}

/*--------------------------------------------------------------------------------------------------------------
 *        Initialize SPI interface
 *------------------------------------------------------------------------------------------------------------*/
void SPI_Init(void) {   
	LPC_SC->PCONP |= (1<<8);				// Enable the peripheral SPI
	LPC_PINCON->PINSEL0 |= 0xC0000000;		// P0.15 as SCK 
	LPC_PINCON->PINSEL1 |= 0x0000003C;  	// select MISO-P0.17,MOSI-P0.18

	LPC_SPI->SPCCR = 0x1E; 					// SPI CLOCK SELECTED AS 100KHZ
	LPC_SPI->SPCR = 0xA0; 					// 8 bit data, actve high clk, master SPI mode,SPI Int enable
                         					// Master mode and SCK line is active high
	LPC_SPI->SPINT  = 0x01; 				// clear the interrupt flag
	NVIC_EnableIRQ(SPI_IRQn);
}

/*--------------------------------------------------------------------------------------------------------------
 *        SPI IRQ Handler										 
 *------------------------------------------------------------------------------------------------------------*/
void SPI_IRQHandler(void) {
    temp = LPC_SPI->SPSR;   				// To clear SPIF bit we have to read status register.
    temp = LPC_SPI->SPDR;   				// Then read the data register(optional)
    LPC_SPI->SPINT = 0x01;  				// To clear the SPI interrupt
	isr_evt_set(EVT_KEY , tsk1);   
}
/*----------------------------------------------------------------------------
 * end of file
 *---------------------------------------------------------------------------*/
