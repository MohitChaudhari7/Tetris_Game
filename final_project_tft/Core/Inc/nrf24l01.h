#include <stdint.h>
#include <stdbool.h>

void NRF_Init();
void NRF_OpenReadingPipe();
void NRF_StartListening();
bool NRF_Avaiable();
void NRF_ReceiveData(char* ptr,uint8_t len);
void NRF_OpenWritingPipe();
void NRF_StopListening();
void NRF_SendData(char* ptr,uint8_t len);
