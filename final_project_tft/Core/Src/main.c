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
 * @file    main.c
 * @brief   Implementation for the wireless transmitter controller
 * @author  Vishnu Kumar Thoodur Venkatachalapathy and Mohit Chaudhari
 * @date    Dec 4, 2023
 */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "spi.h"
#include "ili9341.h"
#include "frame_generator.h"
#include "nrf24l01.h"

#include "UART.h"
#include "Timers.h"
#include "JoyStick.h"
#include "Tetris.h"
#include "GameInput.h"

tetris_matrix_t test_matrix[MATRIX_ROWS][MATRIX_COLUMNS];

int main(void)
{
	// Initialize the required peripherials
	SPI1_Init();
	LCD_Init();
	FG_Init();
	Init_JoyStick();
	Init_Timer2();	// User Timer 2 to generate a ISR every 20ms
	NRF_Init();

	// Setup the nrf module for transmission
	NRF_OpenWritingPipe();
	NRF_StopListening();

	/*
	 * create a variable to store the direction input given by the user and
	 * later send over the nrf channel
	 */

	char x = 'p';

	while (1)
	{
		if(Timer2_flag){
			x = get_dir();
		}

		/*
		 * Sending the latest direction data over nrf channel and waiting for
		 * some time before sending the next data.
		 */
		NRF_SendData(&x,14);
		for(int i = 0; i < 1000;i++);
	}
}
