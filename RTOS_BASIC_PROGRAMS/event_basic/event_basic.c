			#include <rtl.h>
#include <lpc21xx.h>
#include <stdio.h>
OS_TID tsk1,tsk2;    	//declare task identification number variables
OS_RESULT RE1,RE2;

int    cnt1,cnt2,i,cnt3;	// counter
__task void job1 (void); 		//declare function for job1
__task void job2 (void); 		//declare function for job2



__task void job1 (void)		// job1 function definition
 {
  os_tsk_prio_self (2);		// assign priority to job1 as 2
  tsk1=os_tsk_self(); 	   // task id 
  os_tsk_create (job2, 1);		//create job2 and keep in ready state
   while (1)
   { 
     
   	 RE1=os_evt_wait_and (0x0001, 0x0001);		// wait for event 
	 if (RE1== OS_R_EVT)
	 {
	for(i=0;i<15;i++)
   {
	 cnt1++;
		 for(i=0;i<65000;i++);
		  for(i=0;i<65000;i++);
		 for(i=0;i<65000;i++);
		  for(i=0;i<65000;i++);
	 }	
	 	os_dly_wait(1);			
    }
	
	 else if (RE1== OS_R_TMO)
	 {
	for(i=0;i<15;i++)
   {
	 cnt3++;
		 for(i=0;i<65000;i++);
		  for(i=0;i<65000;i++);
		 for(i=0;i<65000;i++);
		  for(i=0;i<65000;i++);
	 }	
	 	//os_dly_wait(1);			
    }
	}
}

__task void job2 (void)		// job2 function definition
 {
  while (1)
   {	   for(i=0;i<15;i++)
   {
          cnt2++;			//increment counter 2
		 for(i=0;i<65000;i++);
		  for(i=0;i<65000;i++);
		 for(i=0;i<65000;i++);
		  for(i=0;i<65000;i++);
    	}		//clear counter	and set event flag to start job1
		//os_dly_wait(1);
os_evt_set (0x0001, tsk1);	
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
