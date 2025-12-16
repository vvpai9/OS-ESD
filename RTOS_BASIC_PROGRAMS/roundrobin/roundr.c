#include <rtl.h>

int counter1;
int counter2;

__task void job1 (void);
__task void job2 (void);

__task void job1 (void) {
  os_tsk_create (job2, 0);   /* Create task 2 and mark it as ready */
  while (1) {                /* loop forever */
    counter1++;              /* update the counter */
  }
}

__task void job2 (void) {
  while (1) {                /* loop forever */
    counter2++;              /* update the counter */
  }
}

void main (void) {
  os_sys_init (job1);        /* Initialize RTX Kernel and start task 1 */
  for (;;);
}
