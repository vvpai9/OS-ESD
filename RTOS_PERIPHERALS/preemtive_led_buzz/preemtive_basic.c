#include <rtl.h>
#include <lpc21xx.h>
#include <stdio.h>
OS_TID tsk1,tsk2;    	//declare task identification number variables
int    cnt1,cnt2, i;	// counter
__task void job1 (void); 		//declare function for job1
__task void job2 (void); 		//declare function for job2

__task void job1 (void)		// job1 function definition
 {	  
 IODIR0=0X00fff00;
  os_tsk_prio_self (1);		// assign priority to job1 as 2
   tsk2=os_tsk_create (job2, 0);		//create job2 and keep in ready state
   while (1)
   {
   for(i=0;i<100000;i++)
   { 
    IOSET0=0X00fff00;	
	}
	os_tsk_prio(tsk2,2);			// change priority of job 2 to switch 
    }
}

__task void job2 (void)		// job2 function definition
 {
 IODIR0=0X00FF0000;
  while (1)
   {
         
		 IOCLR0=0X00FF0000;
		  for(i=0;i<100000;i++); 
		  IOSET0=0X00FF0000;
		  for(i=0;i<100000;i++); 
     }
			// when job1 is done resume with job2
  	  
  }

int main (void) 
{
cnt1=0;
cnt2=0;

  os_sys_init (job1);		//initialize job1 
  while (1);
}
