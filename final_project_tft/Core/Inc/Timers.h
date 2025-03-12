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
 * @brief   Header file for the implementation of various timer modules
 * @author  Vishnu Kumar Thoodur Venkatachalapathy and Mohit Chaudhari
 * @date    Dec 4, 2023
 */

#ifndef INC_TIMERS_H_
#define INC_TIMERS_H_

#include "stm32f4xx.h"

extern int Timer4_flag, Timer3_flag, Timer2_flag;

/**
 * @brief	Initializing timer 2 to run for 200ms.
 * @return	none
 */
void Init_Timer2(void);

/**
 * @brief	Initializing timer 3 to run for 1s.
 * @return	none
 */
void Init_Timer3(void);


/**
 * @brief	Initializing timer 4 to run for 20ms.
 * @return	none
 */
void Init_Timer4(void);

#endif /* INC_TIMERS_H_ */
