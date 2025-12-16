// UART1 INTERFACING.
//--------------------------------------------------------------
// CONTROLLER 		: LPC-2148  	
// DATE 			: DECEMBER - 2015
// Developed By 	: Advanced Electronic Systems Bangalore,India
//----------------------------------------------------------------
//----------------------------------------------------------
// char typed in keyboard will be displayed through serial.
//----------------------------------------------------------

#include<lpc21xx.h>

void uart1_init(void);
void uart_interrupt(void)__irq;

unsigned char temp , tx_flag = 0x00 , rx_flag = 0x00 ;

int main (void)
{	
    uart1_init();

	while(1) 
	{
		while(rx_flag == 0x00 );	
		rx_flag = 0x00 ;

		U1THR = temp ;
		while(tx_flag == 0x00 );	
		tx_flag = 0x00 ;
	}
}			

void uart1_init()
{
	PINSEL0 |= 0X00050000;  	//select TXD1 and RXD1 lines
	U1LCR  = 0X00000083;		//enable baud rate divisor loading and
	          					//select the data format
    U1DLM = 0X00; 
	U1DLL = 0x13;      			//select baud rate 9600 bps
	U1LCR  = 0X00000003;
	U1FCR = 0x01;
	U1IER = 0X03;	   			//select Transmit and Recieve interrupt
	
    VICVectAddr0 = (unsigned long)uart_interrupt;	//UART 1 INTERRUPT 
    VICVectCntl0 = 0x20|7;  						// Assign the VIC channel uart-1 to interrupt priority 7
	VICIntEnable = 0x00000080;     					// Enable the uart-1 interrupt		
}
 
void uart_interrupt(void)__irq
{
	unsigned char temp1 = 0x00 ;

	temp1 = U1IIR;
	temp1 = temp1 & 0x06;
	
	if(temp1  == 0x02)
	{
		tx_flag = 0xff ;
//		VICVectAddr=0; 
	}
	
	else if(temp1  == 0x04)
	{
		temp = U1RBR;
		rx_flag = 0xff ;

//		VICVectAddr=0; 
	}

	VICVectAddr=0; 
}
