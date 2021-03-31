/******************************************************************************
 * @file    stm32l0xx_hal_msp.c
 * @author  MCD Application Team
 * @version V1.1.4
 * @date    08-January-2018
 * @brief   msp file for HAL
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
#include "delay.h"
#include "timeServer.h"
#include "main.h"
/* when fast wake up is enabled, the mcu wakes up in ~20us  * and 
 * does not wait for the VREFINT to be settled. THis is ok for 
 * most of the case except when adc must be used in this case before 
 *starting the adc, you must make sure VREFINT is settled*/
//#define ENABLE_FAST_WAKEUP

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/*!
 * Luminosity interruption flag
 */
extern int lum_gflag;
/*!
 * Moviment interruption flag
 */
extern int mov_gflag;
/*!
 * State of fsm
 */
extern int fsm_state;

/* Private function prototypes -----------------------------------------------*/

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* interruption callback function*/
extern void interruption_callback(void);

/* Private functions ---------------------------------------------------------*/

/**
 * @brief This function configures the source of the time base.
 * @brief  don't enable systick
 * @param TickPriority: Tick interrupt priority.
 * @retval HAL status
 */
HAL_StatusTypeDef HAL_InitTick (uint32_t TickPriority)
{
	/* Return function status */
	return HAL_OK;
}

/**
 * @brief This function provides delay (in ms)
 * @param Delay: specifies the delay time length, in milliseconds.
 * @retval None
 */
void HAL_Delay (__IO uint32_t Delay)
{
	DelayMs (Delay); /* based on RTC */
}

/**
 * #!#
 * @brief  Initializes the MSP.
 * @retval None
 */
void HAL_MspInit (void)
{
	__HAL_RCC_SYSCFG_CLK_ENABLE();
	__HAL_RCC_PWR_CLK_ENABLE();

	/* Disable the Power Voltage Detector */
	HAL_PWR_DisablePVD ();

	/* Enables the Ultra Low Power mode */
	HAL_PWREx_EnableUltraLowPower ();

	__HAL_FLASH_SLEEP_POWERDOWN_ENABLE();

	/*In debug mode, e.g. when DBGMCU is activated, Arm core has always clocks
	 * And will not wait that the FLACH is ready to be read. It can miss in this 
	 * case the first instruction. To overcome this issue, the flash remain clcoked during sleep mode
	 */
	DBG(__HAL_FLASH_SLEEP_POWERDOWN_DISABLE(););

#ifdef ENABLE_FAST_WAKEUP
	/*Enable fast wakeUp*/
	HAL_PWREx_EnableFastWakeUp ();
#else  
	HAL_PWREx_DisableFastWakeUp( );
#endif
}

/**
 * @brief UART MSP Initialization
 *        This function configures the hardware resources used in this example:
 *           - Peripheral's clock enable
 *           - Peripheral's GPIO Configuration
 *           - NVIC configuration for UART interrupt request enable
 * @param huart: UART handle pointer
 * @retval None
 */
/**
* @brief UART MSP Initialization
* This function configures the hardware resources used in this example
* @param huart: UART handle pointer
* @retval None
*/
void HAL_UART_MspInit(UART_HandleTypeDef* huart)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(huart->Instance==USART1)
  {

    /* Peripheral clock enable */
    __HAL_RCC_USART1_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**USART1 GPIO Configuration
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_9|GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF4_USART1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* USART1 interrupt Init */
    HAL_NVIC_SetPriority(USART1_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(USART1_IRQn);

  }

}

/**
* @brief UART MSP De-Initialization
* This function freeze the hardware resources used in this example
* @param huart: UART handle pointer
* @retval None
*/
void HAL_UART_MspDeInit(UART_HandleTypeDef* huart)
{
  if(huart->Instance==USART1)
  {
    /* Peripheral clock disable */
    __HAL_RCC_USART1_CLK_DISABLE();

    /**USART1 GPIO Configuration
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_9|GPIO_PIN_10);


    /* USART1 interrupt DeInit */
    HAL_NVIC_DisableIRQ(USART1_IRQn);

  }

}

/**
 * @brief RTC MSP Initialization
 *        This function configures the hardware resources used in this example:
 *           - Peripheral's clock enable
 * @param hrtc: RTC handle pointer
 * @note  Care must be taken when HAL_RCCEx_PeriphCLKConfig() is used to selects
 *        the RTC clock source; in this case the Backup domain will be reset in
 *        order to modify the RTC Clock source, as consequence RTC registers (including
 *        the backup registers) and RCC_CSR register are set to their reset values.
 * @retval None
 */
void HAL_RTC_MspInit (RTC_HandleTypeDef *hrtc)
{
	RCC_OscInitTypeDef RCC_OscInitStruct;
	RCC_PeriphCLKInitTypeDef PeriphClkInitStruct;

	/*##-1- Configue the RTC clock soucre ######################################*/
	/* -a- Enable LSE Oscillator */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSE;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
	RCC_OscInitStruct.LSEState = RCC_LSE_ON;
	if (HAL_RCC_OscConfig (&RCC_OscInitStruct) != HAL_OK)
	{
		Error_Handler ();
	}

	/* -b- Select LSI as RTC clock source */
	PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
	PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
	if (HAL_RCCEx_PeriphCLKConfig (&PeriphClkInitStruct) != HAL_OK)
	{
		Error_Handler ();
	}

	/*##-2- Enable the RTC peripheral Clock ####################################*/
	/* Enable RTC Clock */
	__HAL_RCC_RTC_ENABLE();

	/*##-3- Configure the NVIC for RTC Alarm ###################################*/
	HAL_NVIC_SetPriority (RTC_Alarm_IRQn, 0x0, 0);
	HAL_NVIC_EnableIRQ (RTC_Alarm_IRQn);
}

/**
 * @brief RTC MSP De-Initialization 
 *        This function freeze the hardware resources used in this example:
 *          - Disable the Peripheral's clock
 * @param hrtc: RTC handle pointer
 * @retval None
 */
void HAL_RTC_MspDeInit (RTC_HandleTypeDef *hrtc)
{
	/* Reset peripherals */
	__HAL_RCC_RTC_DISABLE();
}

/**
 * @brief  Alarm A callback.
 * @param  hrtc: RTC handle
 * @retval None
 */
void HAL_RTC_AlarmAEventCallback (RTC_HandleTypeDef *hrtc)
{
	TimerIrqHandler ();
}

int mov_flag = 0; //flag para interrupcao de movimento

/**
 * @brief  EXTI line detection callbacks.
 * @param  GPIO_Pin: Specifies the pins connected to the EXTI line.
 * @retval None
 */
void HAL_GPIO_EXTI_Callback (uint16_t GPIO_Pin)
{
	HW_GPIO_IrqHandler (GPIO_Pin);

	// tratamento do acionamento do botao (PINO1A = PA1)
	if(GPIO_Pin == ON_FB_Pin) {
		keepAlive_callback();
	}
	// Tratamento interrupcao luminosidade
	if(GPIO_Pin == LX_IRQ2_Pin){
		lum_gflag = 1;
		interruption_callback ();
	}
	// Tratamento interrupcao movimento 6D
	if((GPIO_Pin == IMU_IRQ1_Pin) | (GPIO_Pin == IMU_IRQ2_Pin)){
		if(mov_flag == 1){
			mov_gflag = 1;
			interruption_callback ();
		}
		mov_flag = 1;
	}
//	// Tratamento interrupcao sigmot
//	if(GPIO_Pin == IMU_IRQ2_Pin){
//		mov_gflag = 1;
//		call_send ();
//	}
}
/**
 * @brief  Gets IRQ number as a function of the GPIO_Pin.
 * @param  GPIO_Pin: Specifies the pins connected to the EXTI line.
 * @retval IRQ number
 */
IRQn_Type MSP_GetIRQn (uint16_t GPIO_Pin)
{
	switch (GPIO_Pin) {
		case GPIO_PIN_0:
			return EXTI0_1_IRQn;
		case GPIO_PIN_1:
			return EXTI0_1_IRQn;
		case GPIO_PIN_2:
			return EXTI2_3_IRQn;
		case GPIO_PIN_3:
			return IMU_IRQ2_EXTI_IRQn;
		case GPIO_PIN_4:
			return IMU_IRQ1_EXTI_IRQn;
		case GPIO_PIN_5:
			return EXTI4_15_IRQn;
		case GPIO_PIN_6:
			return EXTI4_15_IRQn;
		case GPIO_PIN_7:
			return EXTI4_15_IRQn;
		case GPIO_PIN_9:
			return EXTI4_15_IRQn;
		case GPIO_PIN_10:
			return EXTI4_15_IRQn;
		case GPIO_PIN_11:
			return EXTI4_15_IRQn;
		case GPIO_PIN_12:
			return LX_IRQ2_EXTI_IRQn;
		case GPIO_PIN_13:
			return EXTI4_15_IRQn;
		case GPIO_PIN_14:
			return EXTI4_15_IRQn;
		case GPIO_PIN_15:
			return EXTI4_15_IRQn;
		default:
			return EXTI4_15_IRQn;
	}
}

/**
 * #!#
* @brief ADC MSP Initialization
* This function configures the hardware resources used in this example
* @param hadc: ADC handle pointer
* @retval None
*/
void HAL_ADC_MspInit(ADC_HandleTypeDef* hadc)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(hadc->Instance==ADC1)
  {
  /* USER CODE BEGIN ADC1_MspInit 0 */

  /* USER CODE END ADC1_MspInit 0 */
    /* Peripheral clock enable */
    __HAL_RCC_ADC1_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**ADC GPIO Configuration
    PA0     ------> ADC_IN0
    PA1     ------> ADC_IN1
    PA2     ------> ADC_IN2
    */
    GPIO_InitStruct.Pin = VBAT_ADC_Pin|ASL_ADC_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* USER CODE BEGIN ADC1_MspInit 1 */

  /* USER CODE END ADC1_MspInit 1 */
  }

}

/**
 * #!#
* @brief ADC MSP De-Initialization
* This function freeze the hardware resources used in this example
* @param hadc: ADC handle pointer
* @retval None
*/
void HAL_ADC_MspDeInit(ADC_HandleTypeDef* hadc)
{
  if(hadc->Instance==ADC1)
  {
  /* USER CODE BEGIN ADC1_MspDeInit 0 */

  /* USER CODE END ADC1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_ADC1_CLK_DISABLE();

    /**ADC GPIO Configuration
    PA0     ------> ADC_IN0
    PA1     ------> ADC_IN1
    PA2     ------> ADC_IN2
    */
    HAL_GPIO_DeInit(GPIOA, VBAT_ADC_Pin|ASL_ADC_Pin);

  /* USER CODE BEGIN ADC1_MspDeInit 1 */

  /* USER CODE END ADC1_MspDeInit 1 */
  }

}

/**
* @brief I2C MSP Initialization
* This function configures the hardware resources used in this example
* @param hi2c: I2C handle pointer
* @retval None

void HAL_I2C_MspInit(I2C_HandleTypeDef* hi2c)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(hi2c->Instance==I2C1)
  {
    __HAL_RCC_GPIOB_CLK_ENABLE();
    GPIO_InitStruct.Pin = MCU_SCL_Pin|MCU_SDA_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF1_I2C1;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    __HAL_RCC_I2C1_CLK_ENABLE();
  }

}
*/

/**
* @brief I2C MSP De-Initialization
* This function freeze the hardware resources used in this example
* @param hi2c: I2C handle pointer
* @retval None

void HAL_I2C_MspDeInit(I2C_HandleTypeDef* hi2c)
{
  if(hi2c->Instance==I2C1)
  {

    __HAL_RCC_I2C1_CLK_DISABLE();

    HAL_GPIO_DeInit(GPIOB, MCU_SCL_Pin|MCU_SDA_Pin);

  }

}
*/

/**
* @brief TIM_PWM MSP Initialization
* This function configures the hardware resources used in this example
* @param htim_pwm: TIM_PWM handle pointer
* @retval None
*/
void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef* htim_pwm)
{

  if(htim_pwm->Instance==TIM3)
  {
  /* USER CODE BEGIN TIM3_MspInit 0 */

  /* USER CODE END TIM3_MspInit 0 */
    /* Peripheral clock enable */
    __HAL_RCC_TIM3_CLK_ENABLE();
  /* USER CODE BEGIN TIM3_MspInit 1 */

  /* USER CODE END TIM3_MspInit 1 */
  }

}

void HAL_TIM_MspPostInit(TIM_HandleTypeDef* htim)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(htim->Instance==TIM3)
  {
  /* USER CODE BEGIN TIM3_MspPostInit 0 */

  /* USER CODE END TIM3_MspPostInit 0 */

    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**TIM3 GPIO Configuration
    PB5     ------> TIM3_CH2
    */
    GPIO_InitStruct.Pin = GPIO_PIN_5;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF4_TIM3;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* USER CODE BEGIN TIM3_MspPostInit 1 */

  /* USER CODE END TIM3_MspPostInit 1 */
  }

}
/**
* @brief TIM_PWM MSP De-Initialization
* This function freeze the hardware resources used in this example
* @param htim_pwm: TIM_PWM handle pointer
* @retval None
*/

void HAL_TIM_PWM_MspDeInit(TIM_HandleTypeDef* htim_pwm)
{

  if(htim_pwm->Instance==TIM3)
  {
  /* USER CODE BEGIN TIM3_MspDeInit 0 */

  /* USER CODE END TIM3_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_TIM3_CLK_DISABLE();
  /* USER CODE BEGIN TIM3_MspDeInit 1 */

  /* USER CODE END TIM3_MspDeInit 1 */
  }

}

/**
 * @brief I2C MSP Initialization
 *        This function configures the hardware resources used in this example:
 *           - Peripheral's clock enable
 *           - Peripheral's GPIO Configuration
 *           - DMA configuration for transmission request by peripheral
 *           - NVIC configuration for DMA interrupt request enable
 * @param hi2c: I2C handle pointer
 * @retval None
 */
void HAL_I2C_MspInit (I2C_HandleTypeDef *hi2c)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	RCC_PeriphCLKInitTypeDef RCC_PeriphCLKInitStruct;

	/*##-1- Configure the I2C clock source. The clock is derived from the SYSCLK #*/
	RCC_PeriphCLKInitStruct.PeriphClockSelection = RCC_PERIPHCLK_I2Cx;
	RCC_PeriphCLKInitStruct.I2c1ClockSelection = RCC_I2CxCLKSOURCE_SYSCLK;
	HAL_RCCEx_PeriphCLKConfig (&RCC_PeriphCLKInitStruct);

	/*##-2- Enable peripherals and GPIO Clocks #################################*/
	/* Enable GPIO TX/RX clock */
	I2Cx_SCL_GPIO_CLK_ENABLE()
	;
	I2Cx_SDA_GPIO_CLK_ENABLE()
	;
	/* Enable I2Cx clock */
	I2Cx_CLK_ENABLE();

	/*##-3- Configure peripheral GPIO ##########################################*/
	/* I2C TX GPIO pin configuration  */
	GPIO_InitStruct.Pin = I2Cx_SCL_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
	GPIO_InitStruct.Alternate = I2Cx_SCL_SDA_AF;
	HAL_GPIO_Init (I2Cx_SCL_GPIO_PORT, &GPIO_InitStruct);

	/* I2C RX GPIO pin configuration  */
	GPIO_InitStruct.Pin = I2Cx_SDA_PIN;
	GPIO_InitStruct.Alternate = I2Cx_SCL_SDA_AF;
	HAL_GPIO_Init (I2Cx_SDA_GPIO_PORT, &GPIO_InitStruct);

	/* NVIC for I2Cx */
	HAL_NVIC_SetPriority (I2Cx_IRQn, 0, 1);
	HAL_NVIC_EnableIRQ (I2Cx_IRQn);
}
/**
 * @brief I2C MSP De-Initialization
 *        This function frees the hardware resources used in this example:
 *          - Disable the Peripheral's clock
 *          - Revert GPIO, DMA and NVIC configuration to their default state
 * @param hi2c: I2C handle pointer
 * @retval None
 */
void HAL_I2C_MspDeInit (I2C_HandleTypeDef *hi2c)
{
	/*##-1- Reset peripherals ##################################################*/
	I2Cx_FORCE_RESET();
	I2Cx_RELEASE_RESET();

	/*##-2- Disable peripherals and GPIO Clocks #################################*/
	/* Configure I2C Tx as alternate function  */
	HAL_GPIO_DeInit (I2Cx_SCL_GPIO_PORT, I2Cx_SCL_PIN);
	/* Configure I2C Rx as alternate function  */
	HAL_GPIO_DeInit (I2Cx_SDA_GPIO_PORT, I2Cx_SDA_PIN);
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
