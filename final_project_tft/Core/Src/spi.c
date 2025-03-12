/*
 * STM32 Projects
 *
 * Copyright (c) 2017 - Terence M. Darwen - tmdarwen.com
 *
 * The MIT License
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "stm32f4xx_hal.h"
#include "spi.h"
#include <stdint.h>

void SPI1_Init()
{
	// Enable clock for SPI1
	(RCC->APB2ENR) |= RCC_APB2ENR_SPI1EN;
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
	GPIOA->MODER &= ~ (GPIO_MODER_MODER5 | GPIO_MODER_MODER6 | GPIO_MODER_MODER7);  // Configure PA2 as Alternative Function.
	GPIOA->MODER |= (GPIO_MODER_MODER5_1 | GPIO_MODER_MODER6_1 | GPIO_MODER_MODER7_1);
	// See pg 149 of ST RM0383 document.  SPI1 is AF5.  And pg 160 of the same document shows
	// alternate function for pin 5 = bits 20-23, pin 6 = bits 24-27, pin 7 = bits 28-31.
	// We set these to 5.
	GPIOA->AFR[0] |= ((5 << 20) | (5 << 24) | (5 << 28));
	GPIOA->OSPEEDR |= ((2 << 10) | (2 << 12) | (2 << 14));


	// See page 602 for details of configuring SPI1 Control Register
	// Set Bit 0: The second clock transition is the first data capture edge
	// Set Bit 1: CK to 1 when idle
	// Set Bit 2: The STM32 is the master, the gyro is the slave
	// Set Bits 3-5 to 010 for a baud rate of fPCLK/8
	// Set Bits 8-9: Software slave management enabled, Internal slave select to 1
	(SPI1->CR1) |= (1 | (1 << 1) | (1 << 2) | (0 << 3) | (1 << 8) | (1 << 9));
	// Set Bit 6: Enable SPI.  See page 603.
	(SPI1->CR1) |= (1 << 6);
}
void WaitForSPI1RXReady()
{
	// See page 605 of the datasheet for info on the SPI status register
	// If Bit0 == 0 then SPI RX is empty
	// If Bit7 == 1 then SPI is busy
//	while(((SPI1->SR & 0x01) == 0) || ((SPI1->SR & 0x80) == 0x80))
//		;
//	while((SPI1->SR & 0x01) == 0)
//		;

	while (((SPI1->SR)&(1<<7))) {};  // wait for BSY bit to Reset -> This will indicate that SPI is not busy in communication
	SPI1->DR = 0;  // send dummy data
	while (!((SPI1->SR) &(1<<0))){};  // Wait for RXNE to set -> This will indicate that the Rx buffer is not empty

}

void WaitForSPI1TXReady()
{
	// See page 605 of the datasheet for info on the SPI status register
	// If Bit1 == 0 then SPI TX buffer is not empty
	// If Bit7 == 1 then SPI is busy
	//while(((SPI1->SR & 0x02) == 0) || ((SPI1->SR & 0x80) == 1))
//	while((SPI1->SR & 0x02) == 0)
//		;
//	int i=0;
//	while (i<size)
//	{
//	   while (!((SPI1->SR)&(1<<1))) {};  // wait for TXE bit to set -> This will indicate that the buffer is empty
//	   SPI1->DR = data[i];  // load the data into the Data Register
//	   i++;
//	}


/*During discontinuous communications, there is a 2 APB clock period delay between the
write operation to the SPI_DR register and BSY bit setting. As a consequence it is
mandatory to wait first until TXE is set and then until BSY is cleared after writing the last
data.
*/
	while (!((SPI1->SR)&(1<<1))) {};  // wait for TXE bit to set -> This will indicate that the buffer is empty
	while (((SPI1->SR)&(1<<7))) {};  // wait for BSY bit to Reset -> This will indicate that SPI is not busy in communication

	//  Clear the Overrun flag by reading DR and SR
	uint8_t temp = SPI1->DR;
	temp = SPI1->SR;
}
void SPI1_SendData(unsigned char data)
{
	WaitForSPI1TXReady();
	SPI1->DR = data;
	WaitForSPI1TXReady();
}
unsigned char SPI1_GetData()
{
	WaitForSPI1RXReady();
	return (SPI1->DR);  // I believe we need this simply because a read must follow a write
}

void SPI2_Init()
{
	// Enable clock for SPI1
	(RCC->APB1ENR) |= RCC_APB1ENR_SPI2EN;
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;

	GPIOB->MODER &= ~ (GPIO_MODER_MODER13 | GPIO_MODER_MODER14 | GPIO_MODER_MODER15);  // Configure PA2 as Alternative Function.
	GPIOB->MODER |= (GPIO_MODER_MODER13_1 | GPIO_MODER_MODER14_1 | GPIO_MODER_MODER15_1);
	// See pg 149 of ST RM0383 document.  SPI1 is AF5.  And pg 160 of the same document shows
	// alternate function for pin 5 = bits 20-23, pin 6 = bits 24-27, pin 7 = bits 28-31.
	// We set these to 5.
	GPIOB->AFR[1] |= ((5 << 20) | (5 << 24) | (5 << 28));
	GPIOB->OSPEEDR |= ((2 << 26) | (2 << 28) | (2 << 30));


	// See page 602 for details of configuring SPI1 Control Register
	// reSet Bit 0: The second clock transition is the first data capture edge
	// reSet Bit 1: CK to 1 when idle
	// Set Bit 2: The STM32 is the master, the gyro is the slave
	// Set Bits 3-5 to 010 for a baud rate of fPCLK/8
	// Set Bits 8-9: Software slave management enabled, Internal slave select to 1
	(SPI2->CR1) &= ~(1 | (1 << 1));
	(SPI2->CR1) |=  (1 << 2) | (3 << 3) | (1 << 8) | (1 << 9);
	// Set Bit 6: Enable SPI.  See page 603.
	(SPI2->CR1) |= (1 << 6);
}
void SPI2_WaitForRXReady()
{
	// See page 605 of the datasheet for info on the SPI status register
	// If Bit0 == 0 then SPI RX is empty
	// If Bit7 == 1 then SPI is busy
//	while(((SPI1->SR & 0x01) == 0) || ((SPI1->SR & 0x80) == 0x80))
//		;
//	while((SPI1->SR & 0x01) == 0)
//		;

	while (((SPI2->SR)&(1<<7))) {};  // wait for BSY bit to Reset -> This will indicate that SPI is not busy in communication
	SPI2->DR = 0;  // send dummy data
	while (!((SPI2->SR) &(1<<0))){};  // Wait for RXNE to set -> This will indicate that the Rx buffer is not empty

}

void SPI2_WaitForTXReady()
{
	// See page 605 of the datasheet for info on the SPI status register
	// If Bit1 == 0 then SPI TX buffer is not empty
	// If Bit7 == 1 then SPI is busy
	//while(((SPI1->SR & 0x02) == 0) || ((SPI1->SR & 0x80) == 1))
//	while((SPI1->SR & 0x02) == 0)
//		;
//	int i=0;
//	while (i<size)
//	{
//	   while (!((SPI1->SR)&(1<<1))) {};  // wait for TXE bit to set -> This will indicate that the buffer is empty
//	   SPI1->DR = data[i];  // load the data into the Data Register
//	   i++;
//	}


/*During discontinuous communications, there is a 2 APB clock period delay between the
write operation to the SPI_DR register and BSY bit setting. As a consequence it is
mandatory to wait first until TXE is set and then until BSY is cleared after writing the last
data.
*/
	while (!((SPI2->SR)&(1<<1))) {};  // wait for TXE bit to set -> This will indicate that the buffer is empty
	while (((SPI2->SR)&(1<<7))) {};  // wait for BSY bit to Reset -> This will indicate that SPI is not busy in communication

	//  Clear the Overrun flag by reading DR and SR
	uint8_t temp = SPI2->DR;
	temp = SPI2->SR;
}
void SPI2_SendData(unsigned char data)
{
	SPI2_WaitForTXReady();
	SPI2->DR = data;
	SPI2_WaitForTXReady();
}
unsigned char SPI2_GetData()
{
	SPI2_WaitForRXReady();
	return (SPI2->DR);  // I believe we need this simply because a read must follow a write
}
