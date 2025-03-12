/*******************************************************************************
 * Copyright (C) 2023 by Vishnu Kumar Thoodur Venkatachalapathy and Mohit Chaudhari
 *
 * Redistribution, modification or use of this software in source or binary
 * forms is permitted as long as the files maintain this copyright. Users are
 * permitted to modify this and use it to learn about the field of embedded
 * software. Vishnu Kumar Thoodur Venkatachalapathy and the University of
 * Colorado are not liable for any misuse of this material.
 * ****************************************************************************/

/**
 * @file    JoyStick.c
 * @brief   Makes use of the onboard ADC to get joystick values
 * 				PC1 -> connect to ADC -> joystick VRy -> ADC1_11
 *				PC2 -> connect to ADC -> joystick VRx -> ADC1_12
 * @author  Vishnu Kumar Thoodur Venkatachalapathy and Mohit Chaudhari
 * @date    Dec 4, 2023
 */

#include "JoyStick.h"
#include "UART.h"

// Global variable to store the Joystick value in X and Y axis
uint16_t Joy_X = 0, Joy_Y = 0;


/**
 * @brief	Starts the ADC convertion to get the current Joysstick value
 * @return	None
 */
void Get_JoyStick_Val(){
	// Start conversion:
	ADC1->CR2 |= ADC_CR2_SWSTART;

	/*
	 * Wait till the conversion is over and save the conversion data for Y-Axis
	 * into a variable
	 */
	while((ADC1->SR & ADC_SR_EOC) == 0);
	Joy_Y = ADC1->DR;


	/*
	 * Wait till the conversion is over and save the conversion data for X-Axis
	 * into a variable
	 */
	while((ADC1->SR & ADC_SR_EOC) == 0);
	Joy_X = ADC1->DR;
}

/**
 * @brief	Initializes the ADC1 and Port C to get input from the Joystick on
 * 			pins PC1 for Y-Axis data and PC2 for X-Axis data
 * @return	None
 */
void Init_JoyStick(){
	// Send clock signal to enable GPIO port C and ADC
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;
	RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;

	// Setting PC1 and PC2 to analog mode
	GPIOC->MODER &= ~GPIO_MODER_MODER1_Msk;
	GPIOC->MODER |= GPIO_MODER_MODER1_0 | GPIO_MODER_MODER1_1;

	GPIOC->MODER &= ~GPIO_MODER_MODER2_Msk;
	GPIOC->MODER |= GPIO_MODER_MODER2_0 | GPIO_MODER_MODER2_1;

	// Turning ADC OFF
	ADC1->CR2 = 0;

	// Setting total number of regular conversions as 2
	ADC1->SQR1 &= ~ADC_SQR1_L_Msk;
	ADC1->SQR1 |= ADC_SQR1_L_0;

	// Setting conversion sequence as PC1, PC2
	ADC1->SQR3 &= ~ADC_SQR3_SQ1_Msk;
	ADC1->SQR3 |= 0x0B << ADC_SQR3_SQ1_Pos;

	ADC1->SQR3 &= ~ADC_SQR3_SQ2_Msk;
	ADC1->SQR3 |= 0x0C << ADC_SQR3_SQ2_Pos;

	// Using SCAN mode by setting the SCAN bit in the CR1 reg.
	ADC1->CR2 &= ~ADC_CR2_CONT;

	// Doing continous conversion by setting CONT bit to 0 in CR2
	ADC1->CR1 |= ADC_CR1_SCAN;

	// Setting ADC to generate a EOC bit after the end of conversion of
	// each channel in the reqular sequence
	ADC1->CR2 |= ADC_CR2_EOCS;

	// Power-on the ADC
	ADC1->CR2 |= ADC_CR2_ADON;
}

/**
 * @brief	Gets the latest Joystick value and prints it out on the USART port
 * @return	None
 */
void Print_JoyStick_Val(){
	Get_JoyStick_Val();
	cputs("\r\nX: ");
	putnum(Joy_X);
	cputs("\tY: ");
	putnum(Joy_Y);
}

