/*
 * *****************************************************************************************************************************
 * @file:    hw_iwdg.h                                                                                                             *
 * @author:  Rennan                                                                                                            *
 * @version: v1.0                                                                                                              *
 * @date:    21 de nov de 2019                                                                                                       *
 * @brief:                                                              *
 * *****************************************************************************************************************************
 *
 */

/* Includes ------------------------------------------------------------------*/
#include "hw_iwdg.h"
#include "lora.h"

/* Private variables ---------------------------------------------------------*/
static IWDG_HandleTypeDef hiwdg;


void HW_IWDG_Init(void)
{

  /* USER CODE BEGIN IWDG_Init 0 */

  /* USER CODE END IWDG_Init 0 */

  /* USER CODE BEGIN IWDG_Init 1 */

  /* USER CODE END IWDG_Init 1 */
  hiwdg.Instance = IWDG;
  hiwdg.Init.Prescaler = IWDG_PRESCALER_256;
  hiwdg.Init.Window = IWDG_WINDOW_DISABLE;
  hiwdg.Init.Reload = 4095;
  if (HAL_IWDG_Init(&hiwdg) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN IWDG_Init 2 */

  /* USER CODE END IWDG_Init 2 */

}

void HW_IDWDG_Refresh(void)
{
	if (LORA_JoinStatus () != LORA_SET)
	{
		/*Not joined, try again later*/
		LORA_Join();
		return;
	}
	HAL_IWDG_Refresh(&hiwdg);
}

void HW_IDWDG_Refresh_Simple(void)
{
	HAL_IWDG_Refresh(&hiwdg);
}
