/*
 * Tetris.h
 *
 *  Created on: Dec 4, 2023
 *      Author: vishn
 */

#ifndef INC_TETRIS_H_
#define INC_TETRIS_H_

#include <stdint.h>

typedef enum{
	block_Sq,
	block_T,
	block_Z,
	block_S,
	block_I,
	block_L1,
	block_L2
}tetris_block_name_t;

typedef struct tetris_matrix{
	uint8_t update;
	uint16_t colour;
	uint8_t	status;
}tetris_matrix_t;

#define MATRIX_ROWS 20
#define MATRIX_COLUMNS 10

extern tetris_matrix_t matrix[MATRIX_ROWS][MATRIX_COLUMNS];
extern int update_display, game_end_flag;

void init_Tetris_Game();
void clear_update();
void clear_table();
void insert_Block(tetris_block_name_t block);
void block_down();
uint32_t get_random_num();
int get_block();
void start_game();
#endif /* INC_TETRIS_H_ */
