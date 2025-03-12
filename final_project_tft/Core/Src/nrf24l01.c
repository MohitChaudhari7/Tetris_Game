#include <stdint.h>
#include <stdbool.h>
#include "stm32f4xx_hal.h"
#include "nrf24l01.h"
#include "spi.h"

#define NRF_CE_RESET ((GPIOB->BSRR) |= (1 << 16))
#define NRF_CE_SET ((GPIOB->BSRR) |= (1 << 0))
#define NRF_CSN_RESET ((GPIOB->BSRR) |= (1 << 17))
#define NRF_CSN_SET ((GPIOB->BSRR) |= (1 << 1))
#define max(a, b) (a > b ? a : b)
#define min(a, b) (a < b ? a : b)

/* Memory Map */
#define NRF_CONFIG  0x00
#define EN_AA       0x01
#define EN_RXADDR   0x02
#define SETUP_AW    0x03
#define SETUP_RETR  0x04
#define RF_CH       0x05
#define RF_SETUP    0x06
#define NRF_STATUS  0x07
#define OBSERVE_TX  0x08
#define CD          0x09
#define RX_ADDR_P0  0x0A
#define RX_ADDR_P1  0x0B
#define RX_ADDR_P2  0x0C
#define RX_ADDR_P3  0x0D
#define RX_ADDR_P4  0x0E
#define RX_ADDR_P5  0x0F
#define TX_ADDR     0x10
#define RX_PW_P0    0x11
#define RX_PW_P1    0x12
#define RX_PW_P2    0x13
#define RX_PW_P3    0x14
#define RX_PW_P4    0x15
#define RX_PW_P5    0x16
#define FIFO_STATUS 0x17
#define DYNPD       0x1C
#define FEATURE     0x1D

/* Bit Mnemonics */
#define MASK_RX_DR  6
#define MASK_TX_DS  5
#define MASK_MAX_RT 4
#define EN_CRC      3
#define CRCO        2
#define PWR_UP      1
#define PRIM_RX     0
#define ENAA_P5     5
#define ENAA_P4     4
#define ENAA_P3     3
#define ENAA_P2     2
#define ENAA_P1     1
#define ENAA_P0     0
#define ERX_P5      5
#define ERX_P4      4
#define ERX_P3      3
#define ERX_P2      2
#define ERX_P1      1
#define ERX_P0      0
#define AW          0
#define ARD         4
#define ARC         0
#define PLL_LOCK    4
#define CONT_WAVE   7
#define RF_DR       3
#define RF_PWR      6
#define RX_DR       6
#define TX_DS       5
#define MAX_RT      4
#define RX_P_NO     1
#define TX_FULL     0
#define PLOS_CNT    4
#define ARC_CNT     0
#define TX_REUSE    6
#define FIFO_FULL   5
#define TX_EMPTY    4
#define RX_FULL     1
#define RX_EMPTY    0
#define DPL_P5      5
#define DPL_P4      4
#define DPL_P3      3
#define DPL_P2      2
#define DPL_P1      1
#define DPL_P0      0
#define EN_DPL      2
#define EN_ACK_PAY  1
#define EN_DYN_ACK  0

/* Instruction Mnemonics */
#define R_REGISTER    0x00
#define W_REGISTER    0x20
#define REGISTER_MASK 0x1F
#define ACTIVATE      0x50
#define R_RX_PL_WID   0x60
#define R_RX_PAYLOAD  0x61
#define W_TX_PAYLOAD  0xA0
#define W_ACK_PAYLOAD 0xA8
#define FLUSH_TX      0xE1
#define FLUSH_RX      0xE2
#define REUSE_TX_PL   0xE3
#define RF24_NOP      0xFF

/* Non-P omissions */
#define LNA_HCURR 0

/* P model memory Map */
#define RPD                 0x09
#define W_TX_PAYLOAD_NO_ACK 0xB0

/* P model bit Mnemonics */
#define RF_DR_LOW   5
#define RF_DR_HIGH  3
#define RF_PWR_LOW  1
#define RF_PWR_HIGH 2

uint8_t addressWidth = 5;

void NRF_Write(uint8_t reg, uint8_t* buf, uint8_t len)
{

	NRF_CSN_RESET;

	SPI2_SendData(W_REGISTER | reg);

    while (len--)
    {
    	SPI2_SendData(*buf++);
    }

    NRF_CSN_SET;
}
uint8_t NRF_Read(uint8_t reg)
{

	NRF_CSN_RESET;


	SPI2_SendData(R_REGISTER | reg);
	uint8_t result = SPI2_GetData();

    NRF_CSN_SET;

    return result;
}
void delay_iter(unsigned int iter)
{
	for(int i =0 ; i <iter;i++)
		;
}
volatile uint8_t config = 0;
uint8_t pipe0_reading_address[5];
void NRF_Init()
{

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
	GPIOB->MODER  &= ~(0x3 << (0));
	GPIOB->MODER  |=  (0x1 << (0));
	GPIOB->OTYPER &= ~(1 << 0);
	GPIOB->MODER  &= ~(0x3 << (2));
	GPIOB->MODER  |=  (0x1 << (2));
	GPIOB->OTYPER &= ~(1 << 1);
	GPIOB->OSPEEDR |= ( (2 << 0)| (2 << 2));

	NRF_CE_RESET;
	NRF_CSN_SET;

	SPI2_Init();

	uint8_t val;
	delay_iter(1000000);
	//retries
	val = (uint8_t)((5) << ARD | (15));
	NRF_Write(SETUP_RETR,&val ,1);

	//set data rate
	//val = NRF_Read(RF_SETUP);
	val = 0x06;
	NRF_Write(RF_SETUP,&val,1);

	//features
	val = 0;
	NRF_Write(FEATURE,&val,1);

	//dynamic payload
	val = 0;
	NRF_Write(DYNPD,&val,1);

	val = 0x3F;
    NRF_Write(EN_AA, &val,1);  // enable auto-ack on all pipes

    val = 3;
    NRF_Write(EN_RXADDR,&val,1); // only open RX pipes 0 & 1

    // write static payload size setting for all pipes
    val = 1;
    for (uint8_t i = 0; i < 6; ++i)
    {
        NRF_Write((uint8_t)(RX_PW_P0 + i), &val, 1);
    }

    //setup address width as 5 bytes
    val = 3;
    NRF_Write(SETUP_AW, &val,1);

    //set channel
    val = 76;
    NRF_Write(RF_CH, &val, 1);

    // Reset current status
    // Notice reset and flush is the last thing we do
    val = 0x70;
    NRF_Write(NRF_STATUS, &val ,1);

    //flush rx and tx
    val = RF24_NOP;
    NRF_Write(FLUSH_RX,&val,1);
    NRF_Write(FLUSH_TX,&val,1);

    //enable 2 bytes crc
    val = 0x0C; //00001100
    NRF_Write(NRF_CONFIG,&val,1);

    //powerup
    val |= 0x02;
    NRF_Write(NRF_CONFIG,&val,1);

    delay_iter(1000000);

    config = NRF_Read(NRF_CONFIG);

}
uint8_t addr[6] = "00001";
void NRF_OpenReadingPipe()
{
	NRF_Write(RX_ADDR_P0,addr, addressWidth);

	NRF_Write(TX_ADDR, addr, addressWidth);

	uint8_t val = NRF_Read(EN_RXADDR) | (1 << ERX_P0);
	NRF_Write(EN_RXADDR,&val,1);

	//set PA level to minimum -18db
    val = NRF_Read(RF_SETUP) & (0xF8);
    NRF_Write(RF_SETUP, &val,1);

}
void NRF_OpenWritingPipe()
{
	NRF_Write(RX_ADDR_P0,addr, addressWidth);

	NRF_Write(TX_ADDR, addr, addressWidth);

	//set PA level to minimum -18db
    uint8_t val = NRF_Read(RF_SETUP) & (0xF8);
    NRF_Write(RF_SETUP, &val,1);

}
void NRF_StartListening()
{
	config |= (1 << PRIM_RX);
	uint8_t val = config;
	NRF_Write(NRF_CONFIG,&val,1);

	val = (1 << RX_DR) | (1 << TX_DS) | (1 << MAX_RT);
	NRF_Write(NRF_STATUS,&val,1);

	NRF_CE_SET;

	NRF_Write(RX_ADDR_P0,addr, addressWidth);
}
void NRF_StopListening()
{
	NRF_CE_RESET;

	delay_iter(1000000);

	config &= ~(1 << PRIM_RX);
	uint8_t val = config;
	NRF_Write(NRF_CONFIG,&val,1);

	val = (NRF_Read(EN_RXADDR) | (1 << ERX_P0));
	NRF_Write(EN_RXADDR,&val,1);
	NRF_CE_SET;

	NRF_Write(RX_ADDR_P0,addr, addressWidth);

}
bool NRF_Avaiable()
{
	uint8_t val = NRF_Read(FIFO_STATUS);
    if (val & 1) { // if RX FIFO is empty
        return 0;
    }
    return 1;
}
void NRF_ReceiveData(char* ptr,uint8_t len)
{
	NRF_CSN_RESET;
	SPI2_SendData(R_RX_PAYLOAD);
	while (len--) {
		*ptr++ = SPI2_GetData();;
	}
    NRF_CSN_SET;

    uint8_t val = (1 << RX_DR);
    NRF_Write(NRF_STATUS,&val,1);
}

void NRF_SendData(char* ptr,uint8_t len)
{
	uint8_t blank_len = 32 - len;
	NRF_CSN_RESET;
	SPI2_SendData(W_TX_PAYLOAD);
//    while (len--) {
    	SPI2_SendData(*ptr);
//    }
//    while (blank_len--) {
//    	SPI2_SendData(0);
//    }
    NRF_CSN_SET;

    NRF_CE_RESET;
    delay_iter(10000);
    NRF_CE_SET;

    uint8_t val = (NRF_Read(NRF_STATUS));
//    while(!(val & ((1 << (TX_DS)) | (1 << (MAX_RT)))))
//    {
//    	delay_iter(100);
//    	val = NRF_Read(OBSERVE_TX);
//    	val = (NRF_Read(NRF_STATUS));
//    }

//	while(NRF_Read(NRF_STATUS) & 1)
//		;
	NRF_Write(FLUSH_TX,&val,1);
    if((NRF_Read(NRF_STATUS)) &  1)
	{
    	val = RF24_NOP;
		NRF_Write(FLUSH_TX,&val,1);
	}
    else if((NRF_Read(NRF_STATUS)) &  (1 << (MAX_RT)))
	{
    	val = RF24_NOP;
		NRF_Write(FLUSH_TX,&val,1);
	}

	val = (1 << RX_DR) | (1 << TX_DS) | (1 << MAX_RT);
	NRF_Write(NRF_STATUS,&val,1);
    //flush rx and tx


//	uint8_t val;
//	NRF_CSN_RESET;
//    val = NRF_Read(NRF_STATUS);         /in order to check TX_FIFO status/
//    if ((!(val & (1 << TX_FULL))))
//    {
//      current_acknowledgement_state = acknowledgement_state;      /setting the acknowledgement state to either NO_ACK or ACK, based on input/
//      if (dynamic_payload == ENABLE)
//        payload_width = current_payload_width;
//	  SPI2_SendData(W_TX_PAYLOAD);
//	  while (len--) {
//		SPI2_SendData((*ptr++));
//	  }
//	  while (blank_len--) {
//		SPI2_SendData(0);
//	  }
//      return (TRANSMIT_BEGIN);                                     /TX FIFO is not full and nrf24l01+ mode is standby ii or ptx/
//    }
//    else
//    {
//      return (TRANSMIT_FAIL);            /TX FIFO full or wrong mode/
//    }
}

//void NRF_SendData(char* ptr,uint8_t len)
//{
//	uint8_t blank_len = 32 - len;
//	NRF_CSN_RESET;
//	SPI2_SendData(W_TX_PAYLOAD);
//    while (len--) {
//    	SPI2_SendData((*ptr++));
//    }
//    while (blank_len--) {
//    	SPI2_SendData(0);
//    }
//    NRF_CSN_SET;
//
//    NRF_CE_RESET;
//    delay_iter(10000);
//    NRF_CE_SET;
//
//    uint8_t val = (NRF_Read(NRF_STATUS));
////    while(!(val & ((1 << (TX_DS)) | (1 << (MAX_RT)))))
////    {
////    	delay_iter(100);
////    	val = NRF_Read(OBSERVE_TX);
////    	val = (NRF_Read(NRF_STATUS));
////    }
//
////	while(NRF_Read(NRF_STATUS) & 1)
////		;
//	NRF_Write(FLUSH_TX,&val,1);
//    if((NRF_Read(NRF_STATUS)) &  1)
//	{
//    	val = RF24_NOP;
//		NRF_Write(FLUSH_TX,&val,1);
//	}
//    else if((NRF_Read(NRF_STATUS)) &  (1 << (MAX_RT)))
//	{
//    	val = RF24_NOP;
//		NRF_Write(FLUSH_TX,&val,1);
//	}
//
//	val = (1 << RX_DR) | (1 << TX_DS) | (1 << MAX_RT);
//	NRF_Write(NRF_STATUS,&val,1);
//    //flush rx and tx
//
//
////	uint8_t val;
////	NRF_CSN_RESET;
////    val = NRF_Read(NRF_STATUS);         /*in order to check TX_FIFO status*/
////    if ((!(val & (1 << TX_FULL))))
////    {
////      current_acknowledgement_state = acknowledgement_state;      /*setting the acknowledgement state to either NO_ACK or ACK, based on input*/
////      if (dynamic_payload == ENABLE)
////        payload_width = current_payload_width;
////	  SPI2_SendData(W_TX_PAYLOAD);
////	  while (len--) {
////		SPI2_SendData((*ptr++));
////	  }
////	  while (blank_len--) {
////		SPI2_SendData(0);
////	  }
////      return (TRANSMIT_BEGIN);                                     /*TX FIFO is not full and nrf24l01+ mode is standby ii or ptx*/
////    }
////    else
////    {
////      return (TRANSMIT_FAIL);            /*TX FIFO full or wrong mode*/
////    }
//}
