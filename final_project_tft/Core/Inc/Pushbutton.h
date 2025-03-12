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
 * @brief   Header file for the Pushbutton.c file
 * @author  Vishnu Kumar Thoodur Venkatachalapathy and Mohit Chaudhari
 * @date    Dec 4, 2023
 */

#ifndef INC_PUSHBUTTON_H_
#define INC_PUSHBUTTON_H_

#include "stm32f4xx.h"

/**
 * @brief	Setup the On-board pushbutton as an input
 *
 * @return	none
 */
void Init_Push_Buton();

/**
 * @brief	Setup the On-board pushbutton to an external interrupt service routine
 *
 * @return	none
 */
void Init_Push_Button_ITR();

#endif /* INC_PUSHBUTTON_H_ */
