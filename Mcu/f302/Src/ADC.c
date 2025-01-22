/*
 * ADC.c
 *
 *  Created on: May 20, 2020
 *      Author: Alka
 */
#include "ADC.h"
#include "main.h"

uint16_t ADC1DataDMA[2];
uint16_t ADC2DataDMA[3];

extern uint16_t ADC_raw_temp;
extern uint16_t ADC_raw_volts;
extern uint16_t ADC_raw_current;
extern uint16_t ADC_raw_input;

void ADC_DMA_Callback() { // read dma buffer and set extern variables

  // ADC_raw_temp = ADC1DataDMA[1];
  // ADC_raw_volts = ADC1DataDMA[0];
  // ADC_raw_current =
  //     (ADC2DataDMA[0] + ADC2DataDMA[1] + ADC2DataDMA[2]) - 3 * 2048;
     
  ADC_raw_temp = 1000;
  ADC_raw_volts = 1000;
  ADC_raw_current = 1000;
}

void enableADC_DMA() { // enables channel

  
  // //	NVIC_SetPriority(DMA1_Channel1_IRQn, 3);
  // //	NVIC_EnableIRQ(DMA1_Channel1_IRQn);

  // LL_DMA_ConfigAddresses(
  //     DMA1, LL_DMA_CHANNEL_1,
  //     LL_ADC_DMA_GetRegAddr(ADC1, LL_ADC_DMA_REG_REGULAR_DATA),
  //     (uint32_t)&ADC1DataDMA, LL_DMA_DIRECTION_PERIPH_TO_MEMORY);

  // LL_DMA_ConfigAddresses(
  //     DMA2, LL_DMA_CHANNEL_1,
  //     LL_ADC_DMA_GetRegAddr(ADC2, LL_ADC_DMA_REG_REGULAR_DATA),
  //     (uint32_t)&ADC2DataDMA, LL_DMA_DIRECTION_PERIPH_TO_MEMORY);

  // /* Set DMA transfer size */
  // LL_DMA_SetDataLength(DMA1, LL_DMA_CHANNEL_1, 2);
  // LL_DMA_SetDataLength(DMA2, LL_DMA_CHANNEL_1, 3);

  // // LL_DMA_EnableIT_TC(DMA1,LL_DMA_CHANNEL_1);

  // // LL_DMA_EnableIT_TE(DMA1,LL_DMA_CHANNEL_1);

  // /*## Activation of DMA
  //  * #####################################################*/
  // /* Enable the DMA transfer */
  // LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_1);
  // LL_DMA_EnableChannel(DMA2, LL_DMA_CHANNEL_1);
}

void activateADC() { // called right after enable regular conversions are
  // started by software and DMA interrupt happens at end of
  // transfer

  __IO uint32_t wait_loop_index = 0;

  LL_ADC_StartCalibration(ADC1, LL_ADC_SINGLE_ENDED);
  LL_ADC_StartCalibration(ADC2, LL_ADC_SINGLE_ENDED);

  /* Poll for ADC effectively calibrated */

  while (LL_ADC_IsCalibrationOnGoing(ADC1) != 0) {
  }
  wait_loop_index = (LL_ADC_DELAY_CALIB_ENABLE_ADC_CYCLES >> 1);
  while (wait_loop_index != 0) {
    wait_loop_index--;
  }

  wait_loop_index = 0;

  while (LL_ADC_IsCalibrationOnGoing(ADC2) != 0) {
  }
  wait_loop_index = (LL_ADC_DELAY_CALIB_ENABLE_ADC_CYCLES >> 1);
  while (wait_loop_index != 0) {
    wait_loop_index--;
  }

  LL_ADC_Enable(ADC1);
  LL_ADC_Enable(ADC2);
  while (LL_ADC_IsActiveFlag_ADRDY(ADC1) == 0) {
  }
  while (LL_ADC_IsActiveFlag_ADRDY(ADC2) == 0) {
  }
}

static void MX_ADC1_Init(void) {

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  LL_ADC_InitTypeDef ADC_InitStruct = {0};
  LL_ADC_REG_InitTypeDef ADC_REG_InitStruct = {0};
  LL_ADC_CommonInitTypeDef ADC_CommonInitStruct = {0};

  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* Peripheral clock enable */
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_ADC12);

  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);
  /**ADC1 GPIO Configuration
  PA1   ------> ADC1_IN2
  PA2   ------> ADC1_IN3
  */
  GPIO_InitStruct.Pin = LL_GPIO_PIN_1 | FET_TEMP_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* ADC1 DMA Init */

  /* ADC1 Init */
  // LL_DMA_SetDataTransferDirection(DMA1, LL_DMA_CHANNEL_1,
  //                                 LL_DMA_DIRECTION_PERIPH_TO_MEMORY);

  // LL_DMA_SetChannelPriorityLevel(DMA1, LL_DMA_CHANNEL_1, LL_DMA_PRIORITY_LOW);

  // LL_DMA_SetMode(DMA1, LL_DMA_CHANNEL_1, LL_DMA_MODE_NORMAL);

  // LL_DMA_SetPeriphIncMode(DMA1, LL_DMA_CHANNEL_1, LL_DMA_PERIPH_NOINCREMENT);

  // LL_DMA_SetMemoryIncMode(DMA1, LL_DMA_CHANNEL_1, LL_DMA_MEMORY_INCREMENT);

  // LL_DMA_SetPeriphSize(DMA1, LL_DMA_CHANNEL_1, LL_DMA_PDATAALIGN_HALFWORD);

  // LL_DMA_SetMemorySize(DMA1, LL_DMA_CHANNEL_1, LL_DMA_MDATAALIGN_HALFWORD);

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */

  /** Common config
   */
  ADC_InitStruct.Resolution = LL_ADC_RESOLUTION_12B;
  ADC_InitStruct.DataAlignment = LL_ADC_DATA_ALIGN_RIGHT;
  ADC_InitStruct.LowPowerMode = LL_ADC_LP_MODE_NONE;
  LL_ADC_Init(ADC1, &ADC_InitStruct);
  ADC_REG_InitStruct.TriggerSource = LL_ADC_REG_TRIG_SOFTWARE;
  ADC_REG_InitStruct.SequencerLength = LL_ADC_REG_SEQ_SCAN_ENABLE_2RANKS;
  ADC_REG_InitStruct.SequencerDiscont = LL_ADC_REG_SEQ_DISCONT_DISABLE;
  ADC_REG_InitStruct.ContinuousMode = LL_ADC_REG_CONV_SINGLE;
  // ADC_REG_InitStruct.DMATransfer = LL_ADC_REG_DMA_TRANSFER_LIMITED;
  ADC_REG_InitStruct.DMATransfer = LL_ADC_REG_DMA_TRANSFER_NONE;
  ADC_REG_InitStruct.Overrun = LL_ADC_REG_OVR_DATA_PRESERVED;
  LL_ADC_REG_Init(ADC1, &ADC_REG_InitStruct);
  ADC_CommonInitStruct.CommonClock = LL_ADC_CLOCK_ASYNC_DIV1;
  ADC_CommonInitStruct.Multimode = LL_ADC_MULTI_INDEPENDENT;
  LL_ADC_CommonInit(__LL_ADC_COMMON_INSTANCE(ADC1), &ADC_CommonInitStruct);

  /* Enable ADC internal voltage regulator */
  LL_ADC_EnableInternalRegulator(ADC1);
  /* Delay for ADC internal voltage regulator stabilization. */
  /* Compute number of CPU cycles to wait for, from delay in us. */
  /* Note: Variable divided by 2 to compensate partially */
  /* CPU processing cycles (depends on compilation optimization). */
  /* Note: If system core clock frequency is below 200kHz, wait time */
  /* is only a few CPU processing cycles. */
  uint32_t wait_loop_index;
  wait_loop_index = ((LL_ADC_DELAY_INTERNAL_REGUL_STAB_US *
                      (SystemCoreClock / (100000 * 2))) /
                     10);
  while (wait_loop_index != 0) {
    wait_loop_index--;
  }

  /** Configure Regular Channel
   */
  LL_ADC_REG_SetSequencerRanks(ADC1, LL_ADC_REG_RANK_1, LL_ADC_CHANNEL_3);
  LL_ADC_SetChannelSamplingTime(ADC1, LL_ADC_CHANNEL_3,
                                LL_ADC_SAMPLINGTIME_61CYCLES_5);
  LL_ADC_SetChannelSingleDiff(ADC1, LL_ADC_CHANNEL_3, LL_ADC_SINGLE_ENDED);

  /** Configure Regular Channel
   */
  LL_ADC_REG_SetSequencerRanks(ADC1, LL_ADC_REG_RANK_2,
                               LL_ADC_CHANNEL_TEMPSENSOR);
  LL_ADC_SetChannelSamplingTime(ADC1, LL_ADC_CHANNEL_TEMPSENSOR,
                                LL_ADC_SAMPLINGTIME_61CYCLES_5);
  LL_ADC_SetChannelSingleDiff(ADC1, LL_ADC_CHANNEL_TEMPSENSOR,
                              LL_ADC_SINGLE_ENDED);
  LL_ADC_SetCommonPathInternalCh(__LL_ADC_COMMON_INSTANCE(ADC1),
                                 LL_ADC_PATH_INTERNAL_TEMPSENSOR);
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */
}

/**
 * @brief ADC2 Initialization Function
 * @param None
 * @retval None
 */
static void MX_ADC2_Init(void) {

  /* USER CODE BEGIN ADC2_Init 0 */

  /* USER CODE END ADC2_Init 0 */

  LL_ADC_InitTypeDef ADC_InitStruct = {0};
  LL_ADC_REG_InitTypeDef ADC_REG_InitStruct = {0};

  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* Peripheral clock enable */
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_ADC12);

  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOC);
  /**ADC2 GPIO Configuration
  PA6   ------> ADC2_IN3
  PC4   ------> ADC2_IN5
  PC5   ------> ADC2_IN11
  */
  GPIO_InitStruct.Pin = CURR_A_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(CURR_A_GPIO_Port, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = CURR_B_Pin | CURR_C_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /* ADC2 DMA Init */

  /* ADC2 Init */
  LL_DMA_SetDataTransferDirection(DMA2, LL_DMA_CHANNEL_1,
                                  LL_DMA_DIRECTION_PERIPH_TO_MEMORY);

  LL_DMA_SetChannelPriorityLevel(DMA2, LL_DMA_CHANNEL_1, LL_DMA_PRIORITY_LOW);

  LL_DMA_SetMode(DMA2, LL_DMA_CHANNEL_1, LL_DMA_MODE_NORMAL);

  LL_DMA_SetPeriphIncMode(DMA2, LL_DMA_CHANNEL_1, LL_DMA_PERIPH_NOINCREMENT);

  LL_DMA_SetMemoryIncMode(DMA2, LL_DMA_CHANNEL_1, LL_DMA_MEMORY_INCREMENT);

  LL_DMA_SetPeriphSize(DMA2, LL_DMA_CHANNEL_1, LL_DMA_PDATAALIGN_HALFWORD);

  LL_DMA_SetMemorySize(DMA2, LL_DMA_CHANNEL_1, LL_DMA_MDATAALIGN_HALFWORD);

  /* USER CODE BEGIN ADC2_Init 1 */

  /* USER CODE END ADC2_Init 1 */

  /** Common config
   */
  ADC_InitStruct.Resolution = LL_ADC_RESOLUTION_12B;
  ADC_InitStruct.DataAlignment = LL_ADC_DATA_ALIGN_RIGHT;
  ADC_InitStruct.LowPowerMode = LL_ADC_LP_MODE_NONE;
  LL_ADC_Init(ADC2, &ADC_InitStruct);
  ADC_REG_InitStruct.TriggerSource = LL_ADC_REG_TRIG_SOFTWARE;
  ADC_REG_InitStruct.SequencerLength = LL_ADC_REG_SEQ_SCAN_ENABLE_3RANKS;
  ADC_REG_InitStruct.SequencerDiscont = LL_ADC_REG_SEQ_DISCONT_DISABLE;
  ADC_REG_InitStruct.ContinuousMode = LL_ADC_REG_CONV_SINGLE;
  ADC_REG_InitStruct.DMATransfer = LL_ADC_REG_DMA_TRANSFER_LIMITED;
  ADC_REG_InitStruct.Overrun = LL_ADC_REG_OVR_DATA_PRESERVED;
  LL_ADC_REG_Init(ADC2, &ADC_REG_InitStruct);

  /* Enable ADC internal voltage regulator */
  LL_ADC_EnableInternalRegulator(ADC2);
  /* Delay for ADC internal voltage regulator stabilization. */
  /* Compute number of CPU cycles to wait for, from delay in us. */
  /* Note: Variable divided by 2 to compensate partially */
  /* CPU processing cycles (depends on compilation optimization). */
  /* Note: If system core clock frequency is below 200kHz, wait time */
  /* is only a few CPU processing cycles. */
  uint32_t wait_loop_index;
  wait_loop_index = ((LL_ADC_DELAY_INTERNAL_REGUL_STAB_US *
                      (SystemCoreClock / (100000 * 2))) /
                     10);
  while (wait_loop_index != 0) {
    wait_loop_index--;
  }

  /** Configure Regular Channel
   */
  LL_ADC_REG_SetSequencerRanks(ADC2, LL_ADC_REG_RANK_1, LL_ADC_CHANNEL_3);
  LL_ADC_SetChannelSamplingTime(ADC2, LL_ADC_CHANNEL_3,
                                LL_ADC_SAMPLINGTIME_61CYCLES_5);
  LL_ADC_SetChannelSingleDiff(ADC2, LL_ADC_CHANNEL_3, LL_ADC_SINGLE_ENDED);

  /** Configure Regular Channel
   */
  LL_ADC_REG_SetSequencerRanks(ADC2, LL_ADC_REG_RANK_2, LL_ADC_CHANNEL_5);
  LL_ADC_SetChannelSamplingTime(ADC2, LL_ADC_CHANNEL_5,
                                LL_ADC_SAMPLINGTIME_61CYCLES_5);
  LL_ADC_SetChannelSingleDiff(ADC2, LL_ADC_CHANNEL_5, LL_ADC_SINGLE_ENDED);

  /** Configure Regular Channel
   */
  LL_ADC_REG_SetSequencerRanks(ADC2, LL_ADC_REG_RANK_3, LL_ADC_CHANNEL_11);
  LL_ADC_SetChannelSamplingTime(ADC2, LL_ADC_CHANNEL_11,
                                LL_ADC_SAMPLINGTIME_61CYCLES_5);
  LL_ADC_SetChannelSingleDiff(ADC2, LL_ADC_CHANNEL_11, LL_ADC_SINGLE_ENDED);
  /* USER CODE BEGIN ADC2_Init 2 */

  /* USER CODE END ADC2_Init 2 */
}

void ADC_Init(void) {
  MX_ADC1_Init();
  MX_ADC2_Init();
}
