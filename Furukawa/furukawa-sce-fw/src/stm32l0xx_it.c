/*
 / _____)             _              | |
 ( (____  _____ ____ _| |_ _____  ____| |__
 \____ \| ___ |    (_   _) ___ |/ ___)  _ \
 _____) ) ____| | | || |_| ____( (___| | | |
 (______/|_____)_|_|_| \__)_____)\____)_| |_|
 (C)2013 Semtech

 Description: Bleeper board GPIO driver implementation

 License: Revised BSD License, see LICENSE.TXT file include in the project

 Maintainer: Miguel Luis and Gregory Cristian
 */
/******************************************************************************
 * @file    stm32l0xx_it.c
 * @author  MCD Application Team
 * @version V1.1.4
 * @date    08-January-2018
 * @brief   manages interupt
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2017 STMicroelectronics International N.V. 
 * All rights reserved.</center></h2>
 *
 * Redistribution and use in source and binary forms, with or without 
 * modification, are permitted, provided that the following conditions are met:
 *
 * 1. Redistribution of source code must retain the above copyright notice, 
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. Neither the name of STMicroelectronics nor the names of other 
 *    contributors to this software may be used to endorse or promote products 
 *    derived from this software without specific written permission.
 * 4. This software, including modifications and/or derivative works of this 
 *    software, must execute solely and exclusively on microcontroller or
 *    microprocessor devices manufactured by or for STMicroelectronics.
 * 5. Redistribution and use of this software other than as permitted under 
 *    this license is void and will automatically terminate your rights under 
 *    this license. 
 *
 * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
 * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
 * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
 * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
 * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "hw.h"
#include "stm32l0xx_it.h"

extern UART_HandleTypeDef UartHandle;
extern int exti_flag;
extern UART_HandleTypeDef huart1;
/** @addtogroup STM32L1xx_HAL_Examples
 * @{
 */

/** @addtogroup SPI_FullDuplex_ComPolling
 * @{
 */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
 * @brief   This function handles NMI exception.
 * @param  None
 * @retval None
 */

void NMI_Handler (void)
{
}

/**
 * @brief  This function handles Hard Fault exception.
 * @param  None
 * @retval None
 */

void HardFault_Handler (void)
{
	update_fault(Hard_Fault);

#if TEST_FAULT_MEM

#else
	while (1)
	{
		__NOP ();
	}
#endif

}

/**
 * @brief  This function handles Memory Manage exception.
 * @param  None
 * @retval None
 */
void MemManage_Handler (void)
{
	update_fault(MEM_Fault);
	/* Go to infinite loop when Memory Manage exception occurs */
#if TEST_FAULT_MEM

#else
	while (1)
	{
	}

#endif
}

/**
 * @brief  This function handles Bus Fault exception.
 * @param  None
 * @retval None
 */
void BusFault_Handler (void)
{
	update_fault(Bus_Fault);
#if TEST_FAULT_MEM

#else
	/* Go to infinite loop when Bus Fault exception occurs */
	while (1)
	{
	}

#endif

}

/**
 * @brief  This function handles Usage Fault exception.
 * @param  None
 * @retval None
 */
void UsageFault_Handler (void)
{
	update_fault(Usage_Fault);
	/* Go to infinite loop when Usage Fault exception occurs */
#if TEST_FAULT_MEM

#else
	while (1)
	{
	}
#endif
}

/**
 * @brief  This function handles Debug Monitor exception.
 * @param  None
 * @retval None
 */
void DebugMon_Handler (void)
{
}

/**
 * @brief  This function handles SysTick Handler.
 * @param  None
 * @retval None
 */
void SysTick_Handler (void)
{
	HAL_IncTick ();
}

/******************************************************************************/
/*                 STM32L1xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32l1xx.s).                                               */
/******************************************************************************/

/**
 * @brief  This function handles PPP interrupt request.
 * @param  None
 * @retval None
 */
/*void PPP_IRQHandler(void)
 {
 }*/

void RTC_IRQHandler (void)
{
	HW_RTC_IrqHandler ();
}

///**
//  * @brief This function handles EXTI line 2 and line 3 interrupts.
//  */
//void EXTI2_3_IRQHandler(void)
//{
//  /* USER CODE BEGIN EXTI2_3_IRQn 0 */
//
//  /* USER CODE END EXTI2_3_IRQn 0 */
//  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_3);
//  /* USER CODE BEGIN EXTI2_3_IRQn 1 */
//
//  /* USER CODE END EXTI2_3_IRQn 1 */
//}
//
///**
//  * @brief This function handles EXTI line 4 to 15 interrupts.
//  */
//void EXTI4_15_IRQHandler(void)
//{
//  /* USER CODE BEGIN EXTI4_15_IRQn 0 */
//
//  /* USER CODE END EXTI4_15_IRQn 0 */
//  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_4);
//  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_8);
//  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_12);
//  /* USER CODE BEGIN EXTI4_15_IRQn 1 */
//
//  /* USER CODE END EXTI4_15_IRQn 1 */
//}

void EXTI0_1_IRQHandler (void)
{
	//HAL_GPIO_EXTI_IRQHandler ( GPIO_PIN_0);
	HAL_GPIO_EXTI_IRQHandler ( GPIO_PIN_1);
}

void EXTI2_3_IRQHandler (void)
{
	HAL_GPIO_EXTI_IRQHandler ( GPIO_PIN_2);

	// IMU_IRQ2
	HAL_GPIO_EXTI_IRQHandler (GPIO_PIN_3);
}

void EXTI4_15_IRQHandler (void)
{
	// IMU_IRQ1
	HAL_GPIO_EXTI_IRQHandler (GPIO_PIN_4);

	HAL_GPIO_EXTI_IRQHandler ( GPIO_PIN_5);
	HAL_GPIO_EXTI_IRQHandler ( GPIO_PIN_6);
	HAL_GPIO_EXTI_IRQHandler ( GPIO_PIN_7);

	// TEMP_IRQ
	HAL_GPIO_EXTI_IRQHandler (GPIO_PIN_8);

	HAL_GPIO_EXTI_IRQHandler ( GPIO_PIN_9);
	HAL_GPIO_EXTI_IRQHandler ( GPIO_PIN_10);
	HAL_GPIO_EXTI_IRQHandler ( GPIO_PIN_11);

	// LX_IRQ2
	HAL_GPIO_EXTI_IRQHandler ( GPIO_PIN_12);

	HAL_GPIO_EXTI_IRQHandler ( GPIO_PIN_13);
	//HAL_GPIO_EXTI_IRQHandler( GPIO_PIN_14);
	if (__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_14) != RESET)
	{
		exti_flag = 1;
		__HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_14);
		HAL_GPIO_EXTI_Callback (GPIO_PIN_14);
	}
	HAL_GPIO_EXTI_IRQHandler ( GPIO_PIN_15);

}

void update_fault(type_fault falta) {

	uint32_t write_back, faultCode;
	uint8_t single_fault;
	write_back = falta;
	faultCode = readFromEEPROM(FAULT_COUNTER_ADD_IN);
	switch(falta) {
		case Hard_Fault:
			single_fault = (faultCode & (0xFF000000)) >> 24;
			faultCode = faultCode & 0x00FFFFFF;
			if(single_fault < 0xFF) {
				single_fault++;
				write_back = faultCode | (single_fault << 24);
			}
			break;
		case Bus_Fault:
			single_fault = (faultCode & (0x00FF0000)) >> 16;
			faultCode = faultCode & 0xFF00FFFF;
			if(single_fault < 0xFF) {
				single_fault++;
				write_back = faultCode | (single_fault << 16);
			}

			break;
		case MEM_Fault:
			single_fault = (faultCode & (0x0000FF00)) >> 8;
			faultCode = faultCode & 0xFFFF00FF;
			if(single_fault < 0xFF) {
				single_fault++;
				write_back = faultCode | (single_fault << 8);
			}

			break;
		case Usage_Fault:
			single_fault = (faultCode & (0x000000FF));
			faultCode = faultCode & 0xFFFFFF00;
			if(single_fault < 0xFF) {
				single_fault++;
				write_back = faultCode | (single_fault);
			}

			break;
		default:
			break;


	}
	writeToEEPROM(FAULT_COUNTER_ADD_IN, write_back);
}

void USART1_IRQHandler(void)
{
  /* USER CODE BEGIN USART1_IRQn 0 */

  /* USER CODE END USART1_IRQn 0 */
  HAL_UART_IRQHandler(&huart1);
  /* USER CODE BEGIN USART1_IRQn 1 */

  /* USER CODE END USART1_IRQn 1 */
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
