
/*

 Round-Robin Scheduling
RTX can be configured to use Round-Robin Multitasking (or task switching). Round-Robin allows quasi-parallel execution of several tasks. Tasks are not really executed concurrently but are time-sliced (the available CPU time is divided into time slices and RTX assigns a time slice to each task). Since the time slice is short (only a few milliseconds) it appears as though tasks execute simultaneously.

Tasks execute for the duration of their time-slice (unless the task's time slice is given up). Then, RTX switches to the next task that is ready to run and has the same priority. If no other task with the same priority is ready to run, the currently running task resumes it execution. The duration of a time slice can be defined in the RTX_config.c configuration file.

The following example shows a simple RTX program that uses Round-Robin Multitasking. The two tasks in this program are counter loops. RTX starts executing task 1, which is the function named job1. This function creates another task called job2. After job1 executes for its time slice, RTX switches to job2. After job2 executes for its time slice, RTX switches back to job1. This process repeats indefinitely.

*/



#include <rtl.h>
#include <lpc21xx.h>

int counter1;
int counter2;
int counter3;
__task void job1 (void);
__task void job2 (void);
__task void job3 (void);

__task void job1 (void) {
  os_tsk_create (job2, 0);   /* Create task 2 and mark it as ready */
  while (1)
   {                /* loop forever */
    counter1++;  
	            /* update the counter */
  }
}

__task void job2 (void)
 {
os_tsk_create (job3, 0); 			/* Create task 2 and mark it as ready */
  while (1) 
  {               		 /* loop forever */
    counter2++;              /* update the counter */
   
  }
}


__task void job3 (void)
 {
  while (1)
   {                /* loop forever */
    counter3++;              /* update the counter */
  }
}
int  main (void)
 {							  

  os_sys_init (job1);        /* Initialize RTX Kernel and start task 1 */
  for (;;);
}



	 