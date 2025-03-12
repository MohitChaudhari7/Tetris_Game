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
 * @file    Pushbutton.c
 * @brief   Interfaces the onboard pushbutton with debouncing
 * @author  Vishnu Kumar Thoodur Venkatachalapathy and Mohit Chaudhari
 * @date    Dec 4, 2023
 */
#include "Pushbutton.h"

#define BUTTON_PIN 0

/**
 * @brief	Setup the On-board pushbutton as an input
 *
 * @return	none
 */
void Init_Push_Buton(){

	// Send clock signal to port A
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;

	// setting the button pin as input and NOT pulling the pin up/down
	GPIOA->MODER  &= ~(0x3 << (BUTTON_PIN*2));
	GPIOA->PUPDR  &= ~(0x3 << (BUTTON_PIN*2));
}

/**
 * @brief	Setup the On-board pushbutton to an external interrupt service routine
 *
 * @return	none
 */
void Init_Push_Button_ITR(){

	RCC->APB2ENR |= (1 << 14); //clk enable

	// Setting External Interrupt 0 to port A
	SYSCFG->EXTICR[0] &= ~((uint32_t)0x0000000F);

	// Configuring an external interrupt to the pin connected to the on-board
	// push-button
	EXTI->IMR |= (1 << BUTTON_PIN);

	// Configuring the interrupt to trigger in rising edge
	EXTI->RTSR |= (1 << BUTTON_PIN);

	// Enabling the External interrupt in NVIC so that ISR commands can be
	// written
	NVIC_EnableIRQ(EXTI0_IRQn);

	// setting 1st priority to the external interrupt
	NVIC_SetPriority(EXTI0_IRQn,0);
}
