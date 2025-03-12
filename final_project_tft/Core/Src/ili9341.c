/* vim: set ai et ts=4 sw=4: */
#include "stm32f4xx_hal.h"
#include "ili9341.h"
#include "spi.h"
#include "testimg.h"
#include "fonts.h"

void delay(unsigned int iter)
{
	for(int i =0 ; i <iter;i++)
		;
}

unsigned int LCD_READ_5Bytes(unsigned char cmd)
{
	uint32_t x = 0;
	(GPIOA->BSRR) |= (1 << 16);
	(GPIOA->BSRR) |= (1 << 17);//dc and cs both low to send command
	//delay(1000);//little delay
	SPI1_SendData(cmd);
	(GPIOA->BSRR) |= (1 << 1);
	x |= (SPI1_GetData() << 24);
	WaitForSPI1RXReady();
	x |= ((SPI1->DR) << 16);  // I believe we need this simply because a read must follow a write
	WaitForSPI1RXReady();
	x |= ((SPI1->DR) << 8); // I believe we need this simply because a read must follow a write
	WaitForSPI1RXReady();
	x |= ((SPI1->DR));  // I believe we need this simply because a read must follow a write
	WaitForSPI1RXReady();
	(GPIOA->BSRR) |= (1 << 0);

	return x;
}

void LCD_WriteCommand(uint8_t cmd)//command write
{
	(GPIOA->BSRR) |= (1 << 16);
	(GPIOA->BSRR) |= (1 << 17);//dc and cs both low to send command
	SPI1_SendData(cmd);
	(GPIOA->BSRR) |= (1 << 0);
	(GPIOA->BSRR) |= (1 << 1);
}

void LCD_WriteData(uint8_t data)//data write
{
	(GPIOA->BSRR) |= (1 << 16);//dc and cs both low to send command
	//delay(1000);//little delay
	(GPIOA->BSRR) |= (1 << 1);
	SPI1_SendData(data);
	(GPIOA->BSRR) |= (1 << 0);
}
//set colour for drawing
void LCD_SendColour(uint16_t colour)
{
	LCD_WriteData(colour>>8);
	LCD_WriteData(colour & 0xFF);
}
void LCD_SetAddress(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2)//set coordinate for print or other function
{
	LCD_WriteCommand(0x2A);
	LCD_WriteData(x1>>8);
	LCD_WriteData(x1);
	LCD_WriteData(x2>>8);
	LCD_WriteData(x2);

	LCD_WriteCommand(0x2B);
	LCD_WriteData(y1>>8);
	LCD_WriteData(y1);
	LCD_WriteData(y2>>8);
	LCD_WriteData(y2);

	LCD_WriteCommand(0x2C);//meory write
}
//clear lcd and fill with colour
void LCD_Clear(uint16_t colour)
{
	uint16_t i,j;
	LCD_SetAddress(0,0,LCD_WIDTH-1,LCD_HEIGHT-1);

	for(i=0;i<LCD_WIDTH;i++)
	{
		for(j=0;j<LCD_HEIGHT;j++)
		{
			LCD_SendColour(colour);
		}
	}
}
void LCD_Fill(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2,uint16_t colour, uint16_t border)
{
	uint16_t i,j;
	LCD_SetAddress(x1,y1,x2,y2);

	for(i = x1; i <= x2; i++)
	{
		for(j = y1; j <= y2; j++)
		{
			if((i < (x1 + border)) || (i > (x2 - border)))
				LCD_SendColour(0x1042);
			else if((j < (y1 + border)) || (j > (y2 - border)))
				LCD_SendColour(0x1042);
			else
				LCD_SendColour(colour);
		}
	}
}
void LCD_Rectangle(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2,uint16_t colour,uint16_t border)
{
	LCD_Fill(x1,y1,x2,y1+border-1,colour,0);
	LCD_Fill(x1,y1+border-1,x1+border-1,y2-border+1,colour,0);
	LCD_Fill(x1,y2-border+1,x2,y2,colour,0);
	LCD_Fill(x2-border+1,y1+border-1,x2,y2,colour,0);
}
void LCD_Init()
{
	// Give a clock to port A as pins PA5-thru-PA7 are connected to the gyro (pg 20 of UM1842) and
	// page 116 of RM0383 for the RCC AHB register info.
	(RCC->AHB1ENR) |= 1;

	// Give a clock to port E as pin PE3 is connected to the chip select of the gyro (pg 20 of
	// UM1842) and page 116 of RM0383 for the RCC AHB register info.
	(RCC->AHB1ENR) |= (1 << 4);

	GPIOA->MODER  &= ~(0x3 << (0));
	GPIOA->MODER  |=  (0x1 << (0));
	GPIOA->OTYPER &= ~(1 << 0);
	GPIOA->MODER  &= ~(0x3 << (2));
	GPIOA->MODER  |=  (0x1 << (2));
	GPIOA->OTYPER &= ~(1 << 1);
	// See pg 20 of the ST UM1842 document.  We'll be using SPI1 to communicate with the gyro.
	// See pg 149 of RM0383.  It shows SPI1 to be alternate function 5.
	// See pg 156 of RM0383 for documentation on the GPIO mode register.  It shows how to set
	// pins to alternate mode.  We do this for pins PA5, PA6, PA7.
	(GPIOE->MODER) |= ((1 << 0) | (1 << 2) | (1 << 4));

	//    It should be set to push-pull low-speed output.
//	GPIOD->MODER  &= ~(0x3 << (GREEN_LED_PIN*2));
//	GPIOD->MODER  |=  (0x1 << (GREEN_LED_PIN*2));
//	GPIOD->OTYPER &= ~(1 << GREEN_LED_PIN);


	// Set the pins to fast speed.  See pg 157 for more info on the register.  Pin 5 corresponds to
	// bits 10/11, 6=12/13, and 7=14,15.  And for port E pin 3 corresponds to bits 6/7.
	(GPIOA->OSPEEDR) |= ( (2 << 0)| (2 << 2));
	(GPIOE->OSPEEDR) |= ((2 << 0) | (2 << 2) | (2 << 4));

	// Set the CS high on the gyro as setting it low indicates communication.  See
	// 25 of the L3GD20 datasheet and page 159 of RM0383 for more info on BSRR.
	(GPIOE->BSRR) |= ((1 << 0) | (1 << 1) | (1 << 2)); //cs

	delay(100000);
	GPIOE->BSRR |= (1 << 18);
	delay(100000);
	GPIOE->BSRR |= (1 << 2);
	delay(100000);

	LCD_WriteCommand(0x01);//soft reset
	delay(1000000);

    LCD_WriteCommand(0xEF);
    LCD_WriteData(0x03);
    LCD_WriteData(0x80);
    LCD_WriteData(0x02);

    LCD_WriteCommand(0xCF);
    LCD_WriteData(0x00);
    LCD_WriteData(0XC1);
    LCD_WriteData(0X30);

    LCD_WriteCommand(0xED);
    LCD_WriteData(0x64);
    LCD_WriteData(0x03);
    LCD_WriteData(0X12);
    LCD_WriteData(0X81);

    LCD_WriteCommand(0xE8);
    LCD_WriteData(0x85);
    LCD_WriteData(0x00);
    LCD_WriteData(0x78);

   LCD_WriteCommand(0xCB);
   LCD_WriteData(0x39);
   LCD_WriteData(0x2C);
   LCD_WriteData(0x00);
   LCD_WriteData(0x34);
   LCD_WriteData(0x02);

   LCD_WriteCommand(0xF7);
   LCD_WriteData(0x20);

   LCD_WriteCommand(0xEA);
   LCD_WriteData(0x00);
   LCD_WriteData(0x00);

   LCD_WriteCommand(LCD_PWCTR1);    //Power control
   LCD_WriteData(0x23);   //VRH[5:0]

   LCD_WriteCommand(LCD_PWCTR2);    //Power control
   LCD_WriteData(0x10);   //SAP[2:0];BT[3:0]

   LCD_WriteCommand(LCD_VMCTR1);    //VCM control
   LCD_WriteData(0x3e); //?????
   LCD_WriteData(0x28);

   LCD_WriteCommand(LCD_VMCTR2);    //VCM control2
   LCD_WriteData(0x86);  //--

   LCD_WriteCommand(LCD_MADCTL);    // Memory Access Control
   LCD_WriteData(0x48);

   LCD_WriteCommand(LCD_PIXFMT);
   LCD_WriteData(0x55);

   LCD_WriteCommand(LCD_FRMCTR1);
   LCD_WriteData(0x00);
   LCD_WriteData(0x18);

   LCD_WriteCommand(LCD_DFUNCTR);    // Display Function Control
   LCD_WriteData(0x08);
   LCD_WriteData(0x82);
   LCD_WriteData(0x27);

   LCD_WriteCommand(0xF2);    // 3Gamma Function Disable
   LCD_WriteData(0x00);

   LCD_WriteCommand(LCD_GAMMASET);    //Gamma curve selected
   LCD_WriteData(0x01);

   LCD_WriteCommand(LCD_GMCTRP1);    //Set Gamma
   LCD_WriteData(0x0F);
   LCD_WriteData(0x31);
   LCD_WriteData(0x2B);
   LCD_WriteData(0x0C);
   LCD_WriteData(0x0E);
   LCD_WriteData(0x08);
   LCD_WriteData(0x4E);
   LCD_WriteData(0xF1);
   LCD_WriteData(0x37);
   LCD_WriteData(0x07);
   LCD_WriteData(0x10);
   LCD_WriteData(0x03);
   LCD_WriteData(0x0E);
   LCD_WriteData(0x09);
   LCD_WriteData(0x00);

   LCD_WriteCommand(LCD_GMCTRN1);    //Set Gamma
   LCD_WriteData(0x00);
   LCD_WriteData(0x0E);
   LCD_WriteData(0x14);
   LCD_WriteData(0x03);
   LCD_WriteData(0x11);
   LCD_WriteData(0x07);
   LCD_WriteData(0x31);
   LCD_WriteData(0xC1);
   LCD_WriteData(0x48);
   LCD_WriteData(0x08);
   LCD_WriteData(0x0F);
   LCD_WriteData(0x0C);
   LCD_WriteData(0x31);
   LCD_WriteData(0x36);
   LCD_WriteData(0x0F);

   	//exit sleep
   	LCD_WriteCommand(0x11);
   	delay(1000000);
   	//display on
   	LCD_WriteCommand(0x29);
   	delay(1000000);

    LCD_WriteCommand(0x36);    //Set MADCTL
    LCD_WriteData(0x44);

    LCD_WriteCommand(0x20);    //inversion off

   	LCD_WriteCommand(0x13);    //lcd normal mode on
	//GPIOA->BSRR |= (1 << 0);

}



void LCD_Testing()
{
	LCD_READ_5Bytes(0x32);
	while(1)
	{
		//red mask 0x001F,Green Mask 0x07E, Blue Mask 0xF800
		LCD_SetAddress(0,0,239,239);
		for(int i = 0 ;i <240;i++)
		{
			for(int j = 0; j <240; j++)
				LCD_SendColour(test_img_240x240[i][j]);
		}
		delay(100000);
		LCD_Rectangle(0,0,239,239,0x00FF,5);



//	   	i = i << 1;
//	   	if( i == 0)
//	   		i = 1;
	}
	// See page 36 of the L3GD20 datasheet.  It shows the hi/lo addresses of each of the axes.
//	*x = GetAxisValue(0x28, 0x29);
//	*y = GetAxisValue(0x2A, 0x2B);
//	*z = GetAxisValue(0x2C, 0x2D);
}
//
//static void ILI9341_SetAddressWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
//    // column address set
//    ILI9341_WriteCommand(0x2A); // CASET
//    {
//        uint8_t data[] = { (x0 >> 8) & 0xFF, x0 & 0xFF, (x1 >> 8) & 0xFF, x1 & 0xFF };
//        ILI9341_WriteData(data, sizeof(data));
//    }
//
//    // row address set
//    ILI9341_WriteCommand(0x2B); // RASET
//    {
//        uint8_t data[] = { (y0 >> 8) & 0xFF, y0 & 0xFF, (y1 >> 8) & 0xFF, y1 & 0xFF };
//        ILI9341_WriteData(data, sizeof(data));
//    }
//
//    // write to RAM
//    ILI9341_WriteCommand(0x2C); // RAMWR
//}
////
////void ILI9341_DrawPixel(uint16_t x, uint16_t y, uint16_t color) {
////    if((x >= ILI9341_WIDTH) || (y >= ILI9341_HEIGHT))
////        return;
////
////    ILI9341_Select();
////
////    ILI9341_SetAddressWindow(x, y, x+1, y+1);
////    uint8_t data[] = { color >> 8, color & 0xFF };
////    ILI9341_WriteData(data, sizeof(data));
////
////    ILI9341_Unselect();
////}
////
void LCD_WriteChar(uint16_t x, uint16_t y, char ch, FontDef font, uint16_t colour, uint16_t bgcolour)
{
    uint32_t i, b, j;

    LCD_SetAddress(x, y, x+font.width-1, y+font.height-1);

    for(i = 0; i < font.height; i++) {
        b = font.data[(ch - 32) * font.height + i];
        for(j = 0; j < font.width; j++) {
            if((b << j) & 0x8000)  {
            	LCD_SendColour(colour);
            } else {
            	LCD_SendColour(bgcolour);
            }
        }
    }
}

void LCD_WriteString(uint16_t x, uint16_t y, char *str, FontDef font, uint16_t colour, uint16_t bgcolour)
{
    while(*str) {
        if(x + font.width >= LCD_WIDTH) {
            x = 0;
            y += font.height;
            if(y + font.height >= LCD_HEIGHT) {
                break;
            }

            if(*str == ' ') {
                // skip spaces in the beginning of the new line
                str++;
                continue;
            }
        }

        LCD_WriteChar(x, y, *str, font, colour, bgcolour);
        x += font.width;
        str++;
    }
}
//void ILI9341_DrawImage(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const uint16_t* data) {
//    if((x >= ILI9341_WIDTH) || (y >= ILI9341_HEIGHT)) return;
//    if((x + w - 1) >= ILI9341_WIDTH) return;
//    if((y + h - 1) >= ILI9341_HEIGHT) return;
//
//    ILI9341_Select();
//    ILI9341_SetAddressWindow(x, y, x+w-1, y+h-1);
//    ILI9341_WriteData((uint8_t*)data, sizeof(uint16_t)*w*h);
//    ILI9341_Unselect();
//}
//
//void ILI9341_InvertColors(bool invert) {
//    ILI9341_Select();
//    ILI9341_WriteCommand(invert ? 0x21 /* INVON */ : 0x20 /* INVOFF */);
//    ILI9341_Unselect();
//}
//
