/**************************************************************************
 * UART0.c for testing UART0
 * Created by 													  
 * Advanced Electronics Systems, Bengaluru
 *-------------------------------------------------------------------------
 * Controller: LPC1768
 * UART0 is initialised with receive and transmit interrupt enable. Baud rate
 * of approximately 9600 is selected without FDR register.
 * Can be used in the necessary softwares by including UART0.h
 * Port lines used: P0.2 and P0.3
 ***************************************************************************/

/**************************************************************************
 * UART0.c for testing UART0
 * Created by 													  
 * Advanced Electronics Systems, Bengaluru
 *-------------------------------------------------------------------------
 * Controller: LPC1768
 * UART0 is initialised with receive and transmit interrupt enable. Baud rate
 * of approximately 9600 is selected without FDR register.
 * Can be used in the necessary softwares by including UART0.h
 * Port lines used: P0.2 and P0.3
 ***************************************************************************/

 #include <LPC17xx.h>
 #include "uart0.h"

 unsigned char recv_buf[50], recv_data=0, recv_index=0;
 unsigned char rx0_flag=0, tx0_flag=0;

 void UART0_Init(void)
 {
	LPC_SC->PCONP |= 0x00000008;	//UART0 peripheral enable
	LPC_PINCON->PINSEL0 &= ~0x000000F0;
	LPC_PINCON->PINSEL0 |= 0x00000050;
	LPC_UART0->LCR = 0x00000083;	//enable divisor latch, parity disable, 1 stop bit, 8bit word length
	LPC_UART0->DLM = 0X00; 
	LPC_UART0->DLL = 0x13;      	//select baud rate 9600 bps
	LPC_UART0->LCR = 0X00000003;
	LPC_UART0->FCR = 0x07;
	LPC_UART0->IER = 0X03;	   		//select Transmit and receive interrupt

	NVIC_EnableIRQ(UART0_IRQn);		//Assigning channel
 } 

 void UART0_IRQHandler(void)
 {
	unsigned long Int_Stat;
	Int_Stat = LPC_UART0->IIR;		//reading the data from interrupt identification register
	Int_Stat = Int_Stat & 0x06;		//masking other than txmit int & rcve data indicator

	if((Int_Stat & 0x02)== 0x02)	//transmit interrupt
		tx0_flag = 0xff;

	else if( (Int_Stat & 0x04) == 0x04)  //recve data availble
	{
		recv_data = LPC_UART0->RBR;
		rx0_flag = 0xff;
	}
 }
