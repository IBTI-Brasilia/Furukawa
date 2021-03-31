/*
 * i2c_lum.c
 *
 *  Created on: 12 de jul de 2019
 *      Author: IBTI
 */

#include "i2c_lum.h"

static ALS_HandleTypedef als1;
static bool ALS_ON = true;

extern bool sensor_flag;
/**
 * @brief: Funcao de leitura do sensor de luminosidade RPR-0521RS via I2C
 * @param: hi2c1 O HandleTypedef para comunicacao com o protocolo I2C
 * @retval: 2 bytes com a medicao da luminosidade
 */
uint8_t * Luminosidade(void)
{
	uint8_t slave_address = RPR_ADDRESS;
	bool aux = 0;
	uint8_t reg_luminosity_high = RPR_REG_H, reg_luminosity_low = RPR_REG_L;
	uint8_t luminosity_high, luminosity_low;
	static uint8_t data[2];


	if(!ALS_ON){
		sensor_flag = true;
		data[0] = 0xFF;
		data[1] = 0xFF;
		return data;

	}

	aux |= ((HW_I2C_bWrite( (uint16_t)slave_address, &reg_luminosity_high, 1)) != 1);
	aux |= ((HW_I2C_bRead( (uint16_t)slave_address, &luminosity_high, 1)) != 1);

	aux |= ((HW_I2C_bWrite( (uint16_t)slave_address, &reg_luminosity_low, 1)) != 1);
	aux |= ((HW_I2C_bRead( (uint16_t)slave_address, &luminosity_low, 1)) != 1);

	data[0] = luminosity_high;
	data[1] = luminosity_low;

	if(aux != 0) {
		sensor_flag = true;
		data[0] = 0xFF;
		data[1] = 0xFF;
	}

	return data;
}

/**
 * @brief: Funcao de inicializacao das interrupcoes do sensor de luminosidade RPR-0521RS via I2C
 * @param: hi2c1 O HandleTypedef para comunicacao com o protocolo I2C
 * @retval: None
 */
void initIRQ_luminosidade(void)
{
	als1.estado = ALS_ENABLE;
	als1.timing = MT_100ms_ST_300ms;
	uint8_t test = RPR_CONF;
	uint8_t conf[2] = {0};
	uint8_t slave_address = RPR_ADDRESS;
	uint8_t rpr_int[2] = {RPR_INTERRUPT, 0x0E}; // RR00 1110 (mode 00, assert 1, unlatch 1, trig als e ps 10)
	// uint8_t rpr_int[2] = {RPR_INTERRUPT, 0x00}; // MODO RESET ->  RR00 0000 (mode 00, assert 0, unlatch 0, trig als e ps 00)


	if(I2C_eCOMMOK(slave_address, test) != I2C_OK)
	{
		ALS_ON = false;
	}
	else if(als1.estado != ALS_ENABLE)
	{
		ALS_ON = false;
		conf[0] = RPR_CONF;
		conf[1] = 0x00;

    }
	else {
		switch(als1.timing)
		{
			case MT_100ms:
				conf[0] = RPR_CONF;
				conf[1] = 0x85;
				break;
			case MT_100ms_ST_300ms:
				conf[0] = RPR_CONF;
				conf[1] = 0x89;
				break;
			case MT_400ms:
				conf[0] = RPR_CONF;
				conf[1] = 0x8A;
				break;
		}
		ALS_ON = true;
	}

	// Configuracao do sensor de luminosidade
	HW_I2C_bWrite( (uint16_t)slave_address, conf, 2);

	if(ALS_ON == false)
	{
		sensor_flag = true;
		return;

	}

	// Gravando a interrupcao no registrador de interrupcao
	HW_I2C_bWrite( (uint16_t)slave_address, rpr_int, 2);


}

/**
 * @brief: Funcao de configuracao das interrupcoes do sensor de luminosidade RPR-0521RS via I2C
 * @param: hi2c1 O HandleTypedef para comunicacao com o protocolo I2C
 * @retval: None
 */
void setIRQ_luminosidade(uint32_t th)
{

	uint8_t th_lsb, th_msb;


	th_lsb = (uint8_t)(th & 0x000000FF);
	th_msb = (uint8_t)((th & 0x0000FF00) >> 8);


	uint8_t slave_address = RPR_ADDRESS;

	uint8_t rpr_als_th_low[2] =  {RPR_ALS_TH_LSB, th_lsb}; // threshold high als
	uint8_t rpr_als_th_high[2] =  {RPR_ALS_TH_MSB, th_msb};

	if(ALS_ON == false)
	{
		sensor_flag = true;
		return;

	}


	// Grava o limiar de interrupcao para medicao als
	HW_I2C_bWrite( (uint16_t)slave_address, rpr_als_th_low, 2);
	HW_I2C_bWrite( (uint16_t)slave_address, rpr_als_th_high, 2);

}
