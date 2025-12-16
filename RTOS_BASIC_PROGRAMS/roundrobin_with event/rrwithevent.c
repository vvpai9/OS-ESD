			 #include <rtl.h>
#include <lpc21xx.h>
#include <stdio.h>
void init_serial (void);
OS_TID tsk1,tsk2;
char    cnt1,cnt2,cnt3;

__task void job1 (void);
__task void job2 (void);
__task void job3 (void);
char arr1[20],arr2[20];
unsigned int i = 0;
  
void delay()
{
  unsigned int j;
  for(j=0;j<10000;j++);
}
__task void job1 (void)
 {
  os_tsk_prio_self (2);
   os_tsk_create (job2, 1);
 
  while (1)
   {   os_evt_wait_or (0x0003, 0x0020);
	   cnt1++;
	   sprintf(arr1,"counter1 :%d",cnt1);
  	while (arr1[i] != '\0')
	{
	  	while (!(U0LSR & 0x20));
	  	U0THR = arr1[i];
		i++;
	}
	i=0;
	while (!(U0LSR & 0x20));
	U0THR = '\n'; 
	if (cnt1==0x05)
	cnt1=0;
	os_dly_wait(1);
	 				
    }
}

__task void job2 (void)
 {
   
   os_tsk_create (job3, 0);
  while (1)
   {	 os_evt_wait_or (0x0002, 0x0010);
          cnt2++;
       	//os_dly_wait(3);  
	if(cnt2==0x05)
        {
              cnt2=0;
			    IOSET0=0X00FF0000;
			   os_evt_set (0x0001, tsk1);
			   delay();
			   delay();	
			 IOCLR0=0X00FF0000;
             }
     //  os_dly_wait(3);

		 IOCLR0=0X00FF0000;
		  
  }
}

__task void job3 (void)
 {

   
  while (1)
   {	 
       cnt3++;
       //	os_dly_wait(3);  
	if(cnt3==0x05)
        {
              cnt3=0;
			  IOSET0=0X00000200;
			  	delay();
			   delay();	
             os_evt_set (0x0002, tsk2);
			  IOCLR0=0X00000200;	

             }
     //  os_dly_wait(3);

		 IOCLR0=0X00000200;
  }
}

int main (void) 
{
cnt1=0;
cnt2=0;
 IODIR0=0X00FF0200;
   init_serial ();
  os_sys_init (job1);
  while (1);
}

	  /*----------------------------------------------------------------------------
 *       init_serial:  Initialize Serial Interface
 *---------------------------------------------------------------------------*/
void init_serial (void) {
  PINSEL0 = 0X0000005;                 // Enable RxD0 and TxD0              
  U0LCR = 0x83;                         // 8 bits, no Parity, 1 Stop bit     
  U0DLL = 0x61;                           // 9600 Baud Rate @ 15MHz VPB Clock  
  U0LCR = 0x03;                         // DLAB = 0                          
}