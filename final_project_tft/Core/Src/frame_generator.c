#include "frame_generator.h"
#include "ili9341.h"
#include <stdint.h>
#include "fonts.h"
#include "Tetris.h"

#define UNIT_WIDTH 14
#define UNIT_HEIGHT 14
#define FRAME_X1 2
#define FRAME_Y1 37
#define FRAME_X2 141
#define FRAME_Y2 317

uint16_t frame[LCD_WIDTH][LCD_HEIGHT];

void FG_Init()
{
	LCD_Clear(LCD_BLACK);
	LCD_Rectangle(FRAME_X1-2,FRAME_Y1-2,FRAME_X2+2,FRAME_Y2+2,LCD_WHITE,2);
	LCD_WriteString(0,0,"TETRIS", Font_16x26,0xFFFF, 0x0000);
	LCD_WriteString(FRAME_X2 + 9,FRAME_Y1,"Score:",Font_11x18,0xFFFF, 0x0000);
}
void FG_UpdateUnit(tetris_matrix_t matrix,uint8_t row,uint8_t col)
{
	uint16_t x1 = FRAME_X1 + col * UNIT_WIDTH;
	uint16_t y1 = FRAME_Y1 + row * UNIT_HEIGHT;
	uint16_t x2 = x1 + UNIT_WIDTH - 1;
	uint16_t y2 = y1 + UNIT_HEIGHT - 1;
	LCD_Fill(x1,y1,x2,y2,matrix.colour,1);
}
void FG_MatrixToFrame(tetris_matrix_t *matrix)
{
	uint8_t row,col;
	for(row = 0; row < MATRIX_ROWS; row++)
	{
		for(col = 0; col < MATRIX_COLUMNS; col++)
		{
			if((matrix->update) == 1)
			{
				FG_UpdateUnit(*matrix,row,col);
				matrix->update = 0;
			}
			matrix++;
		}
	}
}

