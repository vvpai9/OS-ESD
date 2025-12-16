#include <rtl.h>
#include <lpc21xx.h>
#include <stdio.h>
OS_TID tsk1,tsk2;    	//declare task identification number variables
int    cnt1,cnt2, i;	// counter
__task void job1 (void); 		//declare function for job1
__task void job2 (void); 		//declare function for job2

__task void job1 (void)		// job1 function definition
 {
  os_tsk_prio_self (2);		// assign priority to job1 as 2
  
   while (1)
   {
    
    cnt1++;						  //  increment counter1
   	cnt1++;	
	 tsk2=os_tsk_create (job2, 3);		//create job2 and keep in ready state
	}
 
    }


__task void job2 (void)		// job2 function definition
 {
  while (1)
   {
          cnt2++;			//increment counter 2
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

