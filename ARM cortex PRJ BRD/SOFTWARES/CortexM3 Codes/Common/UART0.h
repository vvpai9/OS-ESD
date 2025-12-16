#include <LPC17xx.h>

#ifndef __UART0_H
#define __UART0_H

void UART0_Init(void);
void UART0_IRQHandler(void);

extern unsigned char recv_buf[50], recv_data, recv_index;
extern unsigned char rx0_flag, tx0_flag;

#endif
