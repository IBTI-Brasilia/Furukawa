/*
 * *****************************************************************************************************************************
 * @file:    hw_i2c.c                                                                                                             *
 * @author:  Rennan                                                                                                            *
 * @version: v1.0                                                                                                              *
 * @date:    14 de mai de 2019                                                                                                       *
 * @brief:                                                              *
 * *****************************************************************************************************************************
 *
 */

/* Includes ------------------------------------------------------------------*/
#include "hw_i2c.h"

extern bool sensor_flag;

/* Private variables ---------------------------------------------------------*/
static I2C_HandleTypeDef hi2c1;


void HW_I2C_Init(void)
{

/*
 * @brief: Configuração do barramento I2C
 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.Timing = 0x0030052D;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Analogue filter
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Digital filter
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0) != HAL_OK)
  {
    Error_Handler();
  }

 /*
  * @brief: Configuração do Sensor de Luz
  */

}

uint8_t HW_I2C_bWrite(uint16_t address, uint8_t *reg, uint16_t size)
{
	uint8_t t = 0;
	for (t = 0; t < 5; t++) {
		if(HAL_I2C_Master_Transmit(&hi2c1, address, reg, size, 100) == HAL_OK)
		{
			return 1;
		}
	}
	return 0;
}

uint8_t HW_I2C_bRead(uint16_t address, uint8_t *data, uint16_t size)
{
	uint8_t t = 0;
	for (t = 0; t < 5; t++) {
		if(HAL_I2C_Master_Receive(&hi2c1, address, data, size, 100) == HAL_OK)
		{
			return 1;
		}
	}
	return 0;
}

/*
 * @brief: Funï¿½ï¿½o que detecta o funcionamento do perifï¿½rico I2C
 * @param: addr Endereï¿½o do Perifï¿½rico
 * @param: reg Registrador de Teste
 * @retval: Status da comunicaï¿½ï¿½o I2C
 */
eI2C_status I2C_eCOMMOK(uint8_t addr, uint8_t reg)
{
	eI2C_status status;
	uint8_t data;
	bool check = true;
	for(int i = 0; i < 5; i++)
	{
		if(HW_I2C_bWrite((uint16_t)addr, &reg, 1) == 1)
		{
			if(HW_I2C_bRead((uint16_t)addr, &data, 1) == 1)
			{
				check = true;
				break;
			}
			else
			{
				check = false;
			}
		}
		else
		{
			check = false;
		}
		HAL_Delay(5);
	}
	if(check == true)
	{
		status = I2C_OK;
	}
	else
	{
		status = I2C_NOK;
	}
	return status;


}


/**
 * @brief: Nome autoexplicativo
 *
 * @param: p O ponteiro
 * @param: v O vetor
 * @param: size O tamanho
 * @retval: 2 bytes com a medicao do acelerometro no eixo Z
 */
void ponteiro_pra_vetor(uint8_t *p, uint8_t v[], uint32_t size)
{
	for(int i =0; i < size; i++)
	{
		v[i] = *(p + i);
	}
}
