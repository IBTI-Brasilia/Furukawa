
#include "i2c_mov.h"
static bool IMU_ON = true;
extern bool sensor_flag;

/*
 * @brief: Funcao responsavel pela leitura dos dados do Acelerometro,
 * @return: {X high, X low, Y high, Y low, Z high, Z low}
 */
uint8_t * Movimento_Acc(void) {

	uint8_t slave_address = LSM_ADDRESS, test = LSM_WHO_AM_I, aux = 0;
	uint8_t reg[6] = {LSM_OUTX_H_A, LSM_OUTX_L_A, LSM_OUTY_H_A, LSM_OUTY_L_A, LSM_OUTZ_H_A, LSM_OUTZ_L_A};
	static uint8_t data[6] = {0};

	IMU_ON = true;
	if(I2C_eCOMMOK(slave_address, test) != I2C_OK)
	{
		IMU_ON = false;
	}
	if(IMU_ON == false)
	{
		sensor_flag = true;
		data[0] = 0xFF;
		data[1] = 0xFF;
		data[2] = 0xFF;
		data[3] = 0xFF;
		data[4] = 0xFF;
		data[5] = 0xFF;

		return data;

		}

		for(int i = 0; i < 6; i++)
		{
			aux |= ((HW_I2C_bWrite((uint16_t)slave_address, &reg[i], 1)) != 1);

			aux |= ((HW_I2C_bRead((uint16_t)slave_address, &data[i], 1)) != 1);
		}

		if(aux != 0) {
			sensor_flag = true;
			data[0] = 0xFF;
			data[1] = 0xFF;
			data[2] = 0xFF;
			data[3] = 0xFF;
			data[4] = 0xFF;
			data[5] = 0xFF;
		}

		return data;

}

/*
 * @brief: Funcao responsavel pela leitura dos dados do Giroscopio,
 * @return: {X high, X low, Y high, Y low, Z high, Z low}
 */
uint8_t * Movimento_Giro(void) {

	uint8_t slave_address = LSM_ADDRESS, test = LSM_WHO_AM_I, aux = 0;
	uint8_t reg[6] = {LSM_OUTX_H_G, LSM_OUTX_L_G, LSM_OUTY_H_G, LSM_OUTY_L_G, LSM_OUTZ_H_G, LSM_OUTZ_L_G};
	static uint8_t data[6] = {0};

	IMU_ON = true;
	if(I2C_eCOMMOK(slave_address, test) != I2C_OK)
	{
		IMU_ON = false;
	}
	if(IMU_ON == false)
	{
		sensor_flag = true;
		data[0] = 0xFF;
		data[1] = 0xFF;
		data[2] = 0xFF;
		data[3] = 0xFF;
		data[4] = 0xFF;
		data[5] = 0xFF;

		return data;

		}

		for(int i = 0; i < 6; i++)
		{
			aux |= ((HW_I2C_bWrite((uint16_t)slave_address, &reg[i], 1)) != 1);

			aux |= ((HW_I2C_bRead((uint16_t)slave_address, &data[i], 1)) != 1);
		}

		if(aux != 0) {
			sensor_flag = true;
			data[0] = 0xFF;
			data[1] = 0xFF;
			data[2] = 0xFF;
			data[3] = 0xFF;
			data[4] = 0xFF;
			data[5] = 0xFF;
		}

		return data;

}

/* @brief: Funcao de leitura do Sensor de temperatura LSM6DSOTR via I2C
* @param: hi2c1 O HandleTypedef para comunicacao com o protocolo I2C
* @retval: 2 bytes com a medicao da temperatura
*/
uint8_t * Temperatura_Mov(void)
{
	uint8_t slave_address = LSM_ADDRESS, test = LSM_WHO_AM_I, aux = 0;
	uint8_t reg[2] = {LSM_OUT_TEMP_H, LSM_OUT_TEMP_L};
	// primeiro high - depois o low;
	static uint8_t data[2] = {0};


	if(I2C_eCOMMOK(slave_address, test) != I2C_OK)
	{
		IMU_ON = false;
	}
//		if(HW_I2C_bWrite((uint16_t)slave_address, conf_g, 2) == 1)
//		{
//		IMU_ON = true;
//		}
//	else
//	{
//		IMU_ON = false;
//	}
	if(IMU_ON == false)
	{
		sensor_flag = true;
		data[0] = 0xFF;
		data[1] = 0xFF;

		return data;
	}

	for(int i = 0; i < 2; i++)
	{
		// SOLICITANDO
		aux |= ((HW_I2C_bWrite((uint16_t)slave_address, &reg[i], 1)) != 1);
		// RECEBENDO
		aux |= ((HW_I2C_bRead((uint16_t)slave_address, &data[i], 1)) != 1);
	}

	if(aux != 0) {
		sensor_flag = true;
		data[0] = 0xFF;
		data[1] = 0xFF;
	}


	return data;
}

/**
 * @brief: Funcao de configuracao das interrupcoes do sensor de movimento LSM6DSOTR via I2C
 * @param: none
 * @retval: None
 */
void setIRQ_movimento(void)
{
	// Registradores de interrupcao
	uint8_t slave_address = LSM_ADDRESS;

	if(IMU_ON == false)
	{
		sensor_flag = true;
		return;

	}
	uint8_t aux = 0;
	uint8_t regWho = LSM_WHO_AM_I;
	uint8_t whoIam;
	HAL_Delay(25);
	aux |= ((HW_I2C_bWrite((uint16_t)slave_address, &regWho, 1)) != 1);
	// RECEBENDO
	aux |= ((HW_I2C_bRead((uint16_t)slave_address, &whoIam, 1)) != 1);


	if(aux != 0) {
		sensor_flag = true;
		return;
	}

	// IRQ1
	uint8_t lsm_md1_cfg[2] = {LSM_MD1_CFG, 0x14}; // int1 6D/4D
	uint8_t lsm_tap_cfg2[2] = {LSM_TAP_CFG2, 0x80}; // enable 6D/4D, free-fall, wake-up, tap, inactivity
	uint8_t lsm_tap_ths_6d[2] = {LSM_TAP_THS_6D, 0x60}; // threshold 50 degrees

	// Selecionando a interrupcao a ser utilziada na porta IRQ1
	HW_I2C_bWrite( (uint16_t)slave_address, lsm_md1_cfg, 2);

	// Ativando as interupcoes basicas, entre elas a 6D que sera utilizada
	HW_I2C_bWrite( (uint16_t)slave_address, lsm_tap_cfg2, 2);

  // Configuracao do threshold para a interrupcao 6D
	HW_I2C_bWrite( (uint16_t)slave_address, lsm_tap_ths_6d, 2);

	uint8_t lsm_odr_g_slp_cfg[2];
	uint8_t lsm_ulp_en_cfg[2];
	// se lsdm
	if (whoIam == 0x6A) {
		// Sleep mode enable gyro
		lsm_odr_g_slp_cfg[0] = LSM_CTRL4_G;
		lsm_odr_g_slp_cfg[1] = 0x40;
	} // lsm6ds0
	else if (whoIam == 0x6C) {
		// Ultra-low-power enable
		lsm_ulp_en_cfg[0] = LSM_CTRL5_C;
		lsm_ulp_en_cfg[1] = 0xE0;
		// Ativando ultra-low-power mode
		HW_I2C_bWrite((uint16_t)slave_address, lsm_ulp_en_cfg, 2);
		// Sleep mode enable gyro
		lsm_odr_g_slp_cfg[0] = LSM_CTRL4_G;
		lsm_odr_g_slp_cfg[1] = 0x80;
	} else {
		// Ultra-low-power enable
		lsm_ulp_en_cfg[0] = LSM_CTRL5_C;
		lsm_ulp_en_cfg[1] =  0xE0;
		// Ativando ultra-low-power mode
		HW_I2C_bWrite((uint16_t)slave_address, lsm_ulp_en_cfg, 2);
		// Sleep mode enable gyro
		lsm_odr_g_slp_cfg[0] = LSM_CTRL4_G;
		lsm_odr_g_slp_cfg[1] = 0x80;
	}

	// Acelerometro Low-power mode
	uint8_t lsm_lp_en_xl_cfg[2] = {LSM_CTRL6_C, 0x10}; // High-performance disabled - BIT XL_HM_MODE = 1
	uint8_t lsm_odr_xl_cfg[2] = {LSM_CTRL1_XL, 0xB0}; // Output data rate do acelerometro = 1.6Hz (low power)
	uint8_t lsm_lp_en_g_cfg[2] = {LSM_CTRL7_G, 0x80}; // Output data rate do gyroscopio =  High-performance disabled - BIT G_HM_MODE = 1
	uint8_t lsm_odr_g_cfg[2] = {LSM_CTRL2_G, 0x00}; // Giroscopio em off mode

	// Desativando modo de alta performance - G
	HW_I2C_bWrite((uint16_t)slave_address, lsm_lp_en_g_cfg, 2);
	// Sleep mode - Gyro
	HW_I2C_bWrite((uint16_t)slave_address, lsm_odr_g_slp_cfg, 2);
	// Desativando modo de alta performance
	HW_I2C_bWrite((uint16_t)slave_address, lsm_lp_en_xl_cfg, 2);
	// Ajustando output data rate do acelerometro para 1.6Hz
	HW_I2C_bWrite((uint16_t)slave_address, lsm_odr_xl_cfg, 2);
	// Giroscopio em off mode
	HW_I2C_bWrite((uint16_t)slave_address, lsm_odr_g_cfg, 2);

	// IRQ2
//	uint8_t lsm_md2_cfg[2] = {LSM_MD2_CFG, 0x02}; // int2 embedded func
//	uint8_t access_embedded[2] = {LSM_FUNC_CFG_ACCESS, 0x80}; // access embedded func
//	uint8_t lsm_emb_func_en_a[2] = {LSM_EMB_FUNC_EN_A, 0x20}; // enable sigmot
//	uint8_t lsm_emb_func_int2[2] = {LSM_EMB_FUNC_INT2, 0x20}; // int2 sigmot
//	uint8_t lsm_emb_func_en_init_a[2] = {LSM_EMB_FUNC_INIT_A, 0x20}; // init sigmot
//	uint8_t leave_embedded[2] = {LSM_FUNC_CFG_ACCESS, 0x00}; // leave embedded func
//	while(HAL_I2C_Master_Transmit( (uint16_t)slave_address, lsm_md2_cfg, 2));
//	while(HAL_I2C_Master_Transmit( (uint16_t)slave_address, access_embedded, 2));
//	while(HAL_I2C_Master_Transmit( (uint16_t)slave_address, lsm_emb_func_en_a, 2));
//	while(HAL_I2C_Master_Transmit( (uint16_t)slave_address, lsm_emb_func_int2, 2));
//	while(HAL_I2C_Master_Transmit( (uint16_t)slave_address, lsm_emb_func_en_init_a, 2));
//	while(HAL_I2C_Master_Transmit( (uint16_t)slave_address, leave_embedded, 2));

}

void setThs_mov(uint8_t ths6D, uint8_t thsFF, uint8_t durFF){
	uint8_t slave_address = LSM_ADDRESS;
	uint8_t config_1, config_2;

	// configuracao igual a zero significa manter a configuracao anterior, entao os valores minimos configuraveis sao 1 para o usuario
	if(ths6D != 0){
		ths6D -= 1;
		uint8_t lsm_6D_ths[2] = {LSM_TAP_THS_6D, ths6D};

		HW_I2C_bWrite( (uint16_t)slave_address, lsm_6D_ths, 2);
	}

	if(thsFF != 0){
		thsFF -= 1;
		uint8_t lsm_FF_ths[2] = {LSM_FREE_FALL, thsFF};

		HW_I2C_bWrite( (uint16_t)slave_address, lsm_FF_ths, 2);
	}

	if(durFF != 0){
		durFF -= 1;

		config_1 = (durFF << 2) & 0x80; //Pega o bit mais significativo e deixa na posicao mais significativa
		config_2 = durFF << 3; 			//Deixa a configuracao ocupando a parte mais significativa e exclui o primeiro bit

		uint8_t lsm_FF_dur1[2] = {LSM_WAKE_UP_DUR, config_1};
		uint8_t lsm_FF_dur2[2] = {LSM_FREE_FALL, config_2};

		HW_I2C_bWrite( (uint16_t)slave_address, lsm_FF_dur1, 2);
		HW_I2C_bWrite( (uint16_t)slave_address, lsm_FF_dur2, 2);
	}
}
