/*****************************************************************************
 * Internal RTC Test
 * Developed By
 * Advanced Electronics Systems, Bengaluru
 *---------------------------------------------------------------------------
 * Controller : LPC1768, ALS-PJTBRD-ARMCTXM3-01
 * According to this software, Internal RTC is operated based on the commands
 * sent from PC terminal through UART0. A read and write operation is done and
 * read values are sent to serial port UART0. Untill Esc key pressed at PC 
 * key board a read operation will continue 
 *****************************************************************************/


 #include<LPC17xx.h>
 #include<stdio.h>
 #include "UART0.h"

 #define pclk          3000000
 #define prescale_val  ((pclk/32768)-1)
 #define pre_fraction  (pclk - ((prescale_val +1)*32768))

 #define	CCR_CLKEN	0x00000001;	 	//Clock bit enable in CCR register
 #define	CIIR_IMSEC	0x00000001;		//Inc of second value generates interrupt
 #define	ILR_RTCCIF	0x00000001;		//Counter Increment Interrupt bit in ILR register
 #define	CCR_CTCRST	0x00000002;		//Reset bit of Internal oscillator in CCR register
 #define 	ABORT         0x1B

 unsigned long int delay=0;
 unsigned char RTC_Int = 0, temp_rtc=0;
 unsigned long int rtc_read_data[8];
 unsigned char arr1[20]="RTC DATA WRITTEN";
 unsigned char const *ptr;          //SEC  MIN  Hour Date DOW  DOY  Month Year
 volatile unsigned long *psfr;

 unsigned char RTC_Int, temp_rtc;
 unsigned long int rtc_read_data[];
 unsigned char arr1[];
 unsigned const char *ptr;
 volatile unsigned long *psfr;
 unsigned long static const int rtc_write_data[]= {0x3B,0X3B,0X0C,0X14,0X06,0X8C,0X05,0X07DE};
 unsigned char menu1[] = {"1.RTC WRITE\r2.RTC READ\rPRESS 1 or 2\r"};
 unsigned char menu2[] = {"RTC DATA IS\r"};
 unsigned char menu3[] = {"Year\tMonth\tDOY\tDOW\tDOM\tHOUR\tMIN\tSEC\r"};
 unsigned char ascii_val[10][10];

 void RTC_Init(void);
 void RTC_Start(void);
 void RTC_Stop(void);
 void RTC_Reset(void);
 void RTC_IRQHandler(void);
 void RTC_SetTime(void);
 void RTC_GetTime(void);

 int main()
 {
	unsigned int i=0;

    SystemInit();
    SystemCoreClockUpdate();

	RTC_Init();
    UART0_Init();
    RTC_Start();

	while(1)
	{
		i=0;
        while(menu1[i]!='\0')
        {
 	       LPC_UART0->THR = menu1[i++];
           while(tx0_flag ==0x00);
           tx0_flag =0x00;
    	}
		recv_data = 0x00;
		while((recv_data != '1') && (recv_data != '2'));
		switch(recv_data)
		{
			case '1': 	RTC_SetTime();
            break;

			case '2':	RTC_GetTime();
			break;

			default :	break;
					
		}
	}
 	
}

void RTC_Init(void)
{
	LPC_SC->PCONP	|= (1<<9);		//Power the peripheral RTC
	LPC_RTC->AMR	= 0;		  //Initialize the registers
  	LPC_RTC->CIIR	= 0;
  	LPC_RTC->CCR	= 0;

	NVIC_EnableIRQ(RTC_IRQn);
 
  	return;
}	
void RTC_Start(void)	
{
	LPC_RTC->CCR |= CCR_CLKEN;		//The time counters are enabled
	LPC_RTC->ILR |= ILR_RTCCIF;		//Clears if there is an interrupt
	LPC_RTC->CIIR |= CIIR_IMSEC;     //To generate interrupt for every second 	

	return;
}

void RTC_Stop(void)
{
	LPC_RTC->CCR &= ~CCR_CLKEN;		//Disable the clock enable bit
	return;
}

void RTC_Reset(void)
{
	LPC_RTC->CCR |= CCR_CTCRST;
	return; 
}
void RTC_IRQHandler(void)
{
	unsigned int j=0;

	temp_rtc = LPC_RTC->ILR;

	if(temp_rtc == 0x00000001)
	{		
          psfr = (volatile unsigned long *)&LPC_RTC->SEC;
          
          for(j=0;j<8;j++)
            rtc_read_data[j] = *psfr++;
          
          RTC_Int = 0xff;
	}

	LPC_RTC->ILR |= ILR_RTCCIF;	//Clear the interrupt bit
	
	return;
}

void RTC_SetTime(void)
{
	unsigned int j=0;
		
	psfr = (volatile unsigned long *)&LPC_RTC->SEC;

	for(j=0;j<8;j++)
	{
		*psfr++ = rtc_write_data[j];
	}	
        
        LPC_UART0->THR = 0x0d;
        while(tx0_flag ==0x00);
              tx0_flag =0x00;

        for(j=0;j<20;j++)
        {
          LPC_UART0->THR = arr1[j];        // Data Writing
          while(tx0_flag ==0x00);
                tx0_flag =0x00;			
        }

        LPC_UART0->THR=0x0d;
        while(tx0_flag ==0x00);
            tx0_flag =0x00;
	 
	return;
}

void RTC_GetTime(void)
{
	unsigned int j=0;
   	char *ptr1;        

        while(menu2[j] !='\0')
        {
            LPC_UART0->THR = menu2[j++];
            while(tx0_flag ==0x00);
            tx0_flag =0x00;
        }        
	while(recv_data != ABORT)
	{
          while(RTC_Int ==0x00);
          RTC_Int = 0x00;
          j=0;
          while(menu3[j] !='\0')
          {
            LPC_UART0->THR = menu3[j++];
            while(tx0_flag ==0x00);
            tx0_flag =0x00;
          }                
                
          for(j=0;j<8;j++)
          sprintf(&ascii_val[j][0],"%ld",rtc_read_data[j]);
          
          for(j=15;j>7;j--)
          {
            ptr1 = &ascii_val[j-8][0];
                  
            while(*ptr1 !='\0')
            {
              LPC_UART0->THR = *ptr1++;
              while(tx0_flag == 0x00);
              tx0_flag = 0x00;
            }
            
            LPC_UART0->THR = '\t';
            while(tx0_flag ==0x00);
            tx0_flag =0x00;
          }
          
          LPC_UART0->THR = '\r';
          while(tx0_flag ==0x00);
          tx0_flag =0x00;

	} 	
	return;
}
