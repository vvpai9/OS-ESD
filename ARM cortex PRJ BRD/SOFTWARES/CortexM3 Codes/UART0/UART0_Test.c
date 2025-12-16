/*****************************************************************************
 * UART0 test Demonstration
 * Developed by
 * Advanced Electronics Systems. Bengaluru
 *----------------------------------------------------------------------------
 * This project is for studying the UART0 communication. Code is written in such 
 * a way that, a data is received from UART0 and same data is sent back to the 
 * same port. Use PC communication terminal to see the working. A character typed
 * at the PC key board is sent to the project board via Hyper terminal. And same 
 * character is sent back from the controller to hyper terminal. The data is 
 * displayed on Hyper terminal. 
 *****************************************************************************/

#include<LPC17xx.h>
#include"UART0.h"

unsigned char recv_buf[50], recv_data=0, recv_index=0;
unsigned char rx0_flag=0, tx0_flag=0;

int main(void)
{
	SystemInit();
	SystemCoreClockUpdate();
	UART0_Init();
	while(1)
	{
		while(rx0_flag == 0x00);
		rx0_flag = 0x00;
		LPC_UART0->THR = recv_data;
		while(tx0_flag == 0x00);
		tx0_flag = 0x00;

	}

}

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
