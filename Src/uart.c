#include "uart.h"
#include "serial_receive.h"

#ifndef __stdio_h
#undef __stdio_h
//#include <stdio.h>
#endif

UART_HandleTypeDef UartHandle;
/* UART handler declaration */
__IO ITStatus UartReady = RESET;

/* Buffer used for transmission */


/* Buffer used for reception */
uint8_t aRxBuffer;
extern int SerialReceiveReady;
extern int receive_count;
extern int RxLength;
extern int TxLength;
extern int serial_data[32] ;
extern int state;
extern int mode;

char dispvar[150]={0};
char errvar[150]={0};

enum STATE{IDLE=0, CTRL, COM};
enum MODE{TEST=0, MANUAL, AUTOMATIC};

void uart_config(void)
{
  /* Configure the UART peripheral ######################################*/
  /* Put the USART peripheral in the Asynchronous mode (UART Mode) */
  /* UART configured as follows:
      - Word Length = 8 Bits
      - Stop Bit = One Stop bit
      - Parity = None
      - BaudRate = 9600 baud
      - Hardware flow control disabled (RTS and CTS signals) */
  UartHandle.Instance        = USARTx;

  UartHandle.Init.BaudRate   = 9600;
  UartHandle.Init.WordLength = UART_WORDLENGTH_8B;
  UartHandle.Init.StopBits   = UART_STOPBITS_1;
  UartHandle.Init.Parity     = UART_PARITY_NONE;
  UartHandle.Init.HwFlowCtl  = UART_HWCONTROL_NONE;
  UartHandle.Init.Mode       = UART_MODE_TX_RX;
  if(HAL_UART_DeInit(&UartHandle) != HAL_OK)
  {
		sprintf(errvar,"This is line %d of file %s (function %s)\n", __LINE__, __FILE__, __func__);
    Error_Handler(101); // See "error codes.txt"
  }  
  if(HAL_UART_Init(&UartHandle) != HAL_OK)
  {
		sprintf(errvar,"This is line %d of file %s (function %s)\n", __LINE__, __FILE__, __func__);
    Error_Handler(102); // See "error codes.txt"
  }
	/* Put UART peripheral in reception process */ 
	if(HAL_UART_Receive_IT(&UartHandle, &aRxBuffer, 1) != HAL_OK)
  {
		sprintf(errvar,"This is line %d of file %s (function %s)\n", __LINE__, __FILE__, __func__);
    Error_Handler(103); // See "error codes.txt"
  }
	#ifdef DEBUG_MODE
		sprintf(dispvar,"UART = USART%d \n BAUDRATE = %d \n Word Length = 8 bits \n Stop Bit = 1 \nParity = None \n Flow Control = None \n Mode = TxRx ", 2,UartHandle.Init.BaudRate );
		serial_printf();
	#endif
}

/**
  * @brief  Tx Transfer completed callback
  * @param  UartHandle: UART handle. 
  * @note   This example shows a simple way to report end of IT Tx transfer, and 
  *         you can add your own implementation. 
  * @retval None
  */
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *UartHandle)
{
  /* Set transmission flag: transfer complete */
  UartReady = SET;
}

/**
  * @brief  Rx Transfer completed callback
  * @param  UartHandle: UART handle
  * @note   This example shows a simple way to report end of DMA Rx transfer, and 
  *         you can add your own implementation.
  * @retval None
  */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *UartHandle)
{ 
#ifndef DEBUG_MODE
if (state==COM)
{
#endif
  /* Set reception flag: transfer complete */
  UartReady = SET;
	serial_data[receive_count]= aRxBuffer;
	#ifdef DEBUG_MODE
	if(serial_data[0] == 's')
		state = IDLE;
	#endif
	if(receive_count == 3){
		if(serial_data[0] != REQ){
			receive_count=0;
			return;
		}
		else
			RxLength= serial_data[3];
	}
	if(receive_count == RxLength + 3)
		SerialReceiveReady = SET;
	else
		receive_count++;
		/* Put UART peripheral in reception process */ 
	if(HAL_UART_Receive_IT(UartHandle, &aRxBuffer, 1) != HAL_OK)
  {
		sprintf(errvar,"This is line %d of file %s (function %s)\n", __LINE__, __FILE__, __func__);
    Error_Handler(1031); // See "error codes.txt"
  }
	UartReady = RESET;
#ifndef DEBUG_MODE
}
#endif
}
/**
  * @brief  UART error callbacks
  * @param  UartHandle: UART handle
  * @note   This example shows a simple way to report transfer error, and you can
  *         add your own implementation.
  * @retval None
  */
void HAL_UART_ErrorCallback(UART_HandleTypeDef *UartHandle)
{
	  sprintf(errvar,"This is line %d of file %s (function %s)\n", __LINE__, __FILE__, __func__);
    Error_Handler(104); // See "error codes.txt"
}

void uart_transmit(char aTxBuffer[35], uint32_t BuffSize)
{
	if(HAL_UART_Transmit_IT(&UartHandle, (uint8_t*)aTxBuffer, BuffSize)!= HAL_OK)
  {
		sprintf(errvar,"This is line %d of file %s (function %s)\n", __LINE__, __FILE__, __func__);
    Error_Handler(105); // See "error codes.txt"
  }
	while (UartReady != SET)
  {
  }
  
  /* Reset transmission flag */
  UartReady = RESET;

}

int string_length(char s[]) {
   int c = 0;
 
   while (s[c] != '\0')
      c++;
 
   return c;
}

void ResetRxIT(UART_HandleTypeDef *UartHandle)
{
	/* Put UART peripheral in reception process */ 
	if(HAL_UART_Receive_IT(UartHandle, &aRxBuffer, 1) != HAL_OK)
  {
		sprintf(errvar,"This is line %d of file %s (function %s)\n", __LINE__, __FILE__, __func__);
    Error_Handler(1032); // See "error codes.txt"
  }
}

void serial_printf()
{
 
  /* e.g. write a character to the USART1 and Loop until the end of transmission */
  HAL_UART_Transmit(&UartHandle, (uint8_t *)dispvar, COUNTOF(dispvar), 0xFFFF);

}
