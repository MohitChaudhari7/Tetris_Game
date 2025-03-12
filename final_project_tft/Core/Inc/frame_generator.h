#include <stdint.h>
#include "Tetris.h"

//typedef struct tetris_matrix{
//	uint8_t update;
//	uint16_t colour;
//}tetris_matrix_t;

//#define MATRIX_ROWS 20
//#define MATRIX_COLUMNS 10
extern tetris_matrix_t test_matrix[MATRIX_ROWS][MATRIX_COLUMNS];
void FG_MatrixToFrame(tetris_matrix_t *matrix);
void FG_Init();

