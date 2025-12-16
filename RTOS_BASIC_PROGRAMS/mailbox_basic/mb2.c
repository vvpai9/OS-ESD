#include <rtl.h>
#include<stdio.h>
#include<lpc214x.h>
OS_TID t1;
os_mbx_declare (MsgBox, 100);                /* Declare an RTX mailbox */
U32 mpool[8*sizeof(U32) ];        /* Reserve a memory for 16 messages */
unsigned int rec0,rec1 ;
unsigned int cnt1,cnt2;

__task void task2 (void);

__task void task1 (void)
 {
  /* This task will send a message. */
  U32 *mptr;
   	t1=os_tsk_self();
  os_tsk_create (task2, 0);
  os_mbx_init (MsgBox, sizeof(MsgBox));
  mptr = _alloc_box (mpool);                /* Allocate a memory for the message */
 while(1)
 {	 while(!(cnt1==15)){ cnt1++; }
  os_dly_wait(5);
   	 if (cnt1==15)
  mptr[0] = cnt1; 
   os_mbx_send (MsgBox, mptr, 0xffff);
     cnt1=0;

	 							      /* Send a message to a 'MsgBox' */
 }

     /* Set the message content. */
 
   }
__task void task2 (void) 
{
  /* This task will receive a message. */
  U32 *rptr  ;
  cnt2=0;
  	   os_mbx_wait (MsgBox, (void**)&rptr, 0xffff);      /* Wait for the message to arrive. */
  cnt2 = *rptr;
 	 while(1)
	 {
						                 /* Store the content to 'rec_val' */
  cnt2++;
  os_dly_wait(1);	
 if (cnt2==30)
 {
 cnt2=0;
 os_dly_wait(5);
	 
  }
 }
}

void main (void) 
{
  _init_box (mpool, sizeof(mpool), sizeof(U32));
  os_sys_init_prio(task1,10);
}
