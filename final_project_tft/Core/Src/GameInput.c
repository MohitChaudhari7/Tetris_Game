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
 * @brief   Acts as an interface between the game engine and any available
 * 			input devices
 * @author  Vishnu Kumar Thoodur Venkatachalapathy and Mohit Chaudhari
 * @date    Dec 8, 2023
 */
#include "GameInput.h"

// Thershold defines for the joystick connected to the wireless controller
#define JoyStick_Up_Threshold 2200
#define JoyStick_Down_Threshold 1800
#define JoyStick_Left_Threshold 2000
#define JoyStick_Right_Threshold 2300

/**
 * @brief	Initializes all available inputs on the controller
 * @return	None
 */
void Inputs_Init(){
	Init_JoyStick();
}

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
char get_dir(){
	Get_JoyStick_Val();

	if(Joy_X > JoyStick_Right_Threshold)
		return 'd';

	if(Joy_X < JoyStick_Left_Threshold)
		return 'a';

	if(Joy_Y < JoyStick_Down_Threshold)
		return 's';

	if(Joy_Y > JoyStick_Up_Threshold)
		return 'w';

	return '0';
}
