/*
 * peripherals.c
 *
 *  Created on: Sep. 26, 2020
 *      Author: Alka
 */

// PERIPHERAL SETUP

#include "peripherals.h"

#include "ADC.h"
#include "serial_telemetry.h"
#include "targets.h"

void initCorePeripherals(void) {
  SystemClock_Config();
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_TIM1_Init();
  MX_TIM3_Init();
  MX_COMP1_Init();
  MX_TIM6_Init();
  MX_TIM16_Init();
  MX_TIM17_Init();
// PWM
  //MX_TIM2_Init();
  UN_TIM_Init();
#ifdef USE_SERIAL_TELEMETRY
  telem_UART_Init();
#endif
}

void initAfterJump(void) {
  SCB->VTOR = 0x08001000;

  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SYSCFG);
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);

  /* System interrupt init*/
  NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);

  /* Remap SRAM at 0x00000000 */
  do {
    ((SYSCFG_TypeDef *)(((uint32_t)0x40000000U) + 0x00010000))->CFGR1 &=
        ~((0x3U << (0U)));
    ((SYSCFG_TypeDef *)(((uint32_t)0x40000000U) + 0x00010000))->CFGR1 |=
        ((0x1U << (0U)) | (0x2U << (0U)));
  } while (0);

  __enable_irq();
}

void SystemClock_Config(void) {
  LL_FLASH_SetLatency(LL_FLASH_LATENCY_2);
  while (LL_FLASH_GetLatency() != LL_FLASH_LATENCY_2) {
  }
  LL_RCC_HSE_Enable();

  /* Wait till HSE is ready */
  while (LL_RCC_HSE_IsReady() != 1) {
  }
  LL_RCC_LSI_Enable();

  /* Wait till LSI is ready */
  while (LL_RCC_LSI_IsReady() != 1) {
  }
  LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSE, LL_RCC_PLL_MUL_6,
                              LL_RCC_PREDIV_DIV_1);
  LL_RCC_PLL_Enable();

  /* Wait till PLL is ready */
  while (LL_RCC_PLL_IsReady() != 1) {
  }
  LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
  LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_2);
  LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);
  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);

  /* Wait till System clock is ready */
  while (LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL) {
  }
  LL_Init1msTick(72000000);
  LL_SetSystemCoreClock(72000000);
  LL_RCC_SetUSARTClockSource(LL_RCC_USART1_CLKSOURCE_PCLK2);
  LL_RCC_SetTIMClockSource(LL_RCC_TIM1_CLKSOURCE_PCLK2);
  LL_RCC_SetTIMClockSource(LL_RCC_TIM17_CLKSOURCE_PCLK2);
  LL_RCC_SetTIMClockSource(LL_RCC_TIM15_CLKSOURCE_PCLK2);
  LL_RCC_SetTIMClockSource(LL_RCC_TIM2_CLKSOURCE_PCLK1);
  LL_RCC_SetTIMClockSource(LL_RCC_TIM34_CLKSOURCE_PCLK1);
  LL_RCC_SetADCClockSource(LL_RCC_ADC12_CLKSRC_PLL_DIV_1);
}

void MX_COMP1_Init(void) {
  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);
  /**COMP1 GPIO Configuration
  PA1   ------> COMP1_INP
  PA5   ------> COMP1_INM
  */
  GPIO_InitStruct.Pin = LL_GPIO_PIN_1;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = LL_GPIO_PIN_5;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  NVIC_SetPriority(COMP1_2_IRQn, 0);
  NVIC_EnableIRQ(COMP1_2_IRQn);
}

void MX_IWDG_Init(void) {
  IWDG->KR = 0x0000CCCCU;
  IWDG->KR = 0x00005555U;
  IWDG->PR = LL_IWDG_PRESCALER_16;
  IWDG->RLR = 4000;
  LL_IWDG_ReloadCounter(IWDG);
}

void MX_TIM1_Init(void) {
  LL_TIM_InitTypeDef TIM_InitStruct = {0};
  LL_TIM_OC_InitTypeDef TIM_OC_InitStruct = {0};
  LL_TIM_BDTR_InitTypeDef TIM_BDTRInitStruct = {0};
  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_TIM1);

  TIM_InitStruct.Prescaler = 0;
  TIM_InitStruct.CounterMode = LL_TIM_COUNTERMODE_UP;
  TIM_InitStruct.Autoreload = 3000 - 1;
  TIM_InitStruct.ClockDivision = LL_TIM_CLOCKDIVISION_DIV1;
  TIM_InitStruct.RepetitionCounter = 0;
  LL_TIM_Init(TIM1, &TIM_InitStruct);
  LL_TIM_EnableARRPreload(TIM1);
  LL_TIM_SetClockSource(TIM1, LL_TIM_CLOCKSOURCE_INTERNAL);
  LL_TIM_OC_EnablePreload(TIM1, LL_TIM_CHANNEL_CH1);
#ifdef USE_SWAPPED_OUPUT
  TIM_OC_InitStruct.OCMode = LL_TIM_OCMODE_PWM2;
#else
  TIM_OC_InitStruct.OCMode = LL_TIM_OCMODE_PWM1;
#endif
  TIM_OC_InitStruct.OCState = LL_TIM_OCSTATE_DISABLE;
  TIM_OC_InitStruct.OCNState = LL_TIM_OCSTATE_DISABLE;
  TIM_OC_InitStruct.CompareValue = 0;
#ifdef USE_INVERTED_HIGH
  TIM_OC_InitStruct.OCPolarity = LL_TIM_OCPOLARITY_LOW;
  TIM_OC_InitStruct.OCIdleState = LL_TIM_OCIDLESTATE_HIGH;
#else
  TIM_OC_InitStruct.OCPolarity = LL_TIM_OCPOLARITY_HIGH;
  TIM_OC_InitStruct.OCIdleState = LL_TIM_OCIDLESTATE_LOW;
#endif
#ifdef USE_INVERTED_LOW
  TIM_OC_InitStruct.OCNPolarity = LL_TIM_OCPOLARITY_LOW;
  TIM_OC_InitStruct.OCNIdleState = LL_TIM_OCIDLESTATE_HIGH;
#else
  TIM_OC_InitStruct.OCNPolarity = LL_TIM_OCPOLARITY_HIGH;
  TIM_OC_InitStruct.OCNIdleState = LL_TIM_OCIDLESTATE_LOW;
#endif
  LL_TIM_OC_Init(TIM1, LL_TIM_CHANNEL_CH1, &TIM_OC_InitStruct);
  LL_TIM_OC_DisableFast(TIM1, LL_TIM_CHANNEL_CH1);

  LL_TIM_OC_EnablePreload(TIM1, LL_TIM_CHANNEL_CH2);
  TIM_OC_InitStruct.OCState = LL_TIM_OCSTATE_DISABLE;
  TIM_OC_InitStruct.OCNState = LL_TIM_OCSTATE_DISABLE;
  LL_TIM_OC_Init(TIM1, LL_TIM_CHANNEL_CH2, &TIM_OC_InitStruct);
  LL_TIM_OC_DisableFast(TIM1, LL_TIM_CHANNEL_CH2);

  LL_TIM_OC_EnablePreload(TIM1, LL_TIM_CHANNEL_CH3);
  TIM_OC_InitStruct.OCState = LL_TIM_OCSTATE_DISABLE;
  TIM_OC_InitStruct.OCNState = LL_TIM_OCSTATE_DISABLE;
  LL_TIM_OC_Init(TIM1, LL_TIM_CHANNEL_CH3, &TIM_OC_InitStruct);
  LL_TIM_OC_DisableFast(TIM1, LL_TIM_CHANNEL_CH3);

  LL_TIM_OC_EnablePreload(TIM1, LL_TIM_CHANNEL_CH4);
  TIM_OC_InitStruct.OCState = LL_TIM_OCSTATE_DISABLE;
  TIM_OC_InitStruct.OCNState = LL_TIM_OCSTATE_DISABLE;
  LL_TIM_OC_Init(TIM1, LL_TIM_CHANNEL_CH4, &TIM_OC_InitStruct);
  LL_TIM_OC_DisableFast(TIM1, LL_TIM_CHANNEL_CH4);

  LL_TIM_SetTriggerOutput(TIM1, LL_TIM_TRGO_RESET);
  LL_TIM_SetTriggerOutput2(TIM1, LL_TIM_TRGO2_RESET);
  LL_TIM_DisableMasterSlaveMode(TIM1);
  TIM_BDTRInitStruct.OSSRState = LL_TIM_OSSR_DISABLE;
  TIM_BDTRInitStruct.OSSIState = LL_TIM_OSSI_DISABLE;
  TIM_BDTRInitStruct.LockLevel = LL_TIM_LOCKLEVEL_OFF;
  TIM_BDTRInitStruct.DeadTime = DEAD_TIME;
  TIM_BDTRInitStruct.BreakState = LL_TIM_BREAK_DISABLE;
  TIM_BDTRInitStruct.BreakPolarity = LL_TIM_BREAK_POLARITY_HIGH;
  TIM_BDTRInitStruct.AutomaticOutput = LL_TIM_AUTOMATICOUTPUT_DISABLE;
  LL_TIM_BDTR_Init(TIM1, &TIM_BDTRInitStruct);

  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOC);
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB);
  /**TIM1 GPIO Configuration
  PC0   ------> TIM1_CH1
  PC1   ------> TIM1_CH2
  PC2   ------> TIM1_CH3
  PA7   ------> TIM1_CH1N
  PB0   ------> TIM1_CH2N
  PB1   ------> TIM1_CH3N
  */
#ifdef USE_OPEN_DRAIN_LOW
#pragma message("using open drain low side")
#define LOW_OUTPUT_TYPE LL_GPIO_OUTPUT_OPENDRAIN
#else
#define LOW_OUTPUT_TYPE LL_GPIO_OUTPUT_PUSHPULL
#endif
#ifdef USE_OPEN_DRAIN_HIGH
#pragma message("using open drain high side")
#define HIGH_OUTPUT_TYPE LL_GPIO_OUTPUT_OPENDRAIN
#else
#define HIGH_OUTPUT_TYPE LL_GPIO_OUTPUT_PUSHPULL
#endif
  GPIO_InitStruct.Pin = PHASE_A_GPIO_LOW;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = LOW_OUTPUT_TYPE;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_6;
  LL_GPIO_Init(PHASE_A_GPIO_PORT_LOW, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = PHASE_B_GPIO_LOW;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = LOW_OUTPUT_TYPE;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_6;
  LL_GPIO_Init(PHASE_B_GPIO_PORT_LOW, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = PHASE_C_GPIO_LOW;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = LOW_OUTPUT_TYPE;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_6;
  LL_GPIO_Init(PHASE_C_GPIO_PORT_LOW, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = PHASE_A_GPIO_HIGH;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = HIGH_OUTPUT_TYPE;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_2;
  LL_GPIO_Init(PHASE_A_GPIO_PORT_HIGH, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = PHASE_B_GPIO_HIGH;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = HIGH_OUTPUT_TYPE;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_2;
  LL_GPIO_Init(PHASE_B_GPIO_PORT_HIGH, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = PHASE_C_GPIO_HIGH;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = HIGH_OUTPUT_TYPE;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_2;
  LL_GPIO_Init(PHASE_C_GPIO_PORT_HIGH, &GPIO_InitStruct);

  //  NVIC_SetPriority(TIM1_BRK_UP_TRG_COM_IRQn, 2);
  //  NVIC_EnableIRQ(TIM1_BRK_UP_TRG_COM_IRQn);
}

void MX_TIM3_Init(void) {
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM3);
  TIM3->PSC = 36 - 1;
  TIM3->ARR = 0xFFFF;
}

void MX_TIM6_Init(void) {
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM6);

  NVIC_SetPriority(TIM6_DAC_IRQn, 3);
  NVIC_EnableIRQ(TIM6_DAC_IRQn);
  TIM6->PSC = 72 - 1;
  TIM6->ARR = 1000000 / LOOP_FREQUENCY_HZ;
}

void MX_TIM16_Init(void) {
  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_TIM16);
  TIM16->PSC = 36 - 1;
  TIM16->ARR = 4000;
  NVIC_SetPriority(TIM16_IRQn, 0);
  NVIC_EnableIRQ(TIM16_IRQn);
  LL_TIM_EnableARRPreload(TIM16);
}

// Not sure what this controls, no reference to it in targets.h for f051.
// void MX_TIM16_Init(void) {
//  LL_APB1_GRP2_EnableClock(LL_APB1_GRP2_PERIPH_TIM16);
//  //  NVIC_SetPriority(TIM16_IRQn, 2);
//  //  NVIC_EnableIRQ(TIM16_IRQn);
//  TIM16->PSC = 0;
//  TIM16->ARR = 9000;
//  LL_TIM_DisableARRPreload(TIM16);
//}

void MX_TIM17_Init(void) {
  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_TIM17);
  TIM17->PSC = 72 - 1;
  TIM17->ARR = 0XFFFF;
  LL_TIM_DisableARRPreload(TIM17);
}

void MX_DMA_Init(void) {

  /* Init with LL driver */
  /* DMA controller clock enable */
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA1);
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA2);

  /* DMA interrupt init */
  /* DMA1_Channel1_IRQn interrupt configuration */
  NVIC_SetPriority(DMA1_Channel1_IRQn,
                   NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));
  NVIC_EnableIRQ(DMA1_Channel1_IRQn);

  /* DMA2_Channel1_IRQn interrupt configuration */
  NVIC_SetPriority(DMA2_Channel1_IRQn,
                   NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));
  NVIC_EnableIRQ(DMA2_Channel1_IRQn);
}

void MX_GPIO_Init(void) {
  LL_EXTI_InitTypeDef EXTI_InitStruct = {0};
  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */
  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOF);
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOC);
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB);

  /**/
  LL_GPIO_SetOutputPin(GATE_ENABLE_GPIO_Port, GATE_ENABLE_Pin);

  /**/
  LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_9);

  /**/
  GPIO_InitStruct.Pin = GATE_ENABLE_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(GATE_ENABLE_GPIO_Port, &GPIO_InitStruct);

  /**/
  GPIO_InitStruct.Pin = LL_GPIO_PIN_9;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /**/
  LL_SYSCFG_SetEXTISource(LL_SYSCFG_EXTI_PORTA, LL_SYSCFG_EXTI_LINE9);

  /**/
  LL_GPIO_SetPinPull(GPIOA, LL_GPIO_PIN_9, LL_GPIO_PULL_NO);

  /**/
  LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_9, LL_GPIO_MODE_INPUT);

  /**/
  EXTI_InitStruct.Line_0_31 = LL_EXTI_LINE_9;
  EXTI_InitStruct.Line_32_63 = LL_EXTI_LINE_NONE;
  EXTI_InitStruct.LineCommand = ENABLE;
  EXTI_InitStruct.Mode = LL_EXTI_MODE_IT;
  EXTI_InitStruct.Trigger = LL_EXTI_TRIGGER_RISING;
  LL_EXTI_Init(&EXTI_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */
  /* USER CODE END MX_GPIO_Init_2 */
}

void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  LL_TIM_InitTypeDef TIM_InitStruct = {0};

  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* Peripheral clock enable */
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM2);

  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);
  /**TIM2 GPIO Configuration
  PA9   ------> TIM2_CH3
  */
  GPIO_InitStruct.Pin = LL_GPIO_PIN_9;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_10;
  LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* TIM2 DMA Init */

  /* TIM2_CH3 Init */
  LL_DMA_SetDataTransferDirection(DMA1, LL_DMA_CHANNEL_1, LL_DMA_DIRECTION_PERIPH_TO_MEMORY);

  LL_DMA_SetChannelPriorityLevel(DMA1, LL_DMA_CHANNEL_1, LL_DMA_PRIORITY_LOW);

  LL_DMA_SetMode(DMA1, LL_DMA_CHANNEL_1, LL_DMA_MODE_NORMAL);

  LL_DMA_SetPeriphIncMode(DMA1, LL_DMA_CHANNEL_1, LL_DMA_PERIPH_NOINCREMENT);

  LL_DMA_SetMemoryIncMode(DMA1, LL_DMA_CHANNEL_1, LL_DMA_MEMORY_INCREMENT);

  LL_DMA_SetPeriphSize(DMA1, LL_DMA_CHANNEL_1, LL_DMA_PDATAALIGN_WORD);

  LL_DMA_SetMemorySize(DMA1, LL_DMA_CHANNEL_1, LL_DMA_MDATAALIGN_WORD);

  /* TIM2 interrupt Init */
  NVIC_SetPriority(TIM2_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),0, 0));
  NVIC_EnableIRQ(TIM2_IRQn);

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  TIM_InitStruct.Prescaler = 12;
  TIM_InitStruct.CounterMode = LL_TIM_COUNTERMODE_UP;
  TIM_InitStruct.Autoreload = 0xFFFF;
  TIM_InitStruct.ClockDivision = LL_TIM_CLOCKDIVISION_DIV1;
  LL_TIM_Init(TIM2, &TIM_InitStruct);
  LL_TIM_DisableARRPreload(TIM2);
  LL_TIM_SetTriggerOutput(TIM2, LL_TIM_TRGO_RESET);
  LL_TIM_DisableMasterSlaveMode(TIM2);
  LL_TIM_IC_SetActiveInput(TIM2, LL_TIM_CHANNEL_CH3, LL_TIM_ACTIVEINPUT_DIRECTTI);
  LL_TIM_IC_SetPrescaler(TIM2, LL_TIM_CHANNEL_CH3, LL_TIM_ICPSC_DIV1);
  LL_TIM_IC_SetFilter(TIM2, LL_TIM_CHANNEL_CH3, LL_TIM_IC_FILTER_FDIV1);
  LL_TIM_IC_SetPolarity(TIM2, LL_TIM_CHANNEL_CH3, LL_TIM_IC_POLARITY_BOTHEDGE);
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */

}

void UN_TIM_Init(void) {
  //LL_TIM_InitTypeDef TIM_InitStruct = {0};

  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

#ifdef USE_TIMER_2_CHANNEL_3
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM2);
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);
  /**TIM16 GPIO Configuration
  PA9   ------> TIM2_CH3
  */
  GPIO_InitStruct.Pin = LL_GPIO_PIN_9;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_10;
  LL_GPIO_Init(GPIOA, &GPIO_InitStruct);
#endif

  /* Peripheral clock enable */
#ifdef USE_TIMER_15_CHANNEL_1
  LL_APB1_GRP2_EnableClock(LL_APB1_GRP2_PERIPH_TIM15);
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);
  /**TIM16 GPIO Configuration
  PA6   ------> TIM16_CH1
  */
  GPIO_InitStruct.Pin = LL_GPIO_PIN_2;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_0;
  LL_GPIO_Init(GPIOA, &GPIO_InitStruct);
#endif

  /* TIM16_CH1_UP Init */
  // LL_DMA_SetDataTransferDirection(DMA1, INPUT_DMA_CHANNEL,
  //                                 LL_DMA_DIRECTION_PERIPH_TO_MEMORY);

  // LL_DMA_SetChannelPriorityLevel(DMA1, INPUT_DMA_CHANNEL, LL_DMA_PRIORITY_LOW);

  // LL_DMA_SetMode(DMA1, INPUT_DMA_CHANNEL, LL_DMA_MODE_NORMAL);

  // LL_DMA_SetPeriphIncMode(DMA1, INPUT_DMA_CHANNEL, LL_DMA_PERIPH_NOINCREMENT);

  // LL_DMA_SetMemoryIncMode(DMA1, INPUT_DMA_CHANNEL, LL_DMA_MEMORY_INCREMENT);

  // LL_DMA_SetPeriphSize(DMA1, INPUT_DMA_CHANNEL, LL_DMA_PDATAALIGN_WORD);

  // LL_DMA_SetMemorySize(DMA1, INPUT_DMA_CHANNEL, LL_DMA_MDATAALIGN_WORD);

#ifdef USE_TIMER_2_CHANNEL_3
  NVIC_SetPriority(IC_DMA_IRQ_NAME, 1);
  NVIC_EnableIRQ(IC_DMA_IRQ_NAME);
#endif

#ifdef USE_TIMER_15_CHANNEL_1
  NVIC_SetPriority(IC_DMA_IRQ_NAME, 1);
  NVIC_EnableIRQ(IC_DMA_IRQ_NAME);
#endif
#ifdef USE_TIMER_3_CHANNEL_1
  NVIC_SetPriority(IC_DMA_IRQ_NAME, 1);
  NVIC_EnableIRQ(IC_DMA_IRQ_NAME);
#endif

    IC_TIMER_REGISTER->PSC = 0;
    IC_TIMER_REGISTER->ARR = 96-1;

  // TIM_InitStruct.Prescaler = 0;
  // TIM_InitStruct.CounterMode = LL_TIM_COUNTERMODE_UP;
  // TIM_InitStruct.Autoreload = 65535;
  // TIM_InitStruct.ClockDivision = LL_TIM_CLOCKDIVISION_DIV1;
  // LL_TIM_Init(IC_TIMER_REGISTER, &TIM_InitStruct);
  // LL_TIM_DisableARRPreload(IC_TIMER_REGISTER);
  // LL_TIM_SetTriggerOutput(IC_TIMER_REGISTER, LL_TIM_TRGO_RESET);
  // LL_TIM_DisableMasterSlaveMode(IC_TIMER_REGISTER);
  // LL_TIM_IC_SetActiveInput(IC_TIMER_REGISTER, IC_TIMER_CHANNEL,
  //     LL_TIM_ACTIVEINPUT_DIRECTTI);
  // LL_TIM_IC_SetPrescaler(IC_TIMER_REGISTER, IC_TIMER_CHANNEL, LL_TIM_ICPSC_DIV1);
  // LL_TIM_IC_SetFilter(IC_TIMER_REGISTER, IC_TIMER_CHANNEL, LL_TIM_IC_FILTER_FDIV1);
  // LL_TIM_IC_SetPolarity(IC_TIMER_REGISTER, IC_TIMER_CHANNEL, LL_TIM_IC_POLARITY_BOTHEDGE);

}

#ifdef USE_RGB_LED // has 3 color led
void LED_GPIO_init() {
  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB);

  LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_8);
  LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_5);
  LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_3);

  GPIO_InitStruct.Pin = LL_GPIO_PIN_8;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = LL_GPIO_PIN_5;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = LL_GPIO_PIN_3;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

#endif

#ifdef USE_CUSTOM_LED
void initLed() {
  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
  GPIO_InitStruct.Pin = LL_GPIO_PIN_3;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}
#endif

void reloadWatchDogCounter() { LL_IWDG_ReloadCounter(IWDG); }

void disableComTimerInt() { COM_TIMER->DIER &= ~((0x1UL << (0U))); }

void enableComTimerInt() { COM_TIMER->DIER |= (0x1UL << (0U)); }

void setAndEnableComInt(uint16_t time) {
  COM_TIMER->CNT = 0;
  COM_TIMER->ARR = time;
  COM_TIMER->SR = 0x00;
  COM_TIMER->DIER |= (0x1UL << (0U));
}

uint16_t getintervaTimerCount() { return INTERVAL_TIMER->CNT; }

void setintervaTimerCount(uint16_t intertime) { INTERVAL_TIMER->CNT = 0; }

void setPrescalerPWM(uint16_t presc) { TIM1->PSC = presc; }

void setAutoReloadPWM(uint16_t relval) { TIM1->ARR = relval; }

void setDutyCycleAll(uint16_t newdc) {
  TIM1->CCR1 = newdc;
  TIM1->CCR2 = newdc;
  TIM1->CCR3 = newdc;
}

void setPWMCompare1(uint16_t compareone) { TIM1->CCR1 = compareone; }
void setPWMCompare2(uint16_t comparetwo) { TIM1->CCR2 = comparetwo; }
void setPWMCompare3(uint16_t comparethree) { TIM1->CCR3 = comparethree; }

void generatePwmTimerEvent() { LL_TIM_GenerateEvent_UPDATE(TIM1); }

void resetInputCaptureTimer() {
  IC_TIMER_REGISTER->PSC = 0;
  IC_TIMER_REGISTER->CNT = 0;
}

void enableCorePeripherals() {
  LL_TIM_CC_EnableChannel(TIM1, LL_TIM_CHANNEL_CH1);
  LL_TIM_CC_EnableChannel(TIM1, LL_TIM_CHANNEL_CH2);
  LL_TIM_CC_EnableChannel(TIM1, LL_TIM_CHANNEL_CH3);
  LL_TIM_CC_EnableChannel(TIM1, LL_TIM_CHANNEL_CH1N);
  LL_TIM_CC_EnableChannel(TIM1, LL_TIM_CHANNEL_CH2N);
  LL_TIM_CC_EnableChannel(TIM1, LL_TIM_CHANNEL_CH3N);

  LL_TIM_CC_EnableChannel(TIM1,
                          LL_TIM_CHANNEL_CH4); // timer used for timing adc read
  TIM1->CCR4 =
      100; // set in 10khz loop to match pwm cycle timed to end of pwm on

  /* Enable counter */
  LL_TIM_EnableCounter(TIM1);
  LL_TIM_EnableAllOutputs(TIM1);
  /* Force update generation */
  LL_TIM_GenerateEvent_UPDATE(TIM1);

#ifdef USE_ADC_INPUT

#else
  LL_TIM_CC_EnableChannel(IC_TIMER_REGISTER,
                          IC_TIMER_CHANNEL); // input capture and output compare
  LL_TIM_EnableCounter(IC_TIMER_REGISTER);
#endif

#ifdef USE_LED_STRIP
  send_LED_RGB(255, 0, 0);
#endif

#ifdef USE_RGB_LED
  LED_GPIO_init();
  GPIOB->BRR = LL_GPIO_PIN_8; // turn on red
  GPIOB->BSRR = LL_GPIO_PIN_5;
  GPIOB->BSRR = LL_GPIO_PIN_3; //
#endif

#ifdef USE_CUSTOM_LED
  initLed();
#endif

#ifndef BRUSHED_MODE
  LL_TIM_EnableCounter(COM_TIMER); // commutation_timer priority 0
  LL_TIM_GenerateEvent_UPDATE(COM_TIMER);
#endif
  LL_TIM_EnableCounter(UTILITY_TIMER);
  LL_TIM_GenerateEvent_UPDATE(UTILITY_TIMER);
  //
  LL_TIM_EnableCounter(INTERVAL_TIMER);
  LL_TIM_GenerateEvent_UPDATE(INTERVAL_TIMER);

  LL_TIM_EnableCounter(TEN_KHZ_TIMER);    // 10khz timer
  TEN_KHZ_TIMER->DIER |= (0x1UL << (0U)); // enable interrupt
  LL_TIM_GenerateEvent_UPDATE(TEN_KHZ_TIMER);
  // RCC->APB2ENR  &= ~(1 << 22);  // turn debug off
#ifdef USE_ADC
  ADC_Init();
  enableADC_DMA();
  activateADC();
#endif

#ifndef MCU_F031
  __IO uint32_t wait_loop_index = 0;
  /* Enable comparator */
  LL_COMP_Enable(MAIN_COMP);
#ifdef N_VARIANT // needs comp 1 and 2
  LL_COMP_Enable(COMP1);
#endif
  wait_loop_index =
      ((LL_COMP_DELAY_STARTUP_US * (SystemCoreClock / (100000 * 2))) / 10);
  while (wait_loop_index != 0) {
    wait_loop_index--;
  }
#endif

NVIC_SetPriority(EXTI9_5_IRQn, 2);
NVIC_EnableIRQ(EXTI9_5_IRQn);
LL_EXTI_EnableIT_0_31(LL_EXTI_LINE_9);
}
