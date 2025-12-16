#include <lpc21xx.h>
#ifndef _LCDH
#define  _LCDH

void lcd_init(void);
void clr_disp(void);
void delay_lcd(unsigned int);
void lcd_com(void);						   
void lcd_data(void);
void lcd_puts(unsigned char *);

extern unsigned long int tmp1 , tmp2 ;

#endif	
