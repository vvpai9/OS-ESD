/*-------------------------------------------------------------------------------------------------
 *      RL-ARM - RTX
 *-------------------------------------------------------------------------------------------------
 *      Name:    RTX-SEVEN-SEGMENT-COUNTER.c
 *      Purpose: RTX example program to implement Up-Counter / Down-Counter using pre-emptive task
 *------------------------------------------------------------------------------------------------*/

#include <rtl.h>
#include <LPC17xx.h>

/*\\\\\\\\\\\\\\\DISPLAY ARE CONNECTED IN COMMON CATHODE MODE\\\\\\\\\\\\\\\\\\\\\
	Port0 Connected to data lines of all 7 segement displays
	   a
	  ----
	f|  g |b
	 |----|
	e|    |c
	  ----  . dot
	   d
	a = P0.16					
	b = P0.17
	c = P0.18
	d = P0.19
	e = P0.20 
	f = P0.21
	g = P0.22
	dot = P0.23
	
	
	Select lines for four 7 Segments
	DIG1	P0.28
	DIG2	P0.29
	DIG3	P0.30
	DIG4	P0.31
*/

#define SEG7_CTRL_DIR LPC_GPIO0->FIODIR
#define SEG7_CTRL_SET LPC_GPIO0->FIOSET
#define SEG7_CTRL_CLR LPC_GPIO0->FIOCLR
#define LED_DATA_CLR  LPC_GPIO0->FIOCLR
#define LED_DATA_SET  LPC_GPIO0->FIOSET

#define	DIG4	0x00400000
#define	DIG3	0x00200000
#define	DIG2	0x00100000
#define	DIG1	0x00080000
#define LED_DATA_MASK 0x00000FF0			// P0.4 to P0.11 : Data lines connected to drive Seven Segments

#define KEY_CTRL_DIR LPC_GPIO1->FIODIR
#define KEY_CTRL_SET LPC_GPIO1->FIOSET
#define KEY_CTRL_CLR LPC_GPIO1->FIOCLR
#define KEY_CTRL_PIN LPC_GPIO1->FIOPIN

#define INC 0x00800000		 // KEY SW1 P1.23
#define DEC 0x00400000		 // KEY SW2 P1.22
 
#define EVT_KEY1 0x0001
#define EVT_KEY2 0x0002

unsigned char dig[]={0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F};
unsigned short count=0, qot=0, qot1=0, qot2=0, rem=0, rem1=0, rem2=0;
unsigned short i=0, j=0;

// tsk1, tsk2 are task identifications at run-time
OS_TID tsk1,tsk2, tsk3;

/* Function prototypes */
__task void init_task(void);
__task void task1(void);
__task void task2(void);
__task void task3(void);
void seg7_data_write(unsigned char data);

/*-----------------------------------------------------------------------------------
 *   init_task:  RTX Kernel starts this task with os_sys_init_prio(init_task, 10);
 *---------------------------------------------------------------------------------*/
__task void init_task(void) {
	tsk1 = os_tsk_create(task1,2); 	// task1 at priority 2 
	tsk2 = os_tsk_create(task2,2);	// task2 at priority 2
	tsk3 = os_tsk_create(task3,1);	// task3 at priority 1
	os_tsk_delete_self(); 			// must delete itself before exiting
}
 
/*----------------------------------------------------------------------------
 *   Task 1:  RTX Kernel starts this task with os_tsk_create (task1,2)
 *---------------------------------------------------------------------------*/
__task void task1 (void) {
  	while (1) {
		os_evt_wait_or(EVT_KEY1, 0xFFFF);
		
		if(count == 9999){
			goto disp;
		}
		count++;

		disp:
			qot = count/1000;
			rem1 = count%1000;

			qot1 = rem1/100;
			rem2 = rem1%100;

			qot2 = rem2/10;
			rem = rem2%10;	

			for(i=0; i<1000; i++) {			 // change to inc/dec speed of count
				seg7_data_write(dig[qot]);
				SEG7_CTRL_SET |= DIG1;
				for(j=0; j<500; j++);		 // change to inc/dec brightness of display
				SEG7_CTRL_CLR |= DIG1;

				seg7_data_write(dig[qot1]);
				SEG7_CTRL_SET |= DIG2;
				for(j=0; j<500; j++);	     // change to inc/dec brightness of display
				SEG7_CTRL_CLR |= DIG2;

				seg7_data_write(dig[qot2]);
				SEG7_CTRL_SET |= DIG3;
				for(j=0; j<500; j++);		 // change to inc/dec brightness of display
				SEG7_CTRL_CLR |= DIG3; 

				seg7_data_write(dig[rem]);
				SEG7_CTRL_SET |= DIG4;
				for(j=0; j<500; j++);		 // change to inc/dec brightness of display
				SEG7_CTRL_CLR |= DIG4;
			}
	} //end of while(1)
}

/*----------------------------------------------------------------------------
 *   Task 2:  RTX Kernel starts this task with os_tsk_create (task2,2)
 *---------------------------------------------------------------------------*/
__task void task2(void) {
	while(1) {
		os_evt_wait_or(EVT_KEY2, 0xFFFF);
		if(count == 0x0000) {
			goto disp;
		}
		count--;
		
		disp:
			qot = count/1000;
			rem1 = count%1000;

			qot1 = rem1/100;
			rem2 = rem1%100;

			qot2 = rem2/10;
			rem = rem2%10;	

			for(i=0; i<1000; i++) {			 // change to inc/dec speed of count
				seg7_data_write(dig[qot]);
				SEG7_CTRL_SET |= DIG1;
				for(j=0; j<500; j++);		 // change to inc/dec brightness of display
				SEG7_CTRL_CLR |= DIG1;

				seg7_data_write(dig[qot1]);
				SEG7_CTRL_SET |= DIG2;
				for(j=0; j<500; j++);	     // change to inc/dec brightness of display
				SEG7_CTRL_CLR |= DIG2;

				seg7_data_write(dig[qot2]);
				SEG7_CTRL_SET |= DIG3;
				for(j=0; j<500; j++);		 // change to inc/dec brightness of display
				SEG7_CTRL_CLR |= DIG3; 

				seg7_data_write(dig[rem]);
				SEG7_CTRL_SET |= DIG4;
				for(j=0; j<500; j++);		 // change to inc/dec brightness of display
				SEG7_CTRL_CLR |= DIG4;
			}
	} //end of while(1)
}

/*----------------------------------------------------------------------------
 *   Task 3:  RTX Kernel starts this task with os_tsk_create (task3,1)
 *---------------------------------------------------------------------------*/
__task void task3(void) {
	while(1) {
		if((KEY_CTRL_PIN & INC) == 0) {
			os_evt_set(EVT_KEY1 , tsk1);					// set an event for task1
		}
			
		if((KEY_CTRL_PIN & DEC) == 0) {
			os_evt_set(EVT_KEY2 , tsk2);					// set an event for task2
		}

		qot = count/1000;
		rem1 = count%1000;

		qot1 = rem1/100;
		rem2 = rem1%100;

		qot2 = rem2/10;
		rem = rem2%10;

		for(i=0; i<1000; i++) {			 // change to inc/dec speed of count
			seg7_data_write(dig[qot]);
			SEG7_CTRL_SET |= DIG1;
			for(j=0; j<500; j++);		 // change to inc/dec brightness of display
			SEG7_CTRL_CLR |= DIG1;

			seg7_data_write(dig[qot1]);
			SEG7_CTRL_SET |= DIG2;
			for(j=0; j<500; j++);	     // change to inc/dec brightness of display
			SEG7_CTRL_CLR |= DIG2;
			
			seg7_data_write(dig[qot2]);
			SEG7_CTRL_SET |= DIG3;
			for(j=0; j<500; j++);		 // change to inc/dec brightness of display
			SEG7_CTRL_CLR |= DIG3; 

			seg7_data_write(dig[rem]);
			SEG7_CTRL_SET |= DIG4;
			for(j=0; j<500; j++);		 // change to inc/dec brightness of display
			SEG7_CTRL_CLR |= DIG4;
		}
	}
}

void seg7_data_write(unsigned char data) {
	unsigned int temp=0;
	temp = (data << 4) & LED_DATA_MASK;
    LED_DATA_CLR |= LED_DATA_MASK;
	LED_DATA_SET |= temp;
}

/*----------------------------------------------------------------------------
 *        Main: Initialize and start RTX Kernel
 *---------------------------------------------------------------------------*/
int main (void) {
	LPC_PINCON->PINSEL0 &= 0xFF0000FF;	 // P0.4 to P0.11 GPIO data lines
	LPC_PINCON->PINSEL1 &= 0xFFFFC03F;	 // P0.19 to P0.22 GPIO enable lines
	LPC_PINCON->PINSEL3 &= 0xFFFF0FFF;	 //	P1.22 and P1.23 are selected as GPIO

	LPC_GPIO0->FIODIR |= 0x00780FF0;	 //P0.4 to P0.11 output
	
	KEY_CTRL_DIR &= ~(INC | DEC);
	SEG7_CTRL_DIR |= (DIG1 | DIG2 | DIG3 | DIG4);
	SEG7_CTRL_CLR |= (DIG1 | DIG2 | DIG3 | DIG4);
	
	os_sys_init_prio(init_task, 10);
}
/*----------------------------------------------------------------------------
 * end of file
 *---------------------------------------------------------------------------*/

