// 25LC256 SPI EEPROM INTERFACING
//----------------------------------------------------------------
// CONTROLLER 		: LPC-2148  	
// DATE 			: December - 2015
// Developed By 	: Advanced Electronic Systems Bangalore,India
//----------------------------------------------------------------
//----------------------------------------------------------------
// Data read and write is controlled through Serial.
//----------------------------------------------------------------

#include<lpc214x.h>
#include<stdio.h>

#define WREN_SET 	0X06     // Set the write enable latch (enable write operations)
#define WREN_CMD 	0X02     // Write data to memory array beginning at selected address
#define WREN_RESET 	0X04   	 // Reset the write enable latch (disable write operations)
#define READ_SET 	0X03     // Read data from memory array beginning at selected address

#define MSB_ADDRESS 0X00   	// dont care
#define LSB_ADDRESS 0X00   	// 16 bit address instruction 

#define CHIP_SEL	0X00100000;

void UART0_interrupt(void)__irq;	//declaration of uart1 ISR

void nvrom_read(void);    			// Nvrom Read func
void nvrom_write(void);   			// Nvrom Write func
void uart0_init(void);    			// UART0 initialisation
void spi1_init(void);     			// spi1 initialisation
void transmit( char * ) ;

char arr1[] = "\rDATA WRITTEN INTO EEPROM\r" ;
char arr2[] = "\rSTORED DATA = " ;
char menu[]= {"\r1.Nvrom WRITE \r2.Nvrom READ \rPRESS 1 or 2 \r"};

unsigned char nvrom_wdata[]= {0x00,0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x99};

unsigned char  tx_flag = 0x00, rx_flag = 0x00 ,  tem=0x00 , tem_var = 0x00  ;
unsigned char rxdata = 0x00 , temp_uart1 = 0x00 ;
char buf[10], nvrom_rdata[10] ;
unsigned char *nvrom_ptr , temp1 , dummy = 0x00 ;

int main()
{
	IO0DIR = CHIP_SEL; 	  		// P0.20 made as output.
	PINSEL0 |= 0x00000005;  	// select TXD0 and RXD0 lines  OF UART0 
	
	IO0SET = CHIP_SEL;  		// chip select dissable( P0.20 made high).
	
	uart0_init(); 
	spi1_init();
	
	while(1)
	{
		transmit(menu);

		rxdata =0x00;
		while((rxdata != '1') && (rxdata != '2'));	// waiting for command from serial.
		switch(rxdata)
		{
			case '1': 	nvrom_write();
                        break;

			case '2':	nvrom_read();
						break;

			default :	break;
					
		}// end of switch()

	}//end of while(1)  
 	
}//end of main()


void uart0_init(void)
{
	U0LCR  = 0X83;  	//enable baud rate divisor loading and select the data format  
    U0DLM  = 0X00;  	//select baud rate 9600 bps (the HIGHER 8 bits of the divisor) 
    U0DLL  = 0x13;  	//select baud rate 9600 bps (the LOWER 8 bits of the divisor)
    U0LCR  = 0X03;  	//select 8 bit character length
    U0IER  = 0x03;  	//enable the interrupt

    VICVectAddr1 = (unsigned long)UART0_interrupt;	//UART 0 INTERRUPT          
    VICVectCntl1 = 0x20|6;  			//Assign the VIC channel UART1 to interrupt priority 0
    VICIntEnable |= 0x00000040;  		// Enable the UART1 interrupt
}

void spi1_init(void)
{
	PCONP |= (1<<10);			//Enable the peripheral SPI1
  	PINSEL1 &= ~0x000000FC;		//P0.17 - SCK. P0.18 - MISO1. P0.19 - MOSI1
  	PINSEL1 |= 0x000000A8;

  	SSPCR0 = 0x707;	 			// 8 bit transfer , SPI , CPOL=CPHA=0, PCLK / (CPSDVSR * [SCR+1]) ,SCR=7
  	SSPCR1 = 0x02;				//  enable SSP in master mode
  	SSPICR = 0x01;				// clear intr
  	SSPCPSR = 0x02;				// CPSDVSR = 2 ;
}

void nvrom_write()
{
	unsigned char  count = 0x00  ;
	unsigned int i = 0 ;
	
	nvrom_ptr = nvrom_wdata ;
   
   	IO0CLR = CHIP_SEL ;	 					// Enabling SPI by setting CS line low.
   	for( i = 0 ; i <= 100 ; i++ ) ;  	
	
	while ( ((tem_var = SSPSR) & 0x01) == 0x00 );  // checking for transmit FIFO empty.
	SSPDR = WREN_SET; 							   // write enable
	
	while ( ((tem_var = SSPSR) & 0x10) == 0x10 );  // checking for SSP controller busy/idle.
	dummy = SSPDR;								   // dummy read to clear receive FIFO
  		   												
    IO0SET = CHIP_SEL;  							//CS line should be  Toggle after write enable set. 

    for( i = 0 ; i <= 2000 ; i++ ) ;  
					  
    IO0CLR = CHIP_SEL;							
	
	while ( ((tem_var = SSPSR) & 0x01) == 0x00 );  	// checking for transmit FIFO empty.
	SSPDR = WREN_CMD;    							// write command.
	
	while ( ((tem_var = SSPSR) & 0x10) == 0x10 );  	// checking for SSP controller busy/idle.
	dummy = SSPDR;									// dummy read to clear receive FIFO
	
	while ( ((tem_var = SSPSR) & 0x01) == 0x00 ); 	// checking for transmit FIFO empty.
	SSPDR = MSB_ADDRESS; 							// writing MSB of address

	while ( ((tem_var = SSPSR) & 0x10) == 0x10 ); 	// checking for SSP controller busy/idle.
	dummy = SSPDR;									// dummy read to clear receive FIFO
	     
	while ( ((tem_var = SSPSR) & 0x01) == 0x00 );  	// checking for transmit FIFO empty.
	SSPDR = LSB_ADDRESS;   							// writing LSB of address
								
	while ( ((tem_var = SSPSR) & 0x10) == 0x10 ); 	// checking for SSP controller busy/idle.
	dummy = SSPDR;								    // dummy read to clear receive FIFO

 	for(count = 0 ; count < 10 ; count++ )
    {
		tem = nvrom_ptr[count];						   

		while ( ((tem_var = SSPSR) & 0x01) == 0x00 );  // checking for transmit FIFO empty.
		SSPDR = tem;
		
		while ( ((tem_var = SSPSR) & 0x10) == 0x10 );  // checking for SSP controller busy/idle.								   // writing data 
		dummy = SSPDR;								   // doing dummy read		
	}
	
	IO0SET = CHIP_SEL; 		 // dissabling SPI  by setting CS line HIGH. 
	
	transmit(arr1);
	                                    
} // End of Nvrom_write

void nvrom_read()
{
	unsigned char  count = 0x00 ;
	unsigned int i = 0 , j = 0 ;

	IO0CLR = CHIP_SEL;     	 		// Enabling SPI by setting CS line low.
	for( i = 0 ; i < 1000 ; i++ );	

	
	while ( ((tem_var = SSPSR) & 0x01) == 0x00 ); 	 // checking for transmit FIFO empty. 
	SSPDR = READ_SET; 								 // write command.
	
	while ( ((tem_var = SSPSR) & 0x10) == 0x10 ); 	 // checking for SSP controller busy/idle.	
	dummy = SSPDR;									 // dummy read to clear receive FIFO
	
  	while ( ((tem_var = SSPSR) & 0x01) == 0x00 );	 // checking for transmit FIFO empty.
	SSPDR = MSB_ADDRESS;							 // writing MSB of address 

	while ( ((tem_var = SSPSR) & 0x10) == 0x10 ); 	 // checking for SSP controller busy/idle.
	dummy = SSPDR;									 // dummy read to clear receive FIFO
	
	while ( ((tem_var = SSPSR) & 0x01) == 0x00 ); 	// checking for transmit FIFO empty.
	SSPDR = LSB_ADDRESS;							// writing LSB of address

	while ( ((tem_var = SSPSR) & 0x10) == 0x10 ); 	 // checking for SSP controller busy/idle.
	dummy = SSPDR;									 // dummy read to clear receive FIFO
	
/*	while(((tem_var = SSPSR) & 0x04) == 0x04 )	   // doing dummy read till receive buffer gets empty.
		dummy = SSPDR; 	 */

	for( count = 0 ; count < 10; count++ )
	{        
		while(((tem_var = SSPSR) & 0x01) == 0x00 );  
		SSPDR = 0x00 ;									 // dummy write
		
		while (((tem_var = SSPSR) & 0x10) == 0x10 );

		while(((tem_var = SSPSR) & 0x04) == 0x00 ); 
		temp1 = SSPDR;  
        nvrom_rdata[count]= temp1; 						 // data read
	}

    IO0SET = CHIP_SEL; 		  // dissabling SPI  by setting CS line HIGH. 

	transmit(arr2);
      
    for( i = 0 ; i < 10 ; i++ )
    {
		sprintf(buf,"%x ", nvrom_rdata[i]);	   // transmitting data through serial.
		transmit(buf);
	}
	
	transmit("\r");
	
	for(j = 0 ; j <= 300000 ; j++ ) ;
    
}// End of Nvrom_read

void UART0_interrupt(void)__irq
{
	temp_uart1 = U0IIR;
	temp_uart1 &= 0x06;

	if(temp_uart1  == 0x02)    
		tx_flag = 0xff;

	else if(temp_uart1  == 0x04)
	{
		rxdata = U0RBR;
		rx_flag = 0xff;
	}
	VICVectAddr = 0 ; 		// clear interrupt
}

void transmit( char * buff )
{
	unsigned char i = 0 ;
		
	while( buff[i] != '\0' )
	{
		U0THR = (buff[i]);
		while(tx_flag ==0x00);
		tx_flag =0x00;
		i++ ;
	}
}


