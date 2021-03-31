/*
 * i2c_lum.h
 *
 *  Created on: 12 de jul de 2019
 *      Author: IBTI
 */

#ifndef INC_I2C_LUM_H_
#define INC_I2C_LUM_H_

#include "hw_i2c.h"

// Sensor de Luz I2C
#define	RPR_ADDRESS         0x70			  // Endereco do Sensor de Luz (Slave address) 0111000 | hex: 0x38 foi tambem deslocado para a esquerda, portanto hex: 0x70 | b: 01110000
#define RPR_CONF            0x41			  // Registrador onde deve ser gravado o modo de operacao do periferico
#define	RPR_REG_L           0x46			  // Registrador que contem o byte menos significante do valor medido de Luz ambiente
#define	RPR_REG_H           0x47			  // Registrador que contem o byte mais significante do valor medido de Luz ambiente
#define RPR_INTERRUPT	    0x4A			  // Registrador de configuracao de interrupcoes
#define RPR_PS_TH_LSB       0x4B		      // Registrador com limiar alto para medicao PS (bits menos significativos)
#define RPR_PS_TH_MSB       0x4C			  // Registrador com limiar alto para medicao PS (bits mais significativos)
#define RPR_PS_TL_LSB       0x4D		      // Registrador com limiar baixo para medicao PS (bits menos significativos)
#define RPR_PS_TL_MSB       0x4E		      // Registrador com limiar baixo para medicao PS (bits mais significativos)
#define RPR_ALS_TH_LSB      0x4F			  // Registrador com limiar alto para medicao ALS (bits menos significativos)
#define RPR_ALS_TH_MSB      0x50			  // Registrador com limiar alto para medicao ALS (bits mais significativos)
#define RPR_ALS_TL_LSB      0x51			  // Registrador com limiar baixo para medicao ALS (bits menos significativos)
#define RPR_ALS_TL_MSB      0x52			  // Registrador com limiar baixo para medicao ALS (bits mais significativos)

typedef enum ALS_State
{
	/*
	 * @brief: ALS não habilitado
	 */
	ALS_STANDBY,
	/*
	 * @brief: ALS habilitado
	 */
	ALS_ENABLE
}ALS_State;

typedef enum ALS_Measure_time
{
	/*
	 * @brief: Mede a luz por 100 milissegundos
	 */
	MT_100ms,
	/*
	 * @brief: Mede a luz por 100 milissegundos e dorme por 300 milissegundos
	 */
	MT_100ms_ST_300ms,
	/*
	 * @brief: Mede a luz por 400 milissegundos
	 */
	MT_400ms
}ALS_Measure_time;

typedef struct ALS_HandleTypedef
{
	ALS_State                        estado;

	ALS_Measure_time                 timing;

}ALS_HandleTypedef;
/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------*/


/* read luminosity data*/
uint8_t * Luminosidade(void);
/* initialize luminosity sensor interruption*/
void initIRQ_luminosidade(void);
/* configure luminosity sensor interruption*/
void setIRQ_luminosidade(uint32_t th);


#endif /* INC_I2C_MOV_H_ */
