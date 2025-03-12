/*
 * UART.c
 *
 *  Created on: Dec 4, 2023
 *      Author: vishn
 */

// 			Established USART communication at 19.2KHz and using pins PB6, PB7
// 			for Tx and Rx respectively.

#include "UART.h"

#define GPIO_MODER_MODER6_SET	(3<12)
#define GPIO_MODER_MODER7_SET	(3<14)

/**
 * @brief	Setup the HSI clock and use it for sys clock instead of the HSE Clock
 *
 * @return	none
 */
void Sys_clk_Setup(void){
  /* Checking if the HSI clock is enabled or not and enabling if it is not setup*/
  if( (RCC->CR & RCC_CR_HSIRDY) == 0)
  {
    RCC->CR |= RCC_CR_HSION;  // Turning HSI clock on
    while( (RCC->CR & RCC_CR_HSIRDY) == 0); //Waiting until HSI clock is ready
  }

  /* Select AHB prescaler to 1 (clock not divided)*/
  RCC->CFGR |= RCC_CFGR_HPRE_DIV1;

  /* Setting APB high-speed and Low-speed prescaler to 1 (clock not divided) */
  RCC->CFGR |= RCC_CFGR_PPRE1_DIV1;
  RCC->CFGR |= RCC_CFGR_PPRE2_DIV1;

  /* Select the HSI as system clock source */
  RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_SW));
  RCC->CFGR |= RCC_CFGR_SW_HSI;

  /* Disabling HSE Clock*/
  RCC->CR &= ~RCC_CR_HSEON;
}

/**
 * @brief	Initializing USART1 at 19.2KHz and using pins PB6 and PB7 for Tx
 * 			and Rx respectively
 *
 * @return	none
 */
void Init_UART(){
	// Send clock signal to port B
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
	RCC->APB2ENR |= RCC_APB2ENR_USART1EN;

	// Set pin 6 & 7 to Alternate Function mode
	GPIOB->MODER &= ~GPIO_MODER_MODER6_SET;
	GPIOB->MODER |= GPIO_MODER_MODER6_1;

	GPIOB->MODER &= ~GPIO_MODER_MODER7_SET;
	GPIOB->MODER |= GPIO_MODER_MODER7_1;

	// Select alternate function 7
	// i.e. pin B6 is connected to USART1_TX
	// i.e. pin B7 is connected to USART1_RX
	GPIOB->AFR[0] |= GPIO_AFRL_AFSEL6_0 | GPIO_AFRL_AFSEL6_1 | GPIO_AFRL_AFSEL6_2;
	GPIOB->AFR[0] |= GPIO_AFRL_AFSEL7_0 | GPIO_AFRL_AFSEL7_1 | GPIO_AFRL_AFSEL7_2;

	// Enable USART1
	USART1->CR1 |= USART_CR1_UE;

	// Set word length to 1 start bit and 8 data bits by writing 0 to the register
	USART1->CR1 &= ~USART_CR1_M;

	// Set stop bit to 1 by writing 00 to the register
	USART1->CR2 &= ~USART_CR2_STOP;

	// Set Baud rate to 19200 by setting
	//  DIV_Fraction to 0xA and
	//  DIV_Mantissa to 0x34
	USART1->BRR |= (0xA << USART_BRR_DIV_Fraction_Pos);
	USART1->BRR |= (0x34 << USART_BRR_DIV_Mantissa_Pos);

	// Enable RX
	USART1->CR1 |= USART_CR1_RE;

	// Enable interrupt for RX
	USART1->CR1 |= USART_CR1_RXNEIE;

	// Set the TI bit to send an empty frame as 1st transmission
	USART1->CR1 |= USART_CR1_TE;
}

/**
 * @brief	Returning the data received on the USART port
 *
 * @return	I/P character
 */
int getchar(){
	while(!(USART1->SR & USART_SR_RXNE));
	return USART1->DR;
}

/**
 * @brief		Transmitting the given character on the USART port
 * @param	a	Character/data to be written to the USART port
 * @return		1
 */
int putchar(int a){
	while(!(USART1->SR & USART_SR_TC));
	USART1->DR = a;
	return 1;
}

/**
 * @brief		Transmitting the given number on the USART port
 * @param	a	integer to be written to the USART port (max 4 digits)
 * @return		1
 */
int putnum(int a){
	char str[5];
	int i=0;

	// Directly writing 0 to the USART port if a is 0 and returning
	if(a==0){
		putchar('0');
		return(1);
	}

	// extracting the 1's place digit in a, converting it to char and storing
	// it in the str string till a is 0
	while (a != 0) {
		int rem = a % 10;
		str[i++] = rem + '0';
		a = a / 10;
	}

	// inverting the string since the previous operation stores the number in
	// reverse order
	int start = 0;
	int end = i - 1;
	while (start < end) {
		char temp = str[start];
		str[start] = str[end];
		str[end] = temp;
		end--;
		start++;
	}

	// Appending '\0' to the string and printing out the number
	str[i] = '\0';
	i=0;
	while(str[i] != '\0'){
		putchar(str[i++]);
	}
	return(1);
}

/**
 * @brief		Transmitting the given string to the USART port
 * @param	ptr	string to be written to the UART port
 * @return		1
 */
int cputs(char* ptr){
	int i=0;
	while(ptr[i] != '\0'){
		putchar(ptr[i++]);
	}
	return(1);
}


