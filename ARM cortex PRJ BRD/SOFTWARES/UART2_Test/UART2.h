#include <LPC17xx.h>

#ifndef __UART0_H
#define __UART0_H

void UART2_Init(void);
void UART2_IRQHandler(void);

extern unsigned char recv_buf[50], recv_data, recv_index;
extern unsigned char rx2_flag, tx2_flag;

#endif
