/*----------------------------------------------------------------------------------
 *      RL-ARM - RTX
 *----------------------------------------------------------------------------------
 *      Name:    RTX-TIMER.c
 *      Purpose: RTX example to implement timer : 00:00:00 hour:min:sec
 *---------------------------------------------------------------------------------*/
#include <rtl.h>
#include <LPC17xx.h>
#include <stdio.h>

// tsk1 will contain task identifications at run-time
OS_TID tsk1, tsk2;
OS_MUT mutex;

#define SW1	0x00800000					// P1.23 KEY
										
typedef struct {
	U8 hour;
	U8 min;
	U8 sec;
}TIME;

TIME t;	   								// declare global struct
unsigned char buf[15], i=0, t_var=0, sec_var=0, min_var=0, hr_var=0, key_flag=0;
int delay=0, temp=0;

/* Function prototypes */
__task void init_task(void);
__task void task1(void);
__task void task2(void);
void UART0_Init(void);
unsigned char hex_dec(unsigned char c_var);
unsigned char dec_ascii(unsigned char ch);

/*-----------------------------------------------------------------------------------
 *   init_task:  RTX Kernel starts this task with os_sys_init_prio(init_task, 10);
 *---------------------------------------------------------------------------------*/
__task void init_task(void) {
	os_mut_init(mutex);					// Initialize
	tsk1 = os_tsk_create(task1,2); 		// task1 at priority 2 
	tsk2 = os_tsk_create(task2,1);  	// task2 at priority 1 
	os_tsk_delete_self(); 				// must delete itself before exiting
}
 
/*----------------------------------------------------------------------------
 *   Task 1:  RTX Kernel starts this task with os_tsk_create (task1,2)
 *---------------------------------------------------------------------------*/
__task void task1 (void) {

	os_itv_set(100); 					// set wait interval:  1 second    
    /* do some actions at regular time intervals */
	while (1) {
		LPC_GPIO2->FIOSET = 0x00001000;
		if(++t.sec == 60) {
			 t.sec = 0;
			 if(++t.min == 60) {
			 	t.min = 0;
				if(++t.hour == 24) {
					t.hour = 0;
				}
			}
		}

		hr_var = hex_dec(t.hour);
		min_var = hex_dec(t.min);
		sec_var = hex_dec(t.sec);

		t_var = hr_var;
		t_var &= 0xf0;
		t_var >>= 4;
		buf[0] = dec_ascii(t_var);
		t_var = hr_var & 0x0f;
		buf[1] = dec_ascii(t_var);

		buf[2] = ':';

		t_var = min_var;
		t_var &= 0xf0;
		t_var >>= 4;
		buf[3] = dec_ascii(t_var);
		t_var = min_var & 0x0f;
		buf[4] = dec_ascii(t_var);

		buf[5] = ':';

		t_var = sec_var;
		t_var &= 0xf0;
		t_var >>= 4;
		buf[6] = dec_ascii(t_var);
		t_var = sec_var & 0x0f;
		buf[7] = dec_ascii(t_var);

		buf[8] = '\r';
	
		while(buf[i] != '\0'){		  		// Displaying on hyper terminal
	  		while (!(LPC_UART0->LSR & 0x20));
	  		LPC_UART0->THR = buf[i];
			i++;
		}
		i=0;   
		
		if(	key_flag == 1) {
			key_flag = 0;
			LPC_GPIO2->FIOCLR = 0x00001000;
			t.hour=0;
			t.min=0;
			t.sec=0;
		}

		os_itv_wait();
	}
}

/*----------------------------------------------------------------------------
 *   Task 2:  RTX Kernel starts this task with os_tsk_create (task2,1)
 *---------------------------------------------------------------------------*/
__task void task2 (void) {
	while(1) {
		os_mut_wait(mutex, 0xffff);
		temp = LPC_GPIO1->FIOPIN;
		temp = LPC_GPIO1->FIOPIN & SW1;
	    if( temp == 0x00000000)   			// check to see if key has been released
		{
			for(delay=0; delay < 5000; delay++);
			temp = LPC_GPIO1->FIOPIN;
			temp = LPC_GPIO1->FIOPIN & SW1;
	    	if( temp == 0x00000000)  		// check to see if key has been released
			{								
				key_flag = 1;		 		// set flag if key pressed
				os_mut_release(mutex);
			}
		}
	}
}

/*------------------------------------------------------------------------------
 *        Main: Initialize and start RTX Kernel
 *----------------------------------------------------------------------------*/
int main(void) {
	LPC_PINCON->PINSEL4 &= 0xFCFFFFFF;	// P2.12 GPIO for LED
	LPC_GPIO2->FIODIR = 0x00001000;		// P2.12 is assigned output
	LPC_PINCON->PINSEL3 &= 0xFFFF3FFF;	//
	LPC_GPIO1->FIODIR &= 0xFF7FFFFF;	//

	UART0_Init();	  					// Initialize serial interface
	os_sys_init_prio(init_task, 10);	// Initialize init_task and start RTX Kernel
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

/*----------------------------------------------------------------------------
 *        Hex to decimal conversion 
 *---------------------------------------------------------------------------*/
unsigned char hex_dec(unsigned char c_var) {
	unsigned int mul=1,dec_val =0;
	unsigned char rem=0;
  
	while(c_var != 0x00)					// eg: hex value is 0x86 or decimal 134
    {			  							// first pass gives 4 as remainder and 0d in c_var
    	rem = (c_var)%0x0a;  				// 4 is positioned in the ls nibble of dec_val by the 
    	c_var = (c_var)/0x0a;				// multiplication of rem * mul ; in the next pass
    	dec_val = dec_val + (rem*mul); 		// we get 3 remainder and 1 in c_var and the 3
    	mul = mul*16;  						// is positioned in the higher nibble of dec_val by 2*rem ; this repeats 
    }                						// till c_var goes to zero and a packed bcd is in dec_val
   	return dec_val;	
}											// end of unsigned int hex_2_dec(unsigned int c_var) 

/*----------------------------------------------------------------------------
 *        decimal - ascii Function
 *---------------------------------------------------------------------------*/
unsigned char dec_ascii(unsigned char ch) {
	if(ch > 9)
		ch+=0x37;
	else
		ch+=0x30;
	return ch;
} 

/*----------------------------------------------------------------------------
 * end of file
 *---------------------------------------------------------------------------*/

