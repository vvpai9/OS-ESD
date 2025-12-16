//Switching by priority
//The following example demonstrates one of the task switching mechanisms. Task job1 has a higher priority than task job2. When job1 starts, it creates task job2 and then enters the os_evt_wait_or function. The RTX kernel suspends job1 at this point, and job2 starts executing. As soon as job2 sets an event flag for job1, the RTX kernel suspends job2 and then resumes job1. Task job1 then increments counter cnt1 and calls the os_evt_wait_or function, which suspends it again. The kernel resumes job2, which increments counter cnt2 and sets an event flag for job1. This process of task switching continues indefinitely.


#include <rtl.h>
#include <lpc21xx.h>
#include <stdio.h>
OS_TID tsk1,tsk2,tsk3;    
OS_RESULT result1,result2,result3;

	//declare task identification number variables
int    cnt1,cnt2,cnt3,i;	// counter
__task void job1 (void); 		//declare function for job1
__task void job2 (void); 		//declare function for job2
__task void job3 (void); 		//declare function for job3

__task void job1 (void)		// job1 function definition
 {	
 tsk1=os_tsk_self();
  os_tsk_prio_self (1);		// assign priority to job1 as 2
   tsk2= os_tsk_create (job2, 0);		//create job2
tsk3=	os_tsk_create (job3, 0);		//create job2
   while (1)
   {
   
   	   os_evt_wait_or(0x0001,0x0010);
	   //cnt1=0;
 	  for (i=0; i<15; i++)
	  {
	   cnt1++;			// if event increment counter2
	   }
      os_dly_wait(10);
	os_evt_set (0x0002, tsk2);

   }
}

__task void job2 (void)		// job2 function definition
 {
  while (1)
   {
   	  os_evt_wait_or (0x0002, 0x0020);		// wait for event 
	  // cnt2=0;
	  for (i=0; i<15; i++)
	  {
	   cnt2++;			// if event increment counter2
	   }
     				//increment counter 2
 
	os_dly_wait(10);					   			//clear counter	and set event flag to start job1
    os_evt_set (0x0003, tsk3);	
   }
}


__task void job3 (void)		// job1 function definition
 {

   while (1)
   {
  os_evt_wait_or (0x0003, 0x0030);		// wait for event 
	//cnt3=0; 
for (i=0;i<15;i++)
{
	   cnt3++;			// if event increment counter1
	   }
	   
os_dly_wait(10);	 
		 			//clear counter	and set event flag to start job1
os_evt_set (0x0001, tsk1);	
 }
   }
int main (void) 
{
cnt1=0;
cnt2=0;
cnt3=0;

  os_sys_init (job1);		//initialize job1 
  while (1);
}

