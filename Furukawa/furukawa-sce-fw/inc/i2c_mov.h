/*
 * i2c_mov.h
 *
 *  Created on: 12 de jul de 2019
 *      Author: IBTI
 */

#ifndef INC_I2C_MOV_H_
#define INC_I2C_MOV_H_

#include "hw_i2c.h"


// Sensor de Movimento I2C
#define	LSM_ADDRESS         0xD4			  // Endereco do Sensor de Movimento (Slave address) 11010100 | hex: 0xD5 SA0 = 0 (Ground), R/W = 0
#define LSM_OUT_TEMP_L      0x20			  // Registrador que contem o byte menos significante do valor medido de Temperatura
#define LSM_OUT_TEMP_H      0x21			  // Registrador que contem o byte mais significante do valor medido de Temperatura
#define LSM_OUTX_L_G        0x22			  // Registrador que contem o byte menos significante do valor medido de Velocidade Angular (Giroscopio) no eixo X
#define LSM_OUTX_H_G        0x23              // Registrador que contem o byte mais significante do valor medido de Velocidade Angular (Giroscopio) no eixo X
#define LSM_OUTY_L_G        0x24			  // Registrador que contem o byte menos significante do valor medido de Velocidade Angular (Giroscopio) no eixo Y
#define LSM_OUTY_H_G        0x25              // Registrador que contem o byte mais significante do valor medido de Velocidade Angular (Giroscopio) no eixo Y
#define LSM_OUTZ_L_G        0x26			  // Registrador que contem o byte menos significante do valor medido de Velocidade Angular (Giroscopio) no eixo Z
#define LSM_OUTZ_H_G        0x27              // Registrador que contem o byte mais significante do valor medido de Velocidade Angular (Giroscopio) no eixo Z
#define LSM_OUTX_L_A        0x28			  // Registrador que contem o byte menos significante do valor medido de Aceleracao Linear (Acelerometro) no eixo X
#define LSM_OUTX_H_A        0x29              // Registrador que contem o byte mais significante do valor medido de Aceleracao Linear (Acelerometro) no eixo X
#define LSM_OUTY_L_A        0x2A			  // Registrador que contem o byte menos significante do valor medido de Aceleracao Linear (Acelerometro) no eixo Y
#define LSM_OUTY_H_A        0x2B              // Registrador que contem o byte mais significante do valor medido de Aceleracao Linear (Acelerometro) no eixo Y
#define LSM_OUTZ_L_A        0x2C			  // Registrador que contem o byte menos significante do valor medido de Aceleracao Linear (Acelerometro) no eixo Z
#define LSM_OUTZ_H_A        0x2D              // Registrador que contem o byte mais significante do valor medido de Aceleracao Linear (Acelerometro) no eixo Z
// Configuracao IRQ1 6D/4D
#define LSM_MD1_CFG         0x5E			  // Registrador que multiplexa a interrupcao 6D/4D para o pino IRQ1
#define LSM_TAP_CFG2        0x58		      // Registrador que ativa as funcoes de interrupcao basicas (6D/4D, free-fall, wake-up, tap, inactivity)
#define LSM_TAP_THS_6D      0x59		      // Registrador que estabelece o limiar para interrupcao de 6D/4D
#define LSM_WAKE_UP_DUR		0x5C			  // Registrador que contem 1 bit para configurar duracao de free-fall
#define LSM_FREE_FALL		0x5D			  // Registrador que configura duracao e threshold de free-fall
// Configuracao IRQ2 SIGMOT
#define LSM_MD2_CFG         0x5F			  // Registrador que multiplexa a funcao sigmot para o pino IRQ1
#define LSM_FUNC_CFG_ACCESS 0X01			  // Registrador de acesso para funcoes embarcadas
#define LSM_EMB_FUNC_EN_A   0x12			  // Registrador que ativa a funcao de movimento significativo
#define LSM_EMB_FUNC_INT2	0x0E			  // Registrador que multiplexa a interrupcao sigmot para o pino IRQ2
#define LSM_EMB_FUNC_INIT_A	0x66			  // Registrador que inicializa a funcao de sigmot
#define LSM_WHO_AM_I		0x0F
// Configuracao Ultra-low-power mode
#define LSM_CTRL5_C			0x14			  // Registrador que habilita low-power mode para o acelerometro
#define LSM_CTRL6_C			0x15			  // Registrador que desabilita o modo de alta performance no acelerometro
#define LSM_CTRL1_XL		0x10			  // Registrador que controla output rate do acelerometro
#define LSM_CTRL4_G			0x13			  // Registrador que habilita sleep mode para o giroscopio
#define LSM_CTRL7_G			0x16			  // Registrador que habilita low-power mode para o giroscopio
#define LSM_CTRL2_G			0x11			  // Registrador que controla output rate do giroscopio

/* read giroscope data */
uint8_t * Movimento_Giro(void);
uint8_t * Movimento_Acc(void);
/* read temp data 8 */
uint8_t * Temperatura_Mov(void);
/* configure moviment sensor interruption*/
void setIRQ_movimento(void);

void setThs_mov(uint8_t ths6D, uint8_t thsFF, uint8_t durFF);

#endif /* INC_I2C_MOV_H_ */
