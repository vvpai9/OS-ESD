void data(unsigned char p);
void lcd_init (void);
void cmd(unsigned int x);
    
unsigned char msg1[]="task1\r\n", msg2[]="task2\r\n";
unsigned int i=0;
OS_TID tsk1, tsk2;
OS_SEM semaphore1;
/*----------------------------------------------------------------------------
 *    Task 1 - High Priority - Active every 3 ticks
 *---------------------------------------------------------------------------*/
void delay(unsigned int count)
{
	while(count--);
}


__task void task1 (void) {
  OS_RESULT ret;
  char str[]  = {'H','E','L','L','O'};
	int  j = -1;	

  while (1) {
    /* Pass control to other tasks for 3 OS ticks */
    os_dly_wait(3);
    /* Wait 1 ticks for the free semaphore */
    ret = os_sem_wait (semaphore1, 1);
    if (ret != OS_R_TMO) {
      /* If there was no time-out the semaphore was aquired */
      //printf ("Task 1\n");
	  while(j<5)
	  {
	  data(str[j]);
	delay(1000000);delay(1000000);delay(1000000);
	 //i++;
	j++;
		}
		j = -1;
	    cmd(0x01);	  
      /* Return a token back to a semaphore */
      os_sem_send (semaphore1);
    }
  }
}
/*----------------------------------------------------------------------------
 *    Task 2 - Low Priority - looks for a free semaphore and uses the resource
 *                            whenever it is available
 *---------------------------------------------------------------------------*/
__task void task2 (void) {
   char str[]  = {'W','O','R','L','D'};
	int  j = -1;	
  while (1) 
  {
    /* Wait indefinetly for a free semaphore */
    os_sem_wait (semaphore1, 0xFFFF);
    /* OK, the serial interface is free now, use it. */
    //printf ("Task 2 \n");
     while(j<5){data(str[j]);
	delay(1000000);delay(1000000);delay(1000000);
	 //i++;
	j++;
	}
	j = -1;
	    cmd(0x01);
    /* Return a token back to a semaphore. */
    os_sem_send (semaphore1);
	
  }
}

/*----------------------------------------------------------------------------
 *    Task 3 'init' 
 *---------------------------------------------------------------------------*/
__task void init (void) {
  /* Initialize the UART */
  
  /* Initialize the Semaphore before the first use */
  os_sem_init (semaphore1, 1);
  /* Create an instance of task1 with priority 10 */
  tsk1 = os_tsk_create (task1, 10);
  /* Create an instance of task2 with default priority 1 */
  tsk2 = os_tsk_create (task2, 0);
  /* Delete the init task */  
  os_tsk_delete_self ();
}
/*----------------------------------------------------------------------------
 *       init_serial:  Initialize Serial Interface
 *---------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------
 *    Main: Initialize and start RTX Kernel
 *---------------------------------------------------------------------------*/

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
