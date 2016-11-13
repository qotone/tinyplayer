#include "hw_config.h"
#include "stm32f10x_pwr.h"

GPIO_TypeDef* BUTTON_GPIO_PORT[]= {BUTTON1_GPIO_PORT,BUTTON2_GPIO_PORT};
const uint16_t BUTTON_GPIO_PIN[] = {BUTTON1_GPIO_PIN, BUTTON2_GPIO_PIN};
const uint32_t BUTTON_GPIO_CLK[] = {BUTTON1_GPIO_CLK, BUTTON2_GPIO_CLK};
GPIOMode_TypeDef BUTTON_GPIO_MODE[] = {BUTTON1_GPIO_MODE, BUTTON2_GPIO_MODE};
__IO uint16_t BUTTON_DEBOUNCED_TIME[] = {0, 0};

const uint16_t BUTTON_EXTI_LINE[] = {BUTTON1_EXTI_LINE, BUTTON2_EXTI_LINE};
const uint16_t BUTTON_GPIO_PORT_SOURCE[] = {BUTTON1_EXTI_PORT_SOURCE, BUTTON2_EXTI_PORT_SOURCE};
const uint16_t BUTTON_GPIO_PIN_SOURCE[] = {BUTTON1_EXTI_PIN_SOURCE, BUTTON2_EXTI_PIN_SOURCE};
const uint16_t BUTTON_IRQn[] = {BUTTON1_EXTI_IRQn, BUTTON2_EXTI_IRQn};
EXTITrigger_TypeDef BUTTON_EXTI_TRIGGER[] = {BUTTON1_EXTI_TRIGGER, BUTTON2_EXTI_TRIGGER};



void NVIC_Configuration(void)
{
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
}


void SysTick_Configuration(void)
{
  if(SysTick_Config(SystemCoreClock / 1000))
  {
    while(1)
    {
      //
    }
  }
  NVIC_SetPriority(SysTick_IRQn,SYSTICK_IRQ_PRIORITY);
}

void Set_System(void)
{
  /* Enable for RTC use */
  // Enable PWR and BKP clock;
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP,ENABLE);
  // Enable write access to Backup domain
  PWR_BackupAccessCmd(ENABLE);

  // DWT_Init();
  // init NVIC 
  NVIC_Configuration();


  // ToDo: something init for system 
}


void OLED_IO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  SPI_InitTypeDef SPI_InitStructure;
  /* OLED Control Pins configure */
  RCC_APB2PeriphClockCmd(OLED_CS_GPIO_CLK | OLED_DC_GPIO_CLK | OLED_RST_GPIO_CLK, ENABLE);
  GPIO_InitStructure.GPIO_Pin = OLED_CS_GPIO_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(OLED_CS_GPIO_PORT,&GPIO_InitStructure);

  OLED_RST_LOW();  // add a reset
  OLED_CS_HIGH();

  GPIO_InitStructure.GPIO_Pin = OLED_RST_GPIO_PIN;
  GPIO_Init(OLED_RST_GPIO_PORT,&GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = OLED_DC_GPIO_PIN;
  GPIO_Init(OLED_DC_GPIO_PORT,&GPIO_InitStructure);
  /* SPI Configure */
  RCC_APB2PeriphClockCmd(OLED_SPI_SCK_GPIO_CLK | OLED_SPI_MOSI_GPIO_CLK,ENABLE );
  OLED_SPI_CLK_CMD(OLED_SPI_CLK, ENABLE);
  // Configure OLED_MOSI pins: mosi
  GPIO_InitStructure.GPIO_Pin = OLED_SPI_MOSI_GPIO_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(OLED_SPI_MOSI_GPIO_PORT,&GPIO_InitStructure);
  // Configure OLED_SCK pins: sck
  GPIO_InitStructure.GPIO_Pin = OLED_SPI_SCK_GPIO_PIN;
  GPIO_Init(OLED_SPI_SCK_GPIO_PORT,&GPIO_InitStructure);

  SPI_InitStructure.SPI_BaudRatePrescaler = OLED_SPI_BAUDRATE_PRESCALER;
  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
  //  SPI_InitStructure.TIMode = SPI_TIMODE_DISABLE;
  //  SPI_InitStructure.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  SPI_InitStructure.SPI_CRCPolynomial = 7;
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
  SPI_Init(OLED_SPI,&SPI_InitStructure);

  SPI_Cmd(OLED_SPI, ENABLE);
  OLED_RST_HIGH(); // now reset end begin work oled ...
}


uint8_t OLED_SPI_Write(uint8_t data)
{
  while(SPI_I2S_GetFlagStatus(OLED_SPI, SPI_I2S_FLAG_TXE) == RESET) ;

  SPI_I2S_SendData(OLED_SPI,data);

  while(SPI_I2S_GetFlagStatus(OLED_SPI, SPI_I2S_FLAG_RXNE) == RESET);

  return SPI_I2S_ReceiveData(OLED_SPI);
}


/**
 * @brief  Configures Button GPIO, EXTI Line and DEBOUNCE Timer.
 * @param  Button: Specifies the Button to be configured.
 *   This parameter can be one of following parameters:
 *     @arg BUTTON1: Button1
 *     @arg BUTTON2: Button2
 * @param  Button_Mode: Specifies Button mode.
 *   This parameter can be one of following parameters:
 *     @arg BUTTON_MODE_GPIO: Button will be used as simple IO
 *     @arg BUTTON_MODE_EXTI: Button will be connected to EXTI line with interrupt
 *                     generation capability
 * @retval None
 */
void BUTTON_Init(Button_TypeDef Button, ButtonMode_TypeDef Button_Mode)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    /* Enable the BUTTON Clock */
    RCC_APB2PeriphClockCmd(BUTTON_GPIO_CLK[Button] | RCC_APB2Periph_AFIO, ENABLE);

    /* Configure Button pin as input floating */
    GPIO_InitStructure.GPIO_Mode = BUTTON_GPIO_MODE[Button];
    GPIO_InitStructure.GPIO_Pin = BUTTON_GPIO_PIN[Button];
    GPIO_Init(BUTTON_GPIO_PORT[Button], &GPIO_InitStructure);

    if (Button_Mode == BUTTON_MODE_EXTI)
    {
        /* Disable TIM1 CC4 Interrupt */
        TIM_ITConfig(TIM1, TIM_IT_CC4, DISABLE);

        /* Enable the TIM1 Interrupt */
        NVIC_InitStructure.NVIC_IRQChannel = TIM1_CC_IRQn;
        NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = TIM1_CC_IRQ_PRIORITY;	//OLD: 0x02
        NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;							//OLD: 0x00
        NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

        NVIC_Init(&NVIC_InitStructure);

        /* Enable the Button EXTI Interrupt */
        NVIC_InitStructure.NVIC_IRQChannel = BUTTON_IRQn[Button];
        NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = EXTI2_IRQ_PRIORITY;		//OLD: 0x02
        NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;							//OLD: 0x01
        NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

        NVIC_Init(&NVIC_InitStructure);

        BUTTON_EXTI_Config(Button, ENABLE);
    }
}

void BUTTON_EXTI_Config(Button_TypeDef Button, FunctionalState NewState)
{
    EXTI_InitTypeDef EXTI_InitStructure;

    /* Connect Button EXTI Line to Button GPIO Pin */
    GPIO_EXTILineConfig(BUTTON_GPIO_PORT_SOURCE[Button], BUTTON_GPIO_PIN_SOURCE[Button]);

    /* Clear the EXTI line pending flag */
    EXTI_ClearFlag(BUTTON_EXTI_LINE[Button]);

    /* Configure Button EXTI line */
    EXTI_InitStructure.EXTI_Line = BUTTON_EXTI_LINE[Button];
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = BUTTON_EXTI_TRIGGER[Button];
    EXTI_InitStructure.EXTI_LineCmd = NewState;
    EXTI_Init(&EXTI_InitStructure);
}

/**
 * @brief  Returns the selected Button non-filtered state.
 * @param  Button: Specifies the Button to be checked.
 *   This parameter can be one of following parameters:
 *     @arg BUTTON1: Button1
 *     @arg BUTTON2: Button2
 * @retval Actual Button Pressed state.
 */
uint8_t BUTTON_GetState(Button_TypeDef Button)
{
    return GPIO_ReadInputDataBit(BUTTON_GPIO_PORT[Button], BUTTON_GPIO_PIN[Button]);
}

/**
 * @brief  Returns the selected Button Debounced Time.
 * @param  Button: Specifies the Button to be checked.
 *   This parameter can be one of following parameters:
 *     @arg BUTTON1: Button1
 *     @arg BUTTON2: Button2
 * @retval Button Debounced time in millisec.
 */
uint16_t BUTTON_GetDebouncedTime(Button_TypeDef Button)
{
    return BUTTON_DEBOUNCED_TIME[Button];
}

void BUTTON_ResetDebouncedState(Button_TypeDef Button)
{
    BUTTON_DEBOUNCED_TIME[Button] = 0;
}
