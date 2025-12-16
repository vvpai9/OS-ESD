

#include <RTL.h>                      /* RTX kernel functions & defines      */
#include <lpc21xx.h>
OS_TID tsk1;
OS_TID tsk2;

U32 i;                          
__task void job1 (void);
__task void job2 (void);
void __swi(9)  ONLED (void);
void __swi(8)  OFFLED (void);

void __SWI_8          (void) 
{
IO0DIR=0X00FF0000;
IOSET0=0X00FF0000;					//TURN OFF LEDS
for (i=0;i<1000000;i++);
 
}



void __SWI_9         (void)
 {	 
 
 IO0DIR=0X00FF0000;
	 for (i=0;i<1000000;i++);

	IOCLR0=0X00FF0000;				   //TURN ON LEDS
	 for (i=0;i<1000000;i++); 			   //delay 
	// for (i=0;i<1000000;i++);
 
}


/*----------------------------------------------------------------------------
 *   Task 1
 *---------------------------------------------------------------------------*/
__task void job1 (void)
 {
  //tsk1 = os_tsk_self ();
   os_tsk_create (job2,0);      /* start task 2                        */

  while (1)
   {
    ONLED();              /* TURN ON LEDS           */
  }
}

/*----------------------------------------------------------------------------
 *   Task 2
 *---------------------------------------------------------------------------*/
__task void job2 (void) {
  while (1)
   {
    OFFLED ();           /*TURN OFF LEDS         */
   // os_dly_wait (1);
  }
}

/*----------------------------------------------------------------------------
 *   Initialize and start RTX Kernel
 *---------------------------------------------------------------------------*/
int main (void)
 {
  os_sys_init (job1);
}

/*----------------------------------------------------------------------------
 * end of file
 *---------------------------------------------------------------------------*/


