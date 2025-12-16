// Internal RTC Demonstration.
//----------------------------------------------------------------
// CONTROLLER 		: LPC-2148  	
// DATE 			: December - 2015
// Developed By 	: Advanced Electronic Systems Bangalore,India
//-----------------------------------------------------------------
// To study the Internal RTC Feature of LPC2148 controller.
// Without ALARM, Display RTC data  through serial. 
//-----------------------------------------------------------------

#include<lpc214x.h>
#include<stdio.h>

#define pclk			3000000
#define prescale_val 	((pclk/32768)-1)
#define pre_fraction 	(pclk - ((prescale_val +1)*32768))
#define ABORT 			0x1B

void RTC_ISR(void)__irq;
void UART0_ISR(void)__irq;

void rtc_read(void);
void rtc_write(void);
void transmit(unsigned char *);

void rtc_init(void);
void uart0_init(void);
volatile unsigned long *psfr;
unsigned char flag =0x00;
unsigned char rxdata=0x00, rx_flag=0x00, tx_flag=0x00;
unsigned char one_sec_int=0x00, alarm_flag = 0x00;
unsigned char temp_rtc, temp_uart0;
unsigned long int delay=0;
unsigned long int rtc_read_data[8];
unsigned char arr1[]="RTC DATA WRITTEN\r";
                                                //SEC  MIN  Hour Date DOW  DOY  Month Year
unsigned long  rtc_write_data[]= {0x32,0X3B,0X17,0X1F,0X04,0X16D,0X0C,0X07DF};

unsigned char  *menu[]= {"\r1.RTC WRITE \r2.RTC READ   \rPRESS 1 or 2 \r",
                         "\rRTC DATA IS\r",
                         "\rYear\tMonth\tDOY\tDOW\tDOM\tHOUR\tMIN\tSEC\r"};
unsigned char const *ptr;
char ascii_val[10][10];


int main()
{
	rtc_init();
	uart0_init();
	while(1)
	{
		transmit(menu[0]);

		rxdata =0x00;
		while((rxdata != '1') && (rxdata != '2'));
		switch(rxdata)
		{
			case '1': 	rtc_write();
                        break;

			case '2':	rtc_read();
						break;

			default :	break;
					
		}
	} 	
}


void rtc_init()
{
	CCR = 0x02;
	PREINT = prescale_val;  //Prescaler Integer Register
	PREFRAC = pre_fraction;
	CCR = 0x01;             //Clock Control Register 
	CIIR = 0x01;            //To generate interrupt for every second 	
	VICVectAddr0 = (unsigned long) RTC_ISR; // Assign the RTC ISR function 
   	VICVectCntl0 = 0x20 | 13;               // Assign the VIC channel RTC to interrupt priority 0
   	VICIntEnable |= 0x00002000;             // Enable the RTC interrupt
}

void uart0_init()
{
	PINSEL0 = 0x00000005;
	U0LCR   = 0x00000083;	//enable baud rate divisor loading and select the data format
	U0DLM   = 0x00; 
	U0DLL   = 0x13;      	//select baud rate 9600 bps
	U0LCR   = 0x00000003;
    U0IER   = 0x03;

    VICVectAddr1 = (unsigned long)UART0_ISR;//UART 1 INTERRUPT 
    VICVectCntl1 = 0x20|6;  
	VICIntEnable |= 0x00000040;  

}


void rtc_write()
{				
	unsigned int i=0;
		
	psfr = (volatile unsigned long *)&SEC;

	for(i=0;i<8;i++)
	{
		*psfr++ = rtc_write_data[i];
	}	
        
    U0THR=0x0d;
    while(tx_flag ==0x00);
	tx_flag =0x00;

    for(i=0;i<20;i++)
    {
      U0THR = arr1[i];        // Data Writing
      while(tx_flag ==0x00);
	  tx_flag =0x00;
	  			
    }
}

void rtc_read()
{
	unsigned int i = 0  ;
   	char *ptr1 ;        

	transmit(menu[1]);     
	while(rxdata != ABORT)
	{
		while(one_sec_int ==0x00);
		one_sec_int = 0x00;
		
		transmit(menu[2]);

		for(i=0;i<8;i++)
		{ 
			sprintf(&ascii_val[i][0],"%ld",rtc_read_data[i]);
		}
		for( i = 15 ; i > 7 ; i-- )
		{
			ptr1 = &ascii_val[i-8][0];
			
			transmit(ptr1);

            U0THR = '\t';
            while(tx_flag == 0x00);
            tx_flag =0x00;
		}
		U0THR = '\r';
		while(tx_flag ==0x00);
		tx_flag =0x00;
	} 	
}

void RTC_ISR(void)__irq
{
	unsigned int i=0;
	temp_rtc = ILR;
	if(temp_rtc == 0x01)
	{		
		psfr = (volatile unsigned long *)&SEC;
		for(i=0;i<8;i++)
		{
			rtc_read_data[i] = *psfr++;

		}
		
		one_sec_int = 0xff;
	}
	ILR = temp_rtc;               //To clear the interrupt set
	VICVectAddr=0;                //Acknowledge Interrupt
}

void UART0_ISR(void)__irq
{
	temp_uart0 = U0IIR;
	temp_uart0 &= 0x06;
	if(temp_uart0  == 0x02)
	{       
		tx_flag = 0xff;
	}

	else if(temp_uart0  == 0x04)
	{
         rxdata = U0RBR;
         rx_flag = 0xff;
	}
    VICVectAddr=0; 

}

void transmit(unsigned char * buff)
{
	unsigned char i = 0;
	while(buff[i] != '\0')
    {
		U0THR = buff[i];
		while(tx_flag ==0x00);
		tx_flag =0x00;

		i++ ;
    }
}
	
