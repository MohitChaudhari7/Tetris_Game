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
 * @brief   Header file for the Joystick.c file.
 * 				PC1 -> connect to ADC -> joystick VRy -> ADC1_11
 *				PC2 -> connect to ADC -> joystick VRx -> ADC1_12
 * @author  Vishnu Kumar Thoodur Venkatachalapathy and Mohit Chaudhari
 * @date    Dec 4, 2023
 */

#ifndef INC_JOYSTICK_H_
#define INC_JOYSTICK_H_

#include "stm32f4xx.h"

// Global variable to store the Joystick value in X and Y axis
extern uint16_t Joy_X, Joy_Y;

/**
 * @brief	Starts the ADC convertion to get the current Joysstick value
 * @return	None
 */
void Init_JoyStick();

/**
 * @brief	Initializes the ADC1 and Port C to get input from the Joystick on
 * 			pins PC1 for Y-Axis data and PC2 for X-Axis data
 * @return	None
 */
void Get_JoyStick_Val();

/**
 * @brief	Gets the latest Joystick value and prints it out on the USART port
 * @return	None
 */
void Print_JoyStick_Val();

#endif /* INC_JOYSTICK_H_ */
