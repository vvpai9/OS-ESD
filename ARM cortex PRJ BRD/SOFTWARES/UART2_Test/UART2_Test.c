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
#include"UART2.h"

unsigned char recv_buf[50], recv_data=0, recv_index=0;
unsigned char rx2_flag=0, tx2_flag=0;

int main(void)
{
	SystemInit();
	SystemCoreClockUpdate();
	UART2_Init();
	while(1)
	{
		while(rx2_flag == 0x00);
		rx2_flag = 0x00;
		LPC_UART2->THR = recv_data;
		while(tx2_flag == 0x00);
		tx2_flag = 0x00;

	}

}

void UART2_Init(void)
{
	LPC_SC->PCONP |= (1<<24);	//UART2 peripheral enable
	LPC_PINCON->PINSEL4 &= ~0x000F0000;
	LPC_PINCON->PINSEL4 |= 0x000A0000;
	LPC_UART2->LCR = 0x00000083;	//enable divisor latch, parity disable, 1 stop bit, 8bit word length
	LPC_UART2->DLM = 0X00; 
	LPC_UART2->DLL = 0x13;      	//select baud rate 9600 bps
	LPC_UART2->LCR = 0X00000003;
	LPC_UART2->FCR = 0x07;
	LPC_UART2->IER = 0X03;	   		//select Transmit and receive interrupt

	NVIC_EnableIRQ(UART2_IRQn);		//Assigning channel
}

void UART2_IRQHandler(void)
{
	unsigned long Int_Stat;
	Int_Stat = LPC_UART2->IIR;		//reading the data from interrupt identification register
	Int_Stat = Int_Stat & 0x06;		//masking other than txmit int & rcve data indicator

	if((Int_Stat & 0x02)== 0x02)	//transmit interrupt
		tx2_flag = 0xff;

	else if( (Int_Stat & 0x04) == 0x04)  //recve data availble
	{
		recv_data = LPC_UART2->RBR;
		rx2_flag = 0xff;
	}
}
