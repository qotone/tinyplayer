#include <stdint.h>
#include "stm32f10x.h"

int __io_putchar(int ch)
{
	// 将Printf内容发往串口
		USART_SendData(USART1, (unsigned char) ch);
	//	while (!(USART1->SR & USART_FLAG_TXE));
		while( USART_GetFlagStatus(USART1,USART_FLAG_TC)!= SET);
		return (ch);

}
void retarget_init()
{
  // Initialize UART
  GPIO_InitTypeDef GPIO_InitStruct;
  USART_InitTypeDef USART_InitStruct;

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);

  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStruct);
  
  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOA,&GPIO_InitStruct);
  
  USART_InitStruct.USART_BaudRate = 115200;
  USART_InitStruct.USART_WordLength = USART_WordLength_8b;
  USART_InitStruct.USART_StopBits = USART_StopBits_1;
  USART_InitStruct.USART_Parity = USART_Parity_No;
  USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_Init(USART1,&USART_InitStruct);
  USART_Cmd(USART1, ENABLE);
}

//int _write (int fd, char *ptr, int len)
//{
//  /* Write "len" of char from "ptr" to file id "fd"
//   * Return number of char written.
//   * Need implementing with UART here. */
//  int i = 0;
//  //  uint8_t temp = '\r';
//  while(*ptr && (i<len)){
//    while((USART1->SR & USART_FLAG_TXE) == 0);
//    USART1->DR = *ptr & 0x1FF;
//    //    USART_SendData(USART1,*ptr);
//
//    //    while(USART_GetFlagStatus(USART1,USART_FLAG_TC) == RESET);
//    //    HAL_UART_Transmit(&serialHandle,(uint8_t *)ptr,1,0xFFFF);
//    if(*ptr == '\n'){
//      //HAL_UART_Transmit(&serialHandle,&temp,1,0xFFFF);
//      //      USART_SendData(USART1,'\r');
//      //      while(USART_GetFlagStatus(USART1,USART_FLAG_TC) == RESET);
//      while((USART1->SR & USART_FLAG_TXE) == 0);
//      USART1->DR = '\r' & 0x1FF;
//    }
//    i++;
//    ptr++;
//  }
//  return i;
//}

//int _read (int fd, char *ptr, int len)
//{
//   Read "len" of char to "ptr" from file id "fd"
//   * Return number of char read.
//   * Need implementing with UART here.
//  //  HAL_UART_Receive(&serialHandle,(uint8_t *)ptr,1,0xFFFF);
//
//  //  while(!(USART1->SR & USART_FLAG_RXNE));
//  //*ptr =  USART_ReceiveData(USART1);
//  while ( ( USART1->SR & USART_FLAG_RXNE) == 0) ;
//  *ptr = USART1->DR & 0x1FF;
//
//  return 1;
//}

void _ttywrch(int ch) {
  /* Write one char "ch" to the default console
   * Need implementing with UART here. */
  //  HAL_UART_Transmit(&serialHandle,(uint8_t *)&ch,1,0xFFFF);
  //  USART_SendData(USART1, ch);
  //while(USART_GetFlagStatus(USART1,USART_FLAG_TC) == RESET);
    while((USART1->SR & USART_FLAG_TXE) == 0);
    USART1->DR = ch & 0x1FF;

}

/* SystemInit will be called before main */
/*
void SystemInit()
{
    retarget_init();
    }*/

