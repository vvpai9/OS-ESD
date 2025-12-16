#include <RTL.h>                      /* RTX kernel functions & defines      */
#include <LPC21xx.h>                  /* LPC21xx definitions                 */
#include <stdio.h>
#include "defn.h"
int main (void) {
  IODIR0 = 0xf0ff00fc;
	lcd_init();
  /* Initialize RTX and start init   */
  os_sys_init (init);
}


