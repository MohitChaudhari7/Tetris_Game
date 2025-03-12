/*
 * UART.h
 *
 *  Created on: Dec 4, 2023
 *      Author: vishn
 */


// 			Established USART communication at 19.2KHz and using pins PB6, PB7
// 			for Tx and Rx respectively.

#ifndef INC_UART_H_
#define INC_UART_H_

#include "stm32f4xx.h"

void Sys_clk_Setup(void);

void Init_UART();

int getchar();

int putchar(int a);

int putnum(int a);

int cputs(char* ptr);

#endif /* INC_UART_H_ */
