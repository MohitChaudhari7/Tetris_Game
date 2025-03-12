/* vim: set ai et ts=4 sw=4: */
#ifndef __ILI9341_H__
#define __ILI9341_H__

#include "fonts.h"

#define LCD_CS_PORT
#define LCD_CS_PIN
#define LCD_WIDTH  240
#define LCD_HEIGHT 320

#define LCD_NOP     0x00
#define LCD_SWRESET 0x01
#define LCD_RDDID   0x04
#define LCD_RDDST   0x09

#define LCD_SLPIN   0x10
#define LCD_SLPOUT  0x11
#define LCD_PTLON   0x12
#define LCD_NORON   0x13

#define LCD_RDMODE  0x0A
#define LCD_RDMADCTL  0x0B
#define LCD_RDPIXFMT  0x0C
#define LCD_RDIMGFMT  0x0A
#define LCD_RDSELFDIAG  0x0F

#define LCD_INVOFF  0x20
#define LCD_INVON   0x21
#define LCD_GAMMASET 0x26
#define LCD_DISPOFF 0x28
#define LCD_DISPON  0x29

#define LCD_CASET   0x2A
#define LCD_PASET   0x2B
#define LCD_RAMWR   0x2C
#define LCD_RAMRD   0x2E

#define LCD_PTLAR   0x30
#define LCD_MADCTL  0x36
#define LCD_PIXFMT  0x3A

#define LCD_FRMCTR1 0xB1
#define LCD_FRMCTR2 0xB2
#define LCD_FRMCTR3 0xB3
#define LCD_INVCTR  0xB4
#define LCD_DFUNCTR 0xB6

#define LCD_PWCTR1  0xC0
#define LCD_PWCTR2  0xC1
#define LCD_PWCTR3  0xC2
#define LCD_PWCTR4  0xC3
#define LCD_PWCTR5  0xC4
#define LCD_VMCTR1  0xC5
#define LCD_VMCTR2  0xC7

#define LCD_RDID1   0xDA
#define LCD_RDID2   0xDB
#define LCD_RDID3   0xDC
#define LCD_RDID4   0xDD

#define LCD_GMCTRP1 0xE0
#define LCD_GMCTRN1 0xE1

#define LCD_BLACK 0x0000
#define LCD_WHITE 0xFFFF

void LCD_Testing();
void LCD_Init();
void LCD_Fill(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2,uint16_t colour, uint16_t border);
void LCD_Rectangle(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2,uint16_t colour,uint16_t border);
void LCD_Clear(uint16_t colour);
void LCD_WriteChar(uint16_t x, uint16_t y, char ch, FontDef font, uint16_t colour, uint16_t bgcolour);
void LCD_WriteString(uint16_t x, uint16_t y, char *str, FontDef font, uint16_t colour, uint16_t bgcolour);

#endif // __ILI9341_H__
