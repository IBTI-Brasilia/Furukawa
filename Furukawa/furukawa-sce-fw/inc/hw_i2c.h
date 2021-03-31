/*
 * *****************************************************************************************************************************
 * @file:    hw_i2c.h                                                                                                             *
 * @author:  Renna                                                                                                            *
 * @version: v1.0                                                                                                              *
 * @date:    14 de mai de 2019                                                                                                       *
 * @brief:                                                              *
 * *****************************************************************************************************************************
 *
 */

#ifndef INC_HW_I2C_H_
#define INC_HW_I2C_H_

/* Includes ------------------------------------------------------------------*/
#include "hw.h"
#include "lora.h"
#include "timeServer.h"
#include "vcom.h"
#include "version.h"
#include <stdbool.h>
#include <stdio.h>
/* Private define ------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------*/


/*--TypeDefs-----------------------------------------------------------------------------------------------------------------------------------------------------------*/

typedef enum eI2C_status
{
/*
 * @brief: I2C est� funcionando corretamente
 */
	I2C_OK,
/*
 * @brief: I2C n�o est� funcionando corretamente
 */
	I2C_NOK
}eI2C_status;

/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------*/


/* Private function prototypes -----------------------------------------------*/

void HW_I2C_Init(void);

uint8_t HW_I2C_bWrite(uint16_t address, uint8_t *reg, uint16_t size);

uint8_t HW_I2C_bRead(uint16_t address, uint8_t *data, uint16_t size);

eI2C_status I2C_eCOMMOK(uint8_t addr, uint8_t reg);

/* pointer to vector*/
void ponteiro_pra_vetor(uint8_t *p, uint8_t v[], uint32_t size);

#endif /* INC_HW_I2C_H_ */
