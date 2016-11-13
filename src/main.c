/**
  ******************************************************************************
  * @file    main.c
  * @author  Ac6
  * @version V1.0
  * @date    01-December-2013
  * @brief   Default main function.
  ******************************************************************************
*/


#include <stdio.h>
#include "bsp.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include "sdio_sdcard.h"
#include "ff.h"
#include "Q_fatfs.h"
#include "Audio.h"


#include "vs1003.h"
#include "MP3header.h"

#define VET_CORE     1

#if 0
/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "misc.h"
#include "bsp.h"
#include "mp3play.h"
#include "sdio_sdcard.h"
/* Library includes. */
#include "stm32f10x_it.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_usart.h"
#endif
#ifdef VET_CORE
#define LED0_ON()   GPIO_ResetBits(GPIOB,GPIO_Pin_9)
#define LED0_OFF()  GPIO_SetBits(GPIOB,GPIO_Pin_9)
#define LED0_BLINK()     GPIOB->ODR ^= GPIO_Pin_9
#else
#define LED0_ON()   GPIO_ResetBits(GPIOC,GPIO_Pin_2)
#define LED0_OFF()  GPIO_SetBits(GPIOC,GPIO_Pin_2)
#define LED0_BLINK()     GPIOC->ODR ^= GPIO_Pin_2
#endif
#if 0
#define LED1_ON()   GPIO_ResetBits(GPIOD,GPIO_Pin_13);
#define LED1_OFF()  GPIO_SetBits(GPIOD,GPIO_Pin_13);
#define LED1B()     GPIOD->ODR ^= 0x013;


FATFS FatFs;
FIL fil;
#endif


QueueHandle_t mp3DataQueue;
QueueHandle_t mp3PathQueue;
SemaphoreHandle_t mp3DecodeCompleteSem ;
SemaphoreHandle_t mp3DecodeStopSem;
SemaphoreHandle_t mp3DecodePauseSem;
SemaphoreHandle_t mp3DecodeContinueSem;

static void prvSetupHardware( void );

static void vLEDTask( void *pvParameters );
static void vBTNTask( void *pvParameters );
static void vMP3DecodeTask(void *pvParameters);
static void vVS1003Task(void *pvParameters);
//static void vCOMMTask( void *pvParameters );

extern AUDIO_Playback_status_enum AUDIO_Playback_status;
extern void retarget_init(void);

void Led_Init(void);
//void Comm_Init(void);

#if 0
Q_FATFS_Size_t size;
uint8_t st;
UINT br;
char mydiskPath[4];
uint32_t wbytes;
uint8_t wtext[]= "ok,This is text to write logical disk .2016-1-15.";
#endif

int main( void )
{

	prvSetupHardware();

	// create objects


	mp3DecodeCompleteSem = xSemaphoreCreateBinary();
	if(mp3DecodeCompleteSem == NULL) return 0;
	mp3DecodePauseSem = xSemaphoreCreateBinary();
	if(mp3DecodePauseSem == NULL) return 0;
	mp3DecodeContinueSem = xSemaphoreCreateBinary();
	if(mp3DecodeContinueSem == NULL) return 0;
	mp3DecodeStopSem = xSemaphoreCreateBinary();
	if(mp3DecodeStopSem == NULL) return 0;

	mp3DataQueue = xQueueCreate(1,sizeof(uint8_t * ));
	if(mp3DataQueue == NULL)
		return 0;
	mp3PathQueue = xQueueCreate(1,sizeof(char *));
	if(mp3PathQueue == NULL)
		return 0;


	// create tasks
	xTaskCreate( vLEDTask,  "LED",64, NULL, 2, NULL );
	xTaskCreate( vBTNTask, "BTN",512,NULL,2,NULL);
	xTaskCreate(vMP3DecodeTask,"MP3",1024,NULL,2,NULL);
	xTaskCreate(vVS1003Task,"VS1003",2046,NULL,2,NULL);

       //xTaskCreate( vCOMMTask, "COMM", 2046, NULL, 2, NULL );

	//xQueueSend(mp3PathQueue,(void *)"/1.mp3",100);
	SearchMusic("");
	/* Start the scheduler. */
	vTaskStartScheduler();


	while(1);
	return 0;
}
/*-----------------------------------------------------------*/
//void vAutoPlayTask(void *pvParmeters)
//{
//	char *file_path = "/1.mp3";
//	if(AUDIO_Playback_status == NO_SOUND)
//	{
//		xQueueSend(mp3PathQueue,(void *)file_path,100);
//	}
//}

void vBTNTask(void *pvParameters)
{
	static dlink p;
	char file_path[15]={0};
	char *tmp;
	BaseType_t result;
	GPIO_InitTypeDef GPIO_InitStructure ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	GPIO_Init(GPIOA,&GPIO_InitStructure);

	//printf("[btn]\r\n");
	p= PlayFile;
	for(;;)
	{
		if( !(GPIOA->IDR & GPIO_Pin_11) )
		{
			if(p!=NULL)
			{
				memset(file_path,0,sizeof(file_path));
				tmp = strchr((char *)p->filename,' ');
				file_path[0]='/';
				strncpy(file_path+1,p->filename,strlen(p->filename) - strlen(tmp));
				tmp = file_path;
				if(AUDIO_Playback_status == IS_PLAYING)
				{
					result = xSemaphoreGive(mp3DecodeStopSem);
					printf("stop\r\n");
				}
				//else
				{
					result = xQueueSend(mp3PathQueue,&tmp,10);
					printf("send path");
				}
				//*(file_path + 1 + strlen(p->filename) - strlen(tmp))='';
				//strlen(p->filename) - strlen(tmp);
				if(result == pdTRUE)
				{
					printf("[btn_task] ok!\r\n");
					p=p->next;
				}
				else
				{
					printf("[btn_task] error!\r\n");
				}
				vTaskDelay(200/portTICK_RATE_MS);
			}
			else
			{
				p = PlayFile;
			}
		}


	}


}
void vLEDTask( void *pvParameters )
{
    Led_Init();
    LED0_ON();

	for( ;; )
	{

        LED0_BLINK();

        vTaskDelay(500/portTICK_RATE_MS);
	}
}

void vMP3DecodeTask(void *pvParameters)
{
	char    *RxedMessage;
	BaseType_t xResult;
	//printf("[mp3]\r\n");
	for(;;)
	{
		//RxedMessage = (char*)OSMboxPend(mp3Mbox,0,&err);
		xResult = xQueueReceive(mp3PathQueue,&RxedMessage,portMAX_DELAY);
		if(xResult == pdTRUE)
		{
			printf("-- start decode music %s \r\n",RxedMessage);
			PlayAudioFile( &mp3FileObject , RxedMessage );
		}

	}

}

void vVS1003Task(void *pvParameters)
{
	uint8_t    *ReadData;
	BaseType_t xResult;
	printf("vs1003 initing...\r\n");
	VS1003_Init();
	VS1003_Reset();
	VS1003_SoftReset();
	printf("vs1003 inited\r\n");

 	for(;;)
   	{
	   xResult = xQueueReceive(mp3DataQueue,&ReadData,portMAX_DELAY);//(char*)OSMboxPend(mp3DataQueue,0,&err);
       if( xResult == pdTRUE )
       {
    	   //printf("vs1003 received\r\n");
     	   for(uint8_t index = 0; index < 16; index ++ )
    	   {
	    	   while(  MP3_DREQ == 0 );     /* 等待空闲 */
	    	   VS1003_WriteData( (void*)( ReadData + index * 32 ) );
 	       }
	   }
       else
    	   printf("vs1003 received error\r\n");
//      xResult = xSemaphoreTake(mp3DecodePauseSem,pdMS_TO_TICKS(10));
//       if( xResult == pdTRUE )
//	   {
//	       xSemaphoreTake(mp3DecodeContinueSem,portMAX_DELAY);//OSSemPend(MP3Continue,0,&err);
//	   }
	  xSemaphoreGive(mp3DecodeCompleteSem);// OSSemPost(MP3DecodeComplete);	    /* 发送解码完成信号量 */
    }
}

#if 0
void vCOMMTask( void *pvParameters )
{
  /*    Comm_Init();
    for( ;; )
	{
        USART_SendData(USART1,'@');
        vTaskDelay(1000/portTICK_RATE_MS);
    }
  */
  /*
  MP3_Start();
  MP3_Play();
  */
/*
  Q_FATFS_Size_t size;
  uint8_t st;
  UINT br;
  char mydiskPath[4];
  uint32_t wbytes;
  uint8_t wtext[]= "ok,This is text to write logical disk .2016-1-15.";
*/
  char p[64];
  Q_FATFS_Search_t file_s;
  retarget_init();
  SDIO_Configuration();
  printf(" ==== This is a Demo =====\n");

/* if((st = f_mount(&FatFs, "SD:",1)) == FR_OK)
 {
   printf("Mount OK! \n");
   st= f_open(&fil, "SD:my_file_sd.txt", FA_CREATE_ALWAYS | FA_WRITE);
   printf("Open file OK.%d.\n",st);
   st= f_write(&fil,wtext,sizeof(wtext),(void *)&wbytes);
   printf("write string to file OK!\n");
   Q_FATFS_GetDriveSize("SD:",&size);
   printf("The size of this SDCARD is:%ld, and the free size is: %ld.\n",size.TotalSize,size.FreeSize);
   Q_FATFS_Search("SD:",p,64,&file_s);
   printf("the files is :%s\r\n",p);
   printf("the num is :%ld/%ld\r\n",file_s.FoldersCount,file_s.FilesCount);
   f_close(&fil);
   f_mount(0,"SD:",1);
 }
 else
   printf("Wrong.%d\n",st);*/

  VS1003_SPI_Init();
 // printf("lll\r\n");
  MP3_Start();
  MP3_Play();
 printf("======== End =========\n");

  for(;;)
  {
    printf("test \r\n");
    vTaskDelay(1000/portTICK_RATE_MS);
  }
}
#endif
/*-----------------------------------------------------------*/


static void prvSetupHardware( void )
{
    //SystemInit();
  bsp_Init();
  retarget_init();
  SDIO_Configuration();

  //VS1003_SPI_Init();

}
/*-----------------------------------------------------------*/

void Led_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
#ifdef VET_CORE
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_Init(GPIOB,&GPIO_InitStructure);
#else
    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOC , ENABLE );
    /*LED0 @ PC2*/
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_Init( GPIOC, &GPIO_InitStructure );
#endif


}
