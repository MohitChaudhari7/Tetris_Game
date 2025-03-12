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
 * @file    Timers.c
 * @brief   Implementation of various timer modules
 * @author  Vishnu Kumar Thoodur Venkatachalapathy and Mohit Chaudhari
 * @date    Dec 4, 2023
 */
#include "Timers.h"

int Timer4_flag=0, Timer3_flag=0, Timer2_flag=0;

/**
 * @brief	Initializing timer 4 to run for 20ms.
 * @return	none
 */
void Init_Timer4(void)
{
	// Send clock signal to Timer 3
	RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;

	// Setting prescaler to 159 to get 100KHz clock signal
	TIM4->PSC = 319;

	// 20ms = 50Hz = (100 KHz/50 Hz) =  2000
	TIM4->ARR = 2000;

  	// Enabling the Interrupt for timer 4 and clearing the interrupt status
    TIM4->DIER |= TIM_DIER_UIE;
    TIM4->SR &= ~TIM_SR_UIF;

    // Start Timer 4
    TIM4->CR1 = (1 << 0);
}

/**
 * @brief	Initializing timer 3 to run for 1s.
 * @return	none
 */
void Init_Timer3(void)
{
	// Send clock signal to Timer 3
	RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;

	// Setting prescaler to 159 to get 100KHz clock signal
	TIM3->PSC = 319;

	// 1s = 1Hz = (100 KHz/1 Hz) =  100000
	TIM3->ARR = 100000;

  	// Enabling the Interrupt for timer 3 and clearing the interrupt status
    TIM3->DIER |= TIM_DIER_UIE;
    TIM3->SR &= ~TIM_SR_UIF;
	NVIC_EnableIRQ(TIM3_IRQn);

    TIM3->CR1 = (1 << 0);
}

/**
 * @brief	Initializing timer 2 to run for 200ms.
 * @return	none
 */
void Init_Timer2(void)
{
	// Send clock signal to Timer 2
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

	// Setting prescaler to 159 to get 100KHz clock signal
	TIM2->PSC = 159;

	// 200ms
	TIM2->ARR = 20000;

  	// Enabling the Interrupt for timer 2 and clearing the interrupt status
    TIM2->DIER |= TIM_DIER_UIE;
    TIM2->SR &= ~TIM_SR_UIF;
	NVIC_EnableIRQ(TIM2_IRQn);

    TIM2->CR1 = (1 << 0);
}

/**
 * @brief	IRQ handeler for Timer 2
 * @return	none
 */
void TIM2_IRQHandler(void){

	// Re-settign timer3 ISR
	TIM2->SR &= ~TIM_SR_UIF;

	/*
	 * Setting the timer 2 flag as 1 so that the appropriate code can be
	 * executed where it is required
	 */
	Timer2_flag = 1;

    // Starting timer 2
    TIM2->CR1 = (1 << 0);
}

/**
 * @brief	IRQ handeler for Timer 3
 * @return	none
 */
void TIM3_IRQHandler(void){

	// Re-settign timer3 ISR
	TIM3->SR &= ~TIM_SR_UIF;

	/*
	 * Setting the timer 3 flag as 1 so that the appropriate code can be
	 * executed where it is required
	 */
	Timer3_flag = 1;

    // Starting timer 3
    TIM3->CR1 = (1 << 0);
}


/**
 * @brief	IRQ handeler for Timer 4
 * @return	none
 */
void TIM4_IRQHandler(void){

	// Re-settign timer4 ISR
	TIM4->SR &= ~TIM_SR_UIF;

	/*
	 * Setting the timer 4 flag as 1 so that the appropriate code can be
	 * executed where it is required
	 */
	Timer4_flag = 1;

    // Start Timer 4
    TIM4->CR1 = (1 << 0);
}
