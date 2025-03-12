/*
 * Tetris.c
 *
 *  Created on: Dec 4, 2023
 *      Author: vishn
 */

/*

B			G			R
1111 | 1	111 | 111	1 | 1111 -> 0xFFFF ->

0000 | 0	111 | 111	1 | 1111 -> 0x07FF -> Yellow for Sq
0000 | 0	000 | 000	0 | 1111 -> 0x000F -> Red for Z
0000 | 0	111 | 111	0 | 0000 -> 0x07E0 -> Green for S
1111 | 1	000 | 000	0 | 0000 -> 0xF800 -> Blue for L1
0000 | 0	101 | 001	1 | 1111 -> 0x053F -> Orange for L2
1000 | 0	100 | 000	0 | 0000 -> 0x8400 -> Light Blue for I
1111 | 0	001 | 000	1 | 0100 -> 0xF118 -> Purple for T

1. 0x07E0 -> Green
2. 0x053F -> Orange
3. 0xF800 -> Blue
4. 0x8400 -> Light Blue
5. 0xF118 -> Purple

*/

#include "Tetris.h"
#include "JoyStick.h"
#include "UART.h"
#include "Timers.h"
#include "GameInput.h"
#include "frame_generator.h"

typedef struct tetris_blocks{
	uint8_t blocks[2][4];
	uint16_t colour;
}tetris_blocks_t;

tetris_blocks_t blocks[7];
tetris_matrix_t matrix[MATRIX_ROWS][MATRIX_COLUMNS];
uint8_t blocks_arr[7] = {0,0,0,0,0,0,0};
int row_blocked_flag  = 0, insert_flag = 1, right_end_flag = 0, left_end_flag = 0, row_filled_flag = 0;
int update_display = 0, game_end_flag = 0;


void print_Matrix(){
	cputs("\r\nMatrix info:");
	cputs("\r\n\t0\t1\t2\t3\t4\t5\t6\t7\t8\t9");
	for(int i=0; i<MATRIX_ROWS; i++){
		cputs("\r\n");
		putnum(i);
		cputs(": ");

		for(int j=0; j<MATRIX_COLUMNS; j++){
//			putchar('\t');
//			putchar(' ');
//			putchar(' ');
			cputs("\t");
			putnum(- matrix[i][j].status - matrix[i][j].update);
			putchar(',');
//			putnum(matrix[i][j].update);
//			putchar(',');
			putnum(matrix[i][j].colour);
		}
	}
}

void init_Sq(){
	blocks[block_Sq].blocks[0][1] = 1;
	blocks[block_Sq].blocks[0][2] = 1;
	blocks[block_Sq].blocks[1][1] = 1;
	blocks[block_Sq].blocks[1][2] = 1;

	blocks[block_Sq].colour = 0x07FF; // Yellow color code
}

void init_T(){
	blocks[block_T].blocks[0][1] = 1;
	blocks[block_T].blocks[1][0] = 1;
	blocks[block_T].blocks[1][1] = 1;
	blocks[block_T].blocks[1][2] = 1;

	blocks[block_T].colour = 0xF118; // Purple color code
}

void init_Z(){
	blocks[block_Z].blocks[0][0] = 1;
	blocks[block_Z].blocks[0][1] = 1;
	blocks[block_Z].blocks[1][1] = 1;
	blocks[block_Z].blocks[1][2] = 1;

	blocks[block_Z].colour = 0x000F; // Red color code
}

void init_S(){
	blocks[block_S].blocks[0][1] = 1;
	blocks[block_S].blocks[0][2] = 1;
	blocks[block_S].blocks[1][0] = 1;
	blocks[block_S].blocks[1][1] = 1;

	blocks[block_S].colour = 0x07E0; // Green color code
}

void init_L1(){
	blocks[block_L1].blocks[0][0] = 1;
	blocks[block_L1].blocks[1][0] = 1;
	blocks[block_L1].blocks[1][1] = 1;
	blocks[block_L1].blocks[1][2] = 1;

	blocks[block_L1].colour = 0xF800; // Blue color code
}

void init_L2(){
	blocks[block_L2].blocks[0][2] = 1;
	blocks[block_L2].blocks[1][0] = 1;
	blocks[block_L2].blocks[1][1] = 1;
	blocks[block_L2].blocks[1][2] = 1;

	blocks[block_L2].colour = 0x053F; // Orange color code
}

void init_I(){
	blocks[block_I].blocks[0][0] = 1;
	blocks[block_I].blocks[0][1] = 1;
	blocks[block_I].blocks[0][2] = 1;
	blocks[block_I].blocks[0][3] = 1;

	blocks[block_I].colour = 0x8400; // Purple color code
}

void init_Tetris_Game(){
	for(int i=0; i<MATRIX_ROWS; i++){
		for(int j=0; j<MATRIX_COLUMNS; j++){
			matrix[i][j].update = 1;
			matrix[i][j].colour = 0;
			matrix[i][j].status = 0;
		}
	}

	for(int k=0; k<7; k++){
		for(int i=0; i<2; i++){
			for(int j=0; j<4; j++){
				blocks[k].blocks[i][j] = 0;
			}
		}
	}

	init_Sq();
	init_T();
	init_Z();
	init_S();
	init_L1();
	init_L2();
	init_I();
	update_display = 1;
}

void clear_update(){
	for(int i=0; i<MATRIX_ROWS; i++){
		for(int j=0; j<MATRIX_COLUMNS; j++){
			matrix[i][j].update = 0;
		}
	}
	update_display = 0;
}

void clear_table(){
	for(int i=0; i<MATRIX_ROWS; i++){
		for(int j=0; j<MATRIX_COLUMNS; j++){
			matrix[i][j].update = 0;
			matrix[i][j].colour = 0;
		}
	}
	update_display = 1;
}

void insert_Block(tetris_block_name_t block){
	for(int i=0; i<2; i++){
		for(int j=3; j<7; j++){
			if(blocks[block].blocks[i][j-3]){
				matrix[i][j].update = 1;
				matrix[i][j].colour = blocks[block].colour;
				matrix[i][j].status = 1;
			}
		}
	}
	update_display = 1;
}

void block_down(){
	for(int i=MATRIX_ROWS; i>=0; i--){
		for(int j=0; j<MATRIX_COLUMNS; j++){
			if((matrix[i][j].status == 1) && (i != MATRIX_ROWS)){
				matrix[i+1][j].colour = matrix[i][j].colour;
				if((i == 0)||(matrix[i-1][j].status == 0))
					matrix[i][j].colour = 0;
				else
					matrix[i][j].colour = matrix[i-1][j].colour;
				matrix[i+1][j].update = 1;
				matrix[i+1][j].status = 1;
				matrix[i][j].update=1;
				matrix[i][j].status=0;
			}
		}
	}
	update_display = 1;
}

void block_left(){

	// Check if there is a block to the left
	for(int j=0; j<MATRIX_COLUMNS; j++){
		for(int i=MATRIX_ROWS-1; i>=0; i--){
			if(matrix[i][j].status == 1){
				if((matrix[i][j-1].status == 0) &&(matrix[i][j-1].colour != 0))
					return;
			}
		}
	}

	for(int j=0; j<MATRIX_COLUMNS; j++){
		for(int i=MATRIX_ROWS-1; i>=0; i--){
			if((matrix[i][j].status == 1) && (j != 0)){
//				if((matrix[i][j-1].status == 0) && (matrix[i][j-1].colour != 0))
//				if((matrix[i][j-1].status == 0))
//				if((matrix[i][j-1].colour != 0))
//					return;
//				else

//				if((matrix[i][j-1].colour == 0)||(matrix[i][j-1].status == 1))
					matrix[i][j-1].colour = matrix[i][j].colour;

				if((j == (MATRIX_COLUMNS-1)) || (matrix[i][j+1].status != 1))
					matrix[i][j].colour = 0;
				else
					matrix[i][j].colour = matrix[i][j+1].colour;

				matrix[i][j-1].update = 1;
				matrix[i][j-1].status = 1;
				matrix[i][j].update=1;
				matrix[i][j].status=0;
			}
		}
	}

	update_display = 1;
}

void block_right(){

	// Check if there is a block to the right
	for(int j=MATRIX_COLUMNS-1; j>=0; j--){
		for(int i=MATRIX_ROWS-1; i>=0; i--){
			if(matrix[i][j].status == 1){
				if((matrix[i][j+1].status == 0) &&(matrix[i][j+1].colour != 0))
					return;
			}
		}
	}

	for(int j=MATRIX_COLUMNS-1; j>=0; j--){
		for(int i=MATRIX_ROWS-1; i>=0; i--){
			if((matrix[i][j].status == 1) && (j != (MATRIX_COLUMNS-1))){
//				if((matrix[i][j+1].status == 0) && (matrix[i][j+1].colour != 0))
//					return;
//				else
					matrix[i][j+1].colour = matrix[i][j].colour;

				if((j == 0) || (matrix[i][j-1].status != 1))
					matrix[i][j].colour = 0;
				else
					matrix[i][j].colour = matrix[i][j-1].colour;

				matrix[i][j+1].update = 1;
				matrix[i][j+1].status = 1;
				matrix[i][j].update=1;
				matrix[i][j].status=0;
			}
		}
	}

	update_display = 1;
}

uint32_t get_random_num(){
    // x0=seed; a=multiplier; b=increment; m=modulus;
	Get_JoyStick_Val();
	uint32_t a = 3, b = 1, m = 8;
	uint32_t x0 = (Joy_X - Joy_Y)%m;
	x0 = (a * x0 + b) % m;
	return x0;
}

int get_block(){
	int x = 0;
	static int f=0;

	if(f==7){
		for(int i=0; i<7; i++)
			blocks_arr[i] = 0;
		f=0;
	}

	do{

		if(f == 6){
			for(int i=0; i<7; i++){
				if(blocks_arr[i] == 0){
					blocks_arr[i] = 1;
					x = i;
					f++;
					break;
				}
			}
			break;
		}

		x = get_random_num()%7;

		if(blocks_arr[x] == 1)
			continue;
		else if(blocks_arr[x] == 0){
			blocks_arr[x] = 1;
			f++;
			break;
		}

	}while(1);

	return(x);
}

void clear_status(){
	for(int i=MATRIX_ROWS; i>=0; i--){
		for(int j=0; j<MATRIX_COLUMNS; j++){
			if(matrix[i][j].status == 1)
				matrix[i][j].status = 0;
		}
	}
}

void last_row_check(){
	for(int i=0;i<MATRIX_COLUMNS; i++){
		if(matrix[MATRIX_ROWS-1][i].status == 1){
			insert_flag = 1;
			break;
		}
	}

	if(insert_flag){
		clear_status();
	}
}

void left_end_check(){
	for(int i=0;i<MATRIX_ROWS; i++){
		if(matrix[i][0].status == 1){
			left_end_flag = 1;
			break;
		}
	}
}

void right_end_check(){
	for(int i=0;i<MATRIX_ROWS; i++){
		if(matrix[i][MATRIX_COLUMNS-1].status == 1){
			right_end_flag  = 1;
			break;
		}
	}
}

//TBC
void check_blocked_row(){

	row_blocked_flag = 0;
	for(int j=0; j<MATRIX_COLUMNS; j++){
		for(int i=MATRIX_ROWS-2; i>=0; i--){
			if((matrix[i][j].status == 1)&&(matrix[i+1][j].status == 0)){
				if(matrix[i+1][j].colour != 0){
					row_blocked_flag = 1;
					break;
				}
			}
			if(row_blocked_flag)
				break;
		}
		if(row_blocked_flag)
			break;
	}

	if(row_blocked_flag){
		clear_status();
	}
}


void block_down_end(){
	int x=0;
	do{
		last_row_check();
		check_blocked_row();
		if((row_blocked_flag == 1)||(insert_flag == 1)){
			cputs("\r\nBrought block down ");
			putnum(x);
			cputs(" times");
			break;
		}
		block_down();
		x++;
	}while(1);
	update_display = 1;
}

void block_down_columns(int row_start, int row_end, int row_dest, int col, int n){
	int copy_i=row_start, dest_i=row_dest, j=col;
	for(int k=0; k<n; k++){
		matrix[dest_i][j].colour = matrix[copy_i][j].colour;
		matrix[copy_i][j].colour = 0;
		matrix[dest_i][j].update = 1;
		matrix[copy_i][j].update = 1;
		dest_i++;
		copy_i++;
	}
}

void clear_row(){
//	int num_col_blocks = 0, block_row_start = -1, block_row_end = -1, row_dest = row_filled_flag-1;

	int row_dest = row_filled_flag;
	// Clear the given row
	for(int j=0; j<MATRIX_COLUMNS; j++){
		matrix[row_filled_flag][j].colour = 0;
		matrix[row_filled_flag][j].update = 1;
	}

	// go through each columns and bring down the above blocks
	for(int j=0; j<MATRIX_COLUMNS; j++){
		row_dest = row_filled_flag;
		for(int i=row_filled_flag-1; i>=0; i--){
			matrix[row_dest][j].colour = matrix[i][j].colour;
			matrix[row_dest][j].update = 1;
			row_dest--;
			if(i == 0){
				matrix[i][j].colour = 0;
				matrix[i][j].update = 1;
			}
		}
	}
	row_filled_flag = 0;
	update_display = 1;
}

//void clear_row(){
////	int num_col_blocks = 0, block_row_start = -1, block_row_end = -1, row_dest = row_filled_flag-1;
//
//	int row_dest = row_filled_flag;
//	// Clear the given row
//	for(int j=0; j<MATRIX_COLUMNS; j++){
//		matrix[row_filled_flag][j].colour = 0;
//		matrix[row_filled_flag][j].update = 1;
//	}
//
//	//bring down the blocks from above
//	for(int j=0; j<MATRIX_COLUMNS; j++){ // go through each columns and bring down the above blocks
//
//		// we have num_blocks to bring down, row_start and row_end
//		// Need to check if the blocks below are empty or not and
//		for(int i=row_filled_flag; i<MATRIX_ROWS; i++){
//			if(matrix[i][j].colour != 0){
//				break;
//			}
//			else if(matrix[i][j].colour == 0){
//				row_dest = i;
//			}
//		}
//
//		for(int i=row_filled_flag-1; i>=0; i--){
//			if(matrix[i][j].colour != 0){
////				cputs("\r\n Copying from matrix[");
////				putnum(i);
////				cputs("][");
////				putnum(j);
////				cputs("] to matrix[");
////				putnum(row_dest);
////				cputs("][");
////				putnum(j);
////				cputs("]");
//
//				matrix[row_dest][j].colour = matrix[i][j].colour;
//
////				if(i == 0)
//					matrix[i][j].colour = 0;
////				else
////					matrix[i][j].colour = matrix[i-1][j].colour;
//
//				matrix[row_dest][j].update = 1;
//				matrix[i][j].update = 1;
//				row_dest--;
////				cputs(" updated row_dest to: ");
////				putnum(row_dest);
//			}
//			else if(matrix[i][j].colour == 0){
////				cputs("\r\n Skipping matrix[");
////				putnum(i);
////				cputs("][");
////				putnum(j);
////				cputs("]");
//			}
//
//		}
//
////		for(int i=row_filled_flag-1; i>=0; i--){
////			if(matrix[i][j].colour != 0){
////				num_col_blocks++;
////				if(block_row_start == -1)
////					block_row_start = i;
////			}
////			else if((matrix[i][j].colour == 0)&&(block_row_start != -1)){
////				block_row_end = i+1;
////			}
////		}
////
////		block_down_columns(block_row_start, block_row_end, row_dest, j, num_col_blocks);
////		num_col_blocks = 0;
////		block_row_start = -1;
////		block_row_end = -1;
////		row_dest = row_filled_flag-1;
//	}
//	row_filled_flag = 0;
//	update_display = 1;
//}

void checked_row_filled(){
	int x=0;
	row_filled_flag = 0;
	for(int i=0; i<MATRIX_ROWS; i++){
		for(int j=0; j<MATRIX_COLUMNS; j++){
			if(matrix[i][j].colour != 0)
				x++;
		}
		if(x == MATRIX_COLUMNS){
			row_filled_flag = i;
			break;
		}
		else
			x = 0;
	}
}

void game_end_check(){
	check_blocked_row();
	if(row_blocked_flag == 1){
		row_blocked_flag = 0;
		game_end_flag = 1;
		update_display = 1;
	}
}

void start_game(){
	int x=0;

	Inputs_Init();
	// Init JoyStick
//	char IP = 0;
	int Joy_ip = 0;
	init_Tetris_Game();

	print_Matrix();
	clear_update();

	Init_Timer2();
	Init_Timer3();

	while(1){
//			IP = getchar();
			if(Timer3_flag){
				Timer3_flag = 0;
				if(insert_flag){
					x = get_block();
					cputs("\r\nInserting block: ");
					putnum(x);
					cputs("\r\n");
					insert_Block(x);
					insert_flag = 0;
					game_end_check();
					if(game_end_flag){
						cputs("\r\n\n\n!!!!!!!GAME OVER!!!!!!!\r\n\n\n");
						print_Matrix();
						break;
					}
				}
				else{
//					block_down();
					last_row_check();
					check_blocked_row();
					if(row_blocked_flag == 1){
						row_blocked_flag = 0;
						insert_flag = 1;
					}
					checked_row_filled();
					if(row_filled_flag != 0){
						clear_row();
					}
					block_down();
				}
			}

			if(Timer2_flag){
				Joy_ip = get_dir();
				cputs("\r\nJoy ip: ");
				putnum(Joy_ip);
				cputs("\r\n");
//				if(IP == 'a'){
				if(Joy_ip == 3){
					left_end_check();
					if(!left_end_flag)
						block_left();
					else
						left_end_flag = 0;
				}
//				else if(IP == 'd'){
				else if(Joy_ip == 4){
					right_end_check();
					if(!right_end_flag)
						block_right();
					else
						right_end_flag = 0;
				}
//				else if(IP == 's'){
				else if(Joy_ip == 2){
					block_down_end();
					if(row_blocked_flag == 1){
						row_blocked_flag = 0;
						insert_flag = 1;
					}
					checked_row_filled();
					if(row_filled_flag != 0){
						clear_row();
					}
				}
			}
//			print_Matrix();
			if(update_display == 1)
			{
				FG_MatrixToFrame(&matrix[0][0]);
				update_display = 0;
			}
//			clear_update();
		}

//	while(1){
//		IP = getchar();
//		if(Timer3_flag){
//			Timer3_flag = 0;
//			if(insert_flag){
//				x = get_block();
//				cputs("\r\nInserting block: ");
//				putnum(x);
//				cputs("\r\n");
//				insert_Block(x);
//				insert_flag = 0;
//				game_end_check();
//				if(game_end_flag){
//					cputs("\r\n\n\n!!!!!!!GAME OVER!!!!!!!\r\n\n\n");
//					print_Matrix();
//					break;
//				}
//			}
//			else{
//				block_down();
//				if(IP == 'a'){
//					left_end_check();
//					if(!left_end_flag)
//						block_left();
//					else
//						left_end_flag = 0;
//				}
//				else if(IP == 'd'){
//					right_end_check();
//					if(!right_end_flag)
//						block_right();
//					else
//						right_end_flag = 0;
//				}
//				else if(IP == 's'){
//					block_down_end();
//					if(row_blocked_flag == 1){
//						row_blocked_flag = 0;
//						insert_flag = 1;
//					}
//				}
//
//				last_row_check();
//				check_blocked_row();
//				if(row_blocked_flag == 1){
//					row_blocked_flag = 0;
//					insert_flag = 1;
//				}
//				checked_row_filled();
//				if(row_filled_flag != 0){
//					clear_row();
//				}
//			}
//			print_Matrix();
//			clear_update();
//		}
//	}
}
