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
 * @file    GameInput.c
 * @brief   Header file for the GameInput.c file
 * @author  Vishnu Kumar Thoodur Venkatachalapathy and Mohit Chaudhari
 * @date    Dec 8, 2023
 */

#ifndef INC_GAMEINPUT_H_
#define INC_GAMEINPUT_H_

#include "JoyStick.h"


/**
 * @brief	Initializes all available inputs on the controller
 * @return	None
 */
void Inputs_Init();

/**
 * @brief	Returns a character associated with the direction in which the user
 * 			is moving the joystick
 * @return	character associated with the joystick direction
 * 			w: If user is pressing up direction on the joystick
 * 			s: If user is pressing down direction on the joystick
 * 			a: If user is pressing left direction on the joystick
 * 			d: If user is pressing right direction on the joystick
 * 			0: If user is not giving any input on the joystick
 */
char get_dir();

#endif /* INC_GAMEINPUT_H_ */
