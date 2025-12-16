#include <rtl.h>
#include<stdio.h>
#include<lpc214x.h>
void init_serial(void);
void data(unsigned char p);
void lcd_init (void);
void cmd(unsigned int x);
void delay (unsigned int);
os_mbx_declare (MsgBox, 100);                /* Declare an RTX mailbox */
U32 mpool[8*sizeof(U32) ];        /* Reserve a memory for 16 messages */
char cnt1,cnt2;
int i=0;


__task void task2 (void);

void delay(unsigned int count)
{
	while(count--);
}

__task void task1 (void)
 {
  /* This task will send a count value. */
  U32 *mptr;
  os_tsk_create (task2, 0);
  os_mbx_init (MsgBox, sizeof(MsgBox));
  mptr = _alloc_box (mpool);                /* Allocate a memory for the message */
 while(1)
 {
 init_serial();
 while(!(U0LSR & 0x01));
 cnt1=U0RBR;
 while (!(U0LSR & 0x20));
 U0THR = cnt1;
 mptr[0] = cnt1; 
 os_mbx_send (MsgBox, mptr, 0xffff);	
 os_dly_wait(5);
 }
 }

__task void task2 (void) 
{
  /* This task will receive a count. */
  U32 *rptr  ;
  
 	 while(1)
	 { 
	 os_mbx_wait (MsgBox, (void**)&rptr, 0xffff);      /* Wait for the message to arrive. */
     cnt2 = *rptr;	    /* copy the count value from task1 to cnt2  */
	 data(cnt2);
	
	  		

	  }
 	 }

int main (void) 
{   IODIR0 = 0xf0ff00fc;
	lcd_init();
    init_serial();
    _init_box (mpool, sizeof(mpool), sizeof(U32));
    os_sys_init(task1);
}
								   /*----------------------------------------------------------------------------
 *       init_serial:  Initialize Serial Interface
 *---------------------------------------------------------------------------*/
void init_serial (void) 
{
  PINSEL0 = 0X0000005;                 // Enable RxD0 and TxD0              
  U0LCR = 0x83;                         // 8 bits, no Parity, 1 Stop bit     
  U0DLL = 0x61;                           // 9600 Baud Rate @ 15MHz VPB Clock  
  U0LCR = 0x03;                         // DLAB = 0                          
}


void cmd(unsigned int x)
{
  unsigned int value;
  value = x;
  value = value & 0xF0;
  IOCLR0 = 0xF0;
  IOCLR0 = 0x04; 
  IOSET0 = value;
  IOSET0 = 0x08; 
  delay(100);
  IOCLR0 = 0x08; 
  value = x;
  value = value & 0x0F;
  value = value << 4;
  IOCLR0 = 0xF0;
  IOCLR0 = 0x04; 
  IOSET0 = value;
  IOSET0 = 0x08;
  delay(100);
  IOCLR0 = 0x08; 
}

void data(unsigned char p)
{		
  unsigned int value;
  
  value = p;
  value = value & 0xF0;
  IOCLR0 = 0xF0;
  IOSET0 = 0x04; 
  IOSET0 = value;
  IOSET0 = 0x08; 
  delay(100);
  IOCLR0 = 0x08; 
  value = p;
  value = value & 0x0F;
  value = value << 4;
  IOCLR0 = 0xF0;
  IOSET0 = 0x04; 
  IOSET0 = value;
  IOSET0 = 0x08; 
  delay(100);
  IOCLR0 = 0x08; 
}

void lcd_init ()		 
{
  unsigned int c[] = {0x30,0x30,0x30,0x20,0x28,0x01,0x06,0x0E,0x80};
  unsigned int i;
  for(i=0;i<7;i++)
  { 
    cmd(c[i]);
    delay(10000);
  }
} 

