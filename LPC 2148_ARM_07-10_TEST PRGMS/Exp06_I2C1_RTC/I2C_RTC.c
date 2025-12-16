// DS1307 I2C RTC INTERFACE.
//----------------------------------------------------------------------
// CONTROLLER 		: LPC-2148  	
// DATE 			: December - 2015
// Developed By 	: Advanced Electronic Systems Bangalore,India
//----------------------------------------------------------------------
//----------------------------------------------------------------------
// This project is to study I2C protocol with DS1307 RTC IC.
// data read write can be control by user using serial communication.
// if user press '1' , the default data will be written to DS1307 and by
// pressing '2' the RTC data can be read and display it through serial
//-----------------------------------------------------------------------

#include <lpc214x.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define RTC_ADD         0xd0
#define	SMB_START		0x08
#define	SMB_RPT_START	0x10
#define	SMB_MTADDACK	0x18
#define	SMB_MTADDNACK	0x20
#define	SMB_MTDBACK		0x28
#define	SMB_MTDBNACK	0x30
#define	SMB_MTARBLOST	0x38
#define	SMB_MRADDACK	0x40
#define	SMB_MRADDNACK	0x48
#define	SMB_MRDBACK		0x50
#define	SMB_MRDBNACK	0x58

#define READ			0x01
#define WRITE			0x00
#define RD				0x00
#define WR				0xff
#define CA            	0x0d

void init_devices(void);
void port_init(void);
void rtc_write(void);
void rtc_read(void);
unsigned char hex_to_ascii( unsigned char );

void i2c_interrupt(void) __irq;
void uart_interrupt(void) __irq;

unsigned char rx_flag , tx_flag , ser_int_flg , ser_rec_int_flg , temp ;
unsigned int m =0,n=0;
unsigned char rxdata ;

//RTC WRITE
unsigned char rtc_data[] = { 0x00,0x17,0x05,0x02,0x01,0x03,0x16 };
unsigned int i,l,f1=0,f2=0,p; 
unsigned int bcnt;	
unsigned char flag;

//RTC read	 
unsigned char status , read_over=0; 
unsigned char count1; 
unsigned char rxd_data[8],write_flag=0,read_flag=0,rtc_flag=0,var1=0,var2=0;   
unsigned char *rtc_disp[]= { "sec  : ",
		 	  			     "Min  : ",
						     "hour : ",
						     "Day  : ",
						     "Date : ",
						     "Month: ",
						     "Year : " }; 
							  
unsigned char *b1[] = {"                   ",
                       "Peripheral testing",
                       "1.Test RTC WRITE",
		       		   "2.Test RTC READ"  };	 

int main(void)
{
	port_init();
	init_devices();
	
	while(1)
	{
		U0THR = CA;       	    	//Go to the next line 
		while(ser_int_flg ==0);
		ser_int_flg =0;
		
		for(m=0;m<=3;m++)
		{
			n=0;
			while(*(b1[m] + n) != '\0' )
			{
				U0THR= *(b1[m] + n );
				while( ser_int_flg == 0 );
                ser_int_flg =0;
			   	n=n+1;
         	} 
			U0THR = CA;
			while(ser_int_flg ==0);
			ser_int_flg =0;
		}

  	  	while(ser_rec_int_flg == 0);
	  	temp = rxdata;
	    ser_rec_int_flg =0;

	  	switch(temp)
	  	{
			case '1': write_flag = 0x00;                                                      
                      read_flag = 0x00;
                      rtc_write();
                      break;

			case '2': write_flag = 0x00;                                                      
                      read_flag = 0x00;
                      rtc_read();		
                      break;

			default : break;
		} 
	}
}


void init_devices()
{
	U0LCR = 0x00000083;			//enable baud rate divisor loading and select the data format
	U0DLM  = 0x00; 
	U0DLL =  0x13;      		//select baud rate 9600 bps
	U0LCR = 0x00000003;
    U0IER  = 0x03;
	
	VICVectAddr0 = (unsigned long)uart_interrupt;//UART 1 INTERRUPT 
    VICVectCntl0 = 0x20|6;  

    VICIntEnable = 0x00000040;  

	//i2c interrupt	  
    I2C1CONSET = 0x40;		   //enable I2C
    I2C1SCLH = 0x0018;  	   //100KHZ IS SELECTED
    I2C1SCLL = 0x0012;
    VICVectAddr1 = (unsigned long)i2c_interrupt;//
    VICVectCntl1 = 0x20|19;		//ENABLE I2C INTERRUPT 
    VICIntEnable = 0x00080000; 
}

void port_init()
{
	PCONP |= (1 << 19); 	//I2C 1 is powered
	PINSEL0 |= 0x30C00005;	//P0.14 and P0.11 are set as SDA & SCL
}


void rtc_write()
{
	unsigned char disp_arr[100] = "RTC data written";  
	unsigned char *ptr;
	
	ptr = disp_arr;

	read_over= 0;
	f1 = 0x00;
	f2 = 0x00;
	count1 = 0x00;
	
	U0THR = CA; 
	while(ser_int_flg == 0);
	ser_int_flg = 0;

	while(*ptr != '\0')
	{
		U0THR = *ptr++; 
		while(ser_int_flg == 0);
		ser_int_flg = 0;
	}

	U0THR = CA; 
	while(ser_int_flg == 0);
	ser_int_flg = 0;
	
	l = 0;
	
	write_flag = 0xff;
	rtc_flag = 0xff;

	I2C1CONSET = 0X40; //Enable the I2C 
	I2C1CONCLR = 0x08; //Clear the SI bit 
	I2C1CONSET = 0x20; //Set the Start bit		

	U0THR  = CA; 
	while(ser_int_flg == 0);
	ser_int_flg = 0;

	while(ser_rec_int_flg == 0);
		ser_rec_int_flg = 0;
		
	I2C1CONCLR = 0x40;
}


void rtc_read()
{
	unsigned char disp_arr[100] = "RTC DATA IS";  
    unsigned char *ptr , n ;
   	ptr = disp_arr;
		
	U0THR = CA; 
	while(ser_int_flg == 0);
	ser_int_flg = 0;

	while(*ptr!='\0')
	{
		U0THR = *ptr++; 
		while(ser_int_flg == 0);
		ser_int_flg = 0;
	}
	
	U0THR = CA; 
	while(ser_int_flg == 0);
	ser_int_flg = 0;
	
	l = 0;
	bcnt = 0;

	flag = WR;

    read_flag = 0xff;
	rtc_flag = 0xff;
	read_over= 0;

	f1 = 0x00;
	f2 = 0x00;
	count1 = 0x00;
	 
    VICIntEnable = 0x00000200;
	I2C1CONSET = 0X40;
    I2C1CONCLR = 0x08;
	
	while(1)
	{
		l = 0;
		count1 = 0x00;
		flag = WR;

		I2C1CONSET = 0X20;

		while(f1 != 0xff);
		f1 = 0x00;
		flag = RD;
		I2C1CONSET = 0X20;

		while( read_over ==0);
		read_over =0;
		
		U0THR = 0x0d;	
		while(ser_int_flg == 0);
		ser_int_flg = 0; 
		
		for(i=0;i<7;i++)
		{
			U0THR= CA;
			while(ser_int_flg == 0);
			ser_int_flg = 0;
			n = 0;
			
			while(*(rtc_disp[i] + n) != '\0')
			{
				U0THR = *(rtc_disp[i]+n);
				
				while(ser_int_flg == 0);
				ser_int_flg = 0;
				n++;
			}
			var1 = rxd_data[i];
		    var1 = (var1 & 0xf0) >> 4;
		    var2 = hex_to_ascii(var1);
		     
			U0THR = var2;
			 
		    while(ser_int_flg == 0);
		    ser_int_flg = 0;
		 
		    var1 = rxd_data[i];
		    var1 = (var1 & 0x0f);
		    var2 = hex_to_ascii(var1);
		    U0THR = var2;
	
		    while(ser_int_flg == 0);
		    ser_int_flg = 0;

			U0THR = 0x20;				// now o/p a space between each value
            while(ser_int_flg == 0);
            ser_int_flg = 0;

		 }// end of for loop

		 while( ser_rec_int_flg  == 0);
		
		 ser_rec_int_flg =0;
         if(rxdata == 0x1B)
		 {
		 	break;
		 }

	}// end of while(1)
	
	U0THR= CA; 
	while(ser_int_flg == 0);
	ser_int_flg = 0; 
}

unsigned char hex_to_ascii( unsigned char var1)
{
	unsigned char var2;
	if((var1 <= 0x09 ) )   // ( var1 >= 0x00 ) && 
		var2 = var1 + 0x30;

   else var2 = var1 + 0x37;

   return(var2);

}


void i2c_interrupt(void)__irq 
{
	status = I2C1STAT;
	
	switch(status)
	{
	
		case SMB_START:
			if(read_flag == 0xff)
			{
				if(flag == WR)
				{
					temp = RTC_ADD | WRITE;
					I2C1DAT = temp;
					I2C1CONCLR = 0X20;
				}
		
		 		if(flag == RD)
				{
					temp = RTC_ADD | READ;
					I2C1DAT = temp;
					I2C1CONCLR = 0X20;
				}
             }

             else if(write_flag == 0xff)
			 {
			 	temp = RTC_ADD | WRITE;
					
                I2C1DAT = temp;
                I2C1CONCLR = 0X20;
			 }
             break;

		case SMB_RPT_START:
			if(read_flag == 0xff)
            {
				if(flag == WR)
				{
					temp = RTC_ADD | WRITE;
                    I2C1DAT = temp;
					I2C1CONCLR = 0x20;
                }

				else if(flag == RD)
				{
					temp = RTC_ADD | READ;
                    I2C1DAT = temp;
                    I2C1CONCLR = 0X20;
				}
			}

            else if(write_flag == 0xff)
            {
				temp = RTC_ADD | WRITE;
				I2C1DAT = temp;
				I2C1CONCLR = 0x20;
			}
							
			break;

		case SMB_MTADDACK:
			if(read_flag == 0xff)
        	{
				I2C1DAT = 0x00; 							
            }

			else if(write_flag == 0xff)
			{
				I2C1DAT = 0x00;
                bcnt = bcnt + 1;
            }

			break;

		case SMB_MTDBACK:
			if(read_flag == 0xff)
			{
				I2C1CONSET = 0X10;
                f1 = 0xff;
                flag = RD;
			}
							
			else if(write_flag == 0xff)
			{
				if(bcnt < 8 )
				{
                	if(rtc_flag == 0xff)
                		I2C1DAT = rtc_data[l++];
               		bcnt++;	
				}
                                  
				else if(bcnt >= 8)
                {
					I2C1CONSET = 0X10;
				}
			}
			break;

		case SMB_MTARBLOST://I2CONSET = 0x10;
                           I2C1CONSET = 0x20;
                            break;

		case SMB_MTDBNACK://I2CONSET = 0x10;
							I2C1CONSET = 0x20;
							break;

		case SMB_MTADDNACK:	
			if(read_flag == 0xff)
            {
				//I2CONSET = 0x10;
                I2C1CONSET = 0x20;
                flag = WR;
            }
        	else if(write_flag == 0xff)
			{
				//I2CONSET = 0x10;
                I2C1CONSET = 0x20;
            }
			break;

		case SMB_MRADDACK: I2C1CONSET = 0x04;	// ensuring that AA bit is set to send ACK to slave
		                   break;

		case SMB_MRADDNACK: //I2CONSET = 0x10;
                            I2C1CONSET = 0x20;
                            break;

		case SMB_MRDBACK:
			if(rtc_flag == 0xff)
			{
				if(l<=4)
				{
				    rxd_data[l++] = I2C1DAT;
				    count1 = count1 + 1;
			    }
				else
				{
				   rxd_data[l++] = I2C1DAT;
				   count1 = count1 + 1;
				   I2C1CONCLR = 0x04;	   // clearing the AA bit to ensure NACK is sent for the last byte
				}							// received
				       
			}
			     break;
		case SMB_MRDBNACK:		   // comes here when last byte is received
			if(rtc_flag == 0xff)
			{
				rxd_data[l++] = I2C1DAT;
				count1 = count1 + 1;
				I2C1CONSET = 0X10;	  // setting the STOP condition
               read_over = 0xff;
			}
			break;
	    
	}
	I2C1CONCLR = 0x08;	// clearing  the I2C intr flag
    VICVectAddr=0; 			
} 

void uart_interrupt(void)__irq
{
	temp = U0IIR ;
	if((temp& 0x02) == 0x02)
	{  
		temp = U0IIR; 
		tx_flag = 0xff;
		ser_int_flg = 0xff;
        VICVectAddr=0; 
	}

	else if( (temp & 0x04) == 0x04)
	{ 
        rxdata = U0RBR;
		rx_flag = 0xff;
        ser_rec_int_flg = 0xff;
        VICVectAddr=0; 
	}      
}

