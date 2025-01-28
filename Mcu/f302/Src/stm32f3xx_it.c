/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file    stm32f0xx_it.c
 * @brief   Interrupt Service Routines.
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under BSD 3-Clause license,
 * the "License"; You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                        opensource.org/licenses/BSD-3-Clause
 *
 ******************************************************************************
 */
/* USER CODE END Header */

/* Includes
 * ------------------------------------------------------------------*/
#include "stm32f3xx_it.h"

#include "main.h"
/* Private includes
 * ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "ADC.h"
#include "targets.h"
#include "peripherals.h"

extern void transfercomplete();
extern void PeriodElapsedCallback();
extern void interruptRoutine();
extern void doPWMChanges();
extern void tenKhzRoutine();
extern void sendDshotDma();
extern void receiveDshotDma();
extern void processDshot();
extern char send_telemetry;
extern char telemetry_done;
extern char servoPwm;
extern char dshot_telemetry;
extern char armed;
extern char out_put;
extern char compute_dshot_flag;
/* USER CODE END EV */

int interrupt_time = 0;
/******************************************************************************/
/*           Cortex-M0 Processor Interruption and Exception Handlers */
/******************************************************************************/
/**
 * @brief This function handles Non maskable interrupt.
 */
void NMI_Handler(void)
{
}

/**
 * @brief This function handles Hard fault interrupt.
 */
void HardFault_Handler(void)
{
    while (1) {
    }
}

/**
 * @brief This function handles System service call via SWI instruction.
 */
void SVC_Handler(void)
{
}

/**
 * @brief This function handles Pendable request for system service.
 */
void PendSV_Handler(void)
{
}

/**
 * @brief This function handles System tick timer.
 */
void SysTick_Handler(void)
{
}

/******************************************************************************/
/* STM32F3xx Peripheral Interrupt Handlers */
/* Add here the Interrupt Handlers for the used peripherals. */
/* For the available peripheral interrupt handler names, */
/* please refer to the startup file (startup_stm32f3xx.s). */
/******************************************************************************/
// ADC DMA (Disable for now)
// void DMA1_Channel1_IRQHandler(void) // ADC
// {
//     if (LL_DMA_IsActiveFlag_TC1(DMA1) == 1)
//     {
//         /* Clear flag DMA global interrupt */
//         /* (global interrupt flag: half transfer and transfer complete flags) */
//         LL_DMA_ClearFlag_GI1(DMA1);
//         ADC_DMA_Callback();
//         /* Call interruption treatment function */
//         //   AdcDmaTransferComplete_Callback();
//     }

//     /* Check whether DMA transfer error caused the DMA interruption */
//     if (LL_DMA_IsActiveFlag_TE1(DMA1) == 1)
//     {
//         /* Clear flag DMA transfer error */
//         LL_DMA_ClearFlag_TE1(DMA1);

//         /* Call interruption treatment function */
//     }
// }

void DMA1_Channel1_IRQHandler(void)
{
#ifdef USE_TIMER_2_CHANNEL_3
    if (armed && dshot_telemetry)
    {
        DMA1->IFCR |= DMA_IFCR_CGIF1;
        DMA1_Channel1->CCR = 0x00;
        if (out_put)
        {
            receiveDshotDma();
            compute_dshot_flag = 2;
        }
        else
        {
            sendDshotDma();
            compute_dshot_flag = 1;
        }
        EXTI->SWIER |= LL_EXTI_LINE_9;
        return;
    }
    if (LL_DMA_IsActiveFlag_HT1(DMA1))
    {
        if (servoPwm)
        {
            LL_TIM_IC_SetPolarity(IC_TIMER_REGISTER, IC_TIMER_CHANNEL,
                                  LL_TIM_IC_POLARITY_FALLING);
            LL_DMA_ClearFlag_HT1(DMA1);
        }
    }
    if (LL_DMA_IsActiveFlag_TC1(DMA1) == 1)
    {
        LL_DMA_ClearFlag_GI1(DMA1);
        LL_DMA_DisableChannel(DMA1, LL_DMA_CHANNEL_1);
        transfercomplete();
        EXTI->SWIER |= LL_EXTI_LINE_9;
    }
    else if (LL_DMA_IsActiveFlag_TE1(DMA1) == 1)
    {
        LL_DMA_ClearFlag_GI1(DMA1);
    }
#endif
}

void DMA2_Channel1_IRQHandler(void)
{
    if (LL_DMA_IsActiveFlag_TC1(DMA2) == 1)
    {
        /* Clear flag DMA global interrupt */
        /* (global interrupt flag: half transfer and transfer complete flags) */
        LL_DMA_ClearFlag_GI1(DMA2);
        ADC_DMA_Callback();
        /* Call interruption treatment function */
        //   AdcDmaTransferComplete_Callback();
    }

    /* Check whether DMA transfer error caused the DMA interruption */
    if (LL_DMA_IsActiveFlag_TE1(DMA2) == 1)
    {
        /* Clear flag DMA transfer error */
        LL_DMA_ClearFlag_TE1(DMA2);

        /* Call interruption treatment function */
    }
}

void EXTI9_5_IRQHandler(void) {
    LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_9);
    processDshot();
}

/**
 * @brief This function handles TIM1 update and TIM16 interrupts.
 */
void TIM1_UP_TIM16_IRQHandler(void)
{
    if (LL_TIM_IsActiveFlag_UPDATE(TIM16) == 1) {
        PeriodElapsedCallback();
        LL_TIM_ClearFlag_UPDATE(TIM16);
    }
}

/**
 * @brief This function handles TIM6 global interrupt and DAC1 underrun interrupt.
 */
void TIM6_DAC_IRQHandler(void)
{
    if (LL_TIM_IsActiveFlag_UPDATE(TIM6) == 1)
    {
        LL_TIM_ClearFlag_UPDATE(TIM6);
        tenKhzRoutine();
    }
}

void TIM2_IRQHandler(void)
{
    if (LL_TIM_IsActiveFlag_CC1(TIM2) == 1) {
        LL_TIM_ClearFlag_CC1(TIM2);
    }

    if (LL_TIM_IsActiveFlag_UPDATE(TIM2) == 1) {
        LL_TIM_ClearFlag_UPDATE(TIM2);
        // update_interupt++;
    }
}

/**
 * @brief This function handles COMP1 and COMP2 interrupts through EXTI lines 21 and 22.
 */
void COMP1_2_IRQHandler(void)
{
    if (LL_EXTI_IsActiveFlag_0_31(LL_EXTI_LINE_21) != RESET)
    {
        LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_21);
        interruptRoutine();
    }
}
