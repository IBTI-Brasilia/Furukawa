/******************************************************************************
* @file    main.c
* @author  MCD Application Team
* @version V1.1.4
* @date    08-January-2018
* @brief   this is the main!
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
#include "low_power_manager.h"
#include "lora.h"
#include "timeServer.h"
#include "vcom.h"
#include "version.h"
#include "main.h"
#include "hw_i2c.h"
#include "i2c_lum.h"
#include "i2c_mov.h"
#include "hw_i2c.h"
#include "flash_eraseprogram.h"
#include "utils.h"
#include "stm32l0xx_it.h"
#include "led.h"

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/
// LORA
/*!
* LoRaWAN Adaptive Data Rate
* @note Please note that when ADR is enabled the end-device should be static
*/
#define LORAWAN_ADR_STATE LORAWAN_ADR_OFF
/*!
* LoRaWAN Default data Rate Data Rate
* @note Please note that LORAWAN_DEFAULT_DATA_RATE is used only when ADR is disabled
*/
#define LORAWAN_DEFAULT_DATA_RATE DR_0
/*!
* LoRaWAN application port
* @note do not use 224. It is reserved for certification
*/
#define LORAWAN_APP_PORT                            8
/*!
* Number of trials for the join request.
*/
#define JOINREQ_NBTRIALS                            3
/*!
* LoRaWAN default endNode class port
*/
#define LORAWAN_DEFAULT_CLASS                       CLASS_A
/*!
* LoRaWAN default confirm state
*/
#define LORAWAN_DEFAULT_CONFIRM_MSG_STATE           LORAWAN_UNCONFIRMED_MSG
/*!
* User application data buffer size
*/
#define LORAWAN_APP_DATA_BUFF_SIZE                           64

/*!
* User application data
*/
static uint8_t AppDataBuff[LORAWAN_APP_DATA_BUFF_SIZE];
// FSM
#define IDLE  				0  // Normal (1 mensagem/dia)
#define WARN  				1  // Alerta (1 mensagem/minuto)
#define WARN2 				3  // Alerta + mensagem diaria pendente
#define rIDLE 				5  // Retorna normal + mensagem scheduled
#define DOWN_MEDIDAS	 	6  // solicitacao de algumas medidas
#define DOWN_CONF_SW	 	7  // conf de timers e limiares [SW]
#define DOWN_CONF_HW	 	8  // conf de hardware
#define KEEP_ALV_FLAG		9  // sms de emergencia apos keep alive com flag
#define FAIL_CONFIG			10 // falha de configuracao
#define WARN_END_KEEP_FLAG	11 // sms de TERMINO de warn com a sms de um keep alive
#define FORCE_KEEP_ALIVE	12 // force keep alive
// Portas usadas
#define KA_PORT				 1 // Porta usada para enviar Keep Alive
#define WARN_PORT			 2 // Porta usada para enviar mensangens de alarme
#define SENS_PORT			 3 // Porta usada para enviar as mediadas dos sensores, quando requisitado
#define HW_CONF_PORT		 5 // Porta usada para enviar as configuracoes de HardWare
#define SW_CONF_PORT		 6 // Porta usada para enviar as configuracoes de SoftWare
#define FAIL_PORT			 9 // Porta usada para enviar mensagens de erro de configuracao
#define MULT_DL_PORT	 	 10 // Porta usada para enviar downlink multiplo

// EEPROM ADRESS
// flag e alerta
#define FLAG_ADDRESS	 							0x8080000 // endereco inicial da eeprom - uma palavra 2 bytes
#define KEEP_ALIVE_TIME_ADD	 						0x8080004 // endereco eeprom do tempo de keep alive
#define ALERT_TX_WARN_TIME_ADD		 				0x8080008 // endereco eeprom do tempo do periodo de envio de alerta **FREQUENCIA**
#define WARN_TOTAL_TIME_PERIOD_ADD			 		0x808000C // endereco eeprom do tempo do periodo TOTAL de alarme ***
// limiares
#define THS_LUM_ADD	 								0x8080010 // endereco do limite de luminosidade
#define THS_BAT_ADD	 								0x8080014 // endereco do limite de bateria
#define THS_ACC_ADD		 							0x8080018 // endereco do limite do aceleromentro
#define THS_ADR_ADD			 						0x808001C // endereco com o byte ADDR.
// endereco eeprom de informacoes de fabrica
#define COID_PRD_ADD								0x8080020 // endereco do codigo de produto 5 digitos [5 hexas]
#define NUM_SERIE_ADD								0x8080024 // endereco do numero de serie 6 digitos [6 hexas]
#define DATE_FAB_ADD								0x8080028 // endereco do dado de data dd_mm_aa [6 hexas]

// endereco da versao de hardware e de firmware
#define HW_VER_ADD									0x8080030 // endereco da informacoes relativas a software e hardware
#define SW_VER_ADD									0x8080034 // endereco da informacoes relativas a software e hardware

#define FAULT_COUNTER_ADD 							0x8080040 // endereco do contador de faultas ( excecoes ) gravado na eeprom

// DEFINES DA SERIAL
// para uso do protocolo
#define SERIAL_INIT_FLAG 							0xE5 // flag que esta no byte zero do que for recebido
#define SERIAL_OK									0xAA // indica que mensagem esta certa
#define SERIAL_ERRO_LEITURA							0xF3 // mesagem que indica erro ao ler
#define SERIAL_ERRO_ESCRITA		 					0xF7 // carga util da mensagem que indica erro de escrita
#define SERIAL_DATA_LEN_DEFAULT  					0x01 // tamanho da carga util do pacote de informacao
#define SERIAL_DATA_SEND_ALL	 					0x28 // tamanho da carga util do pacote contendo todos os retornos possiveis
#define SERIAL_DATA_SEND_ALMOST	 					0x10 // tamanho da carga util do pacote contendo quase todas informacoes
#define SERIAL_TOTAL_LEN_DFT						0x05 // tamanho total do pacote de informacao


// flag eeprom

# define FLAG_VALUE 								0xFEFAE4B3

#define TESTE_PLACAS 								0

#if TEST_MODE

/*!
* Defines the application data transmission duty cycle, value in [ms].
*/
#define KEEP_ALIVE_DEFAULT 		  	10010U // 10 segundos

#elif TESTE_PLACAS
/*!
* Defines the application data transmission duty cycle, value in [ms].
*/
#define KEEP_ALIVE_DEFAULT 		  	180000U // 3 minutos e 0 s
#else
#define KEEP_ALIVE_DEFAULT 		  	3600000 // 1 hora
#endif
/*!
* Defines the total period of warning state, value in [ms].
*/
#if TESTE_PLACAS
#define WARN_PERIOD_DEFAULT  		120000U // 2 minutos e 0 seg
#else
#define WARN_PERIOD_DEFAULT  		300010U // 5 minutos
#endif
/*!
*  Defines the warning state transmission frequency, value in [ms].
*/

#if TESTE_PLACAS
#define WARN_FREQ_TX_DEFAULT  	   		30000U	//30 segundos
#else
#define WARN_FREQ_TX_DEFAULT  		20000U	//20 segundos
#endif
/*!
*  Defines the timer between keep alive pkt with flag and the warning pkt [ms].
*/
#define SEND_PKTWARN_TIMER  	   		15000U	//15 segundos

/*!
*  Defines the timer between keep alive pkt with flag and the warning pkt [ms].
*/
#define SERIAL_TIMER  	   		10000U	//10 segundos

/*!
*  Defines the default luminosisy threshold.
*/
#if TESTE_PLACAS
#define LUM_THS_DEFAULT  	   	700U	//10 lux
#else
#define LUM_THS_DEFAULT  	   	10U	//700 lux
#endif

#define VREFINT_CAL_ADDR                   0x1FF80078U

/*!
*  Defines the default moviment 6D threshold.
*/
#if TESTE_PLACAS
#define MOV_6D_THS_DEFAULT  	   		0x04	//sensitivity: from 0x01 (80) to 0x04 (50)
#else
#define MOV_6D_THS_DEFAULT  	   		0x04	//sensitivity: from 0x01 (80) to 0x04 (50)
#endif

/*!
*  Defines the default moviment 6D threshold.
*/
#if TESTE_PLACAS
#define MOV_FF_THS_DEFAULT  	   		0x01	// from 0x01 (156 mg) to 0x08 (500 mg)
#else
#define MOV_FF_THS_DEFAULT  	   		0x01	// from 0x01 (156 mg) to 0x08 (500 mg)
#endif
/*!
*  Defines the default moviment 6D threshold.
*/
#if TESTE_PLACAS
#define MOV_FF_DUR_DEFAULT  	   		0x01	//from 0x01 (0 seg) to 0x40 (39.375 seg)
#else
#define MOV_FF_DUR_DEFAULT  	   		0x01	//from 0x01 to 0x40
#endif
/*!
*  Defines the default voltage limit
*/
#if TESTE_PLACAS
#define VOLT_THS_DEFAULT  	   		2.0	//2.0 Volts
#else
#define VOLT_THS_DEFAULT  	   		2.0	//2.0 Volts
#endif

/*!
* Defines one day in [ms].
*/
#define DAY 86400000

#define HW_VERSION 				0x30
#define SW_VERSION 				0x301 // 3.0.1 atualizado 06/06/2020

/*!
* Defines the application data transmission duty cycle, value in [ms].
*/
uint32_t APP_TX_DUTYCYCLE;
/* Defines the total time warning state.
*/
uint32_t WARN_PERIOD;
/*!
* Defines the warning state Tx (packet frequency send), value in [ms].
*/
uint32_t WARN_TX_SEND;

/*!
* Defines the watchdog duty cycle, value in [ms].
*/
uint32_t WD_DUTYCYCLE = 25000;	//25 segundos

uint8_t control_serial = 1; // variavel de controle para continuar ou nao no modo serial

/*Variaveis padroes de cod prod, num serie e data fab*/
uint32_t CODIGO_PRODUTO = 64521;

uint32_t NUMERO_SERIE = 331705;

uint32_t DATA_FAB = (31 << 12) | (01 << 8) | (20);

uint8_t CONFIG_ERROR = 0x00; // usado para indicar qual erro houve (downlink)

/* Private variables ---------------------------------------------------------*/ 

// LORA
/*!
* User application data structure
*/
static lora_AppData_t AppData = { AppDataBuff, 0, 0 };
// SENSOR
/*!
* Battery level (0 to 254)
*/
uint8_t batteryLevel_mV;
/*!
* Battery voltage [V]
*/
uint16_t battery_V;
/*!
* Battery warning limit in Volts
*/
float battery_limit;

// INT
/*!
* External interruption flag
*/
int exti_flag = 0;
/*!
* Luminosity interruption flag
*/
int lum_gflag = 0;
/*!
* Moviment interruption flag
*/
int mov_gflag = 0;

/*!
* Sensor fault flag
*/
bool sensor_flag = false;

/*!
temp celsius  8
*/
uint16_t temp_C;

/*!
* luminosidade warning limit lower
*/
uint32_t luminosity_limit = LUM_THS_DEFAULT;
/*!
* moviment 6D warning limit
*/
uint8_t mov_6D_limit = MOV_6D_THS_DEFAULT;
/*!
* moviment free-fall warning limit threshold
*/
uint8_t mov_FF_ths = MOV_FF_THS_DEFAULT;
/*!
* moviment free-fall warning limit duration
*/
uint8_t mov_FF_duration = MOV_FF_DUR_DEFAULT;

// HW
/*!
* ADC (Battery)
*/
ADC_HandleTypeDef hadc;

UART_HandleTypeDef huart1;

/*!
* hwwdg (Window WatchDog)
*/
//IWDG_HandleTypeDef hiwdg;
/* Private macro -------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

// LORA
/* call back when LoRa endNode has received a frame*/
static void LORA_RxData (lora_AppData_t *AppData);
/* call back when LoRa endNode has just joined*/
static void LORA_HasJoined (void);
/* call back when LoRa endNode has just switch the class*/
static void LORA_ConfirmClass (DeviceClass_t Class);
/* LoRa endNode send request*/
static void Send (uint32_t down_op);
/* finite state machine logic*/
static void fsm(int next_state);
/* interruption callback function*/
void interruption_callback(void);
/* tx timer callback function*/
static void OnKeepAliveEvent (void);
/* start the tx process*/
static void LoraStartTx (TxEventType_t EventType, uint8_t change);
/* watchdog feed callback function*/
static void OnWDTimerEvent (void);
/* start the warning state*/
static void WDStart (TxEventType_t EventType);
/* warning state callback function*/
static void OnWarnTxTimerEvent (void);
/* start the warning state*/
static void WarnStart (TxEventType_t EventType, uint8_t change);
/* warning state timeout callback function*/
static void OnWarnEndPeriodEvent (void);
/* start the warning state timeout*/
static void WarnPeriodStart (TxEventType_t EventType, uint8_t change);
/* warning packet after keep alive flag callback function*/
static void SendWarnSMSFlag (void);
/* start the 15 seconds after non-interrupt flag*/
static void Flag15sStart (TxEventType_t EventType);
/* keep alive packet after end warn period callback function*/
static void SendEndWarnSMS (void);
/* start the keep alive flag state timeout*/
static void EndWarnPeriodStart (TxEventType_t EventType);
/*finish serial time, not possible to do serial command any more*/
static void FinishSerialTimer (void);
/*start the timer resp for serial open */
static void SerialTimerStart (TxEventType_t EventType);

/*----- Funcoes de Leitura -------------*/

/* leitura da bateria e assinalar flag */
uint8_t leitura_bateria(uint32_t * i, int only_flag );
/* leitura da temperatura*/
void leitura_temp(uint32_t * i, uint8_t only1);
/* leitura da luminosidade */
void leitura_luminosidade(uint32_t * i, uint8_t only1);
/* leitura do sensor de aceleracao */
void leitura_acc(uint32_t * i);
/* leitura das flags*/
void leitura_flags(uint32_t * i, uint8_t battery_gflag);
/*leituras de configuracoes de software tais como limiares e tempos */
void leitura_config_ths(uint32_t * i);
/*leitura de versoes de hardware e de software */
void leitura_versoes(uint32_t * i);
/*Envio de codigo de erro de configuracao*/
void erroConfig(uint32_t * i);

uint8_t serialCode( uint8_t * vector);


uint8_t Serial_data[40];  //  creating a buffer of 40 bytes

/* Private variables ---------------------------------------------------------*/

// LORA
/* initialises the Lora Parameters*/
static LoRaParam_t LoRaParamInit = { LORAWAN_ADR_STATE,
	LORAWAN_DEFAULT_DATA_RATE,
	LORAWAN_PUBLIC_NETWORK,
	JOINREQ_NBTRIALS };
/* load Main call backs structure*/
static LoRaMainCallback_t LoRaMainCallbacks = { HW_GetBatteryLevel,
	HW_GetTemperatureLevel,
	HW_GetUniqueId,
	HW_GetRandomSeed,
	LORA_RxData,
	LORA_HasJoined,
	LORA_ConfirmClass};

// FSM
/*!
* Actual state of the state machine
*/
int fsm_state = IDLE;
/* transmission timer*/
static TimerEvent_t KeepAlive;
/* warning state frequency send timer*/
static TimerEvent_t WarnTxSendTimer;
/* Watchdog timer*/
static TimerEvent_t WDTimer;
/* warning state timeout timer*/
static TimerEvent_t WarnPeriodTimeoutTimer;
/* time between keep alive packet with flag timer*/
static TimerEvent_t Flag15sTimer;
/* time between end warn and keep alive timer*/
static TimerEvent_t EndWarn15sTimer;
/* time keep serial open*/
static TimerEvent_t Serial10sTimer;
/* Private function prototypes -----------------------------------------------*/ 

// HW
/* timer initialization*/
void HAL_TIM_MspPostInit (TIM_HandleTypeDef *htim);
/* call back when LoRa endNode has received a frame*/
/*Init e desinit usart */
static void MX_USART1_UART_Init(void);

static void MX_USART1_UART_DeInit(void);

/**
* @brief  Main program
* @param  None
* @retval None
*/
int main (void)
{

	uint32_t auxVolt;

	LPM_EnterLowPower();

	/* STM32 HAL library initialization*/
	HAL_Init ();

	/* Configure the system clock*/
	SystemClock_Config ();

	/* Configure the hardware */
	HW_Init ();
	
	/* Configure the UART */
	MX_USART1_UART_Init();

	// signal to start the init the firmware
	// 3x blink
	blink_led_xTimes(500,3,1);
	
	/* para ser usado no while enquanto for diferente de HAL_OK e nao tiver batido o timer da serial */
	HAL_StatusTypeDef saida = HAL_ERROR;
	
	// init serial timer
	SerialTimerStart(TX_ON_TIMER);
	
	// refresh de prevencao do WD
	HW_IDWDG_Refresh_Simple();
	// Esperar por interrupcao
	//HAL_UART_Receive_IT(&huart1,Rx_data,10);
	while(control_serial) {

		//		__NOP ();
		//		HAL_Delay(300);

		do {
		
			saida = HAL_UART_Receive (&huart1, Serial_data, 40, 1000);  // receive 40 bytes of data

		} while (saida != HAL_OK && control_serial == 1);

		if(saida == HAL_OK) {
			// refresh de prevencao do WD
			HW_IDWDG_Refresh_Simple();
			// decodifica o pacote recebido
			serialCode(Serial_data);
			// reiniciar timer de forma a conceder mais 10s (nessa versao)
			TimerReset(&Serial10sTimer);
		}
	}
	switch_off_led();
	// refresh de prevencao do WD
	HW_IDWDG_Refresh_Simple();
	
	/* DeInit the UART */
	MX_USART1_UART_DeInit();

	/* configure/int values and threshold */
	if(readFromEEPROM(FLAG_ADDRESS) != FLAG_VALUE ){

		/* init timers and store in eeprom */
		// KEEP ALIVE
		APP_TX_DUTYCYCLE = (uint32_t)KEEP_ALIVE_DEFAULT;
		writeToEEPROM (KEEP_ALIVE_TIME_ADD,KEEP_ALIVE_DEFAULT);

		// TEMPO TOTAL DE ALERTA (PERIODO)
		WARN_PERIOD = (uint32_t)WARN_PERIOD_DEFAULT;
		writeToEEPROM (WARN_TOTAL_TIME_PERIOD_ADD,WARN_PERIOD_DEFAULT);

		// TEMPO DE ENVIO DE ALERTA (FREQUENCIA)
		WARN_TX_SEND = (uint32_t)WARN_FREQ_TX_DEFAULT;
		writeToEEPROM (ALERT_TX_WARN_TIME_ADD,WARN_FREQ_TX_DEFAULT);

		// LIMIAR DA LUMINOSIDADE
		luminosity_limit = LUM_THS_DEFAULT;
		writeToEEPROM (THS_LUM_ADD, luminosity_limit);

		// LIMIAR DO MOVIMENTO
		mov_6D_limit = MOV_6D_THS_DEFAULT;
		mov_FF_ths = MOV_FF_THS_DEFAULT;
		mov_FF_duration = MOV_FF_DUR_DEFAULT;
		writeToEEPROM (THS_ACC_ADD, ((0x00 << 24) | (mov_6D_limit << 16) | (mov_FF_ths << 8) | (mov_FF_duration) ));

		// LIMIAR DA BATERIA
		battery_limit = (float)VOLT_THS_DEFAULT;
		auxVolt = (uint32_t)(battery_limit*1000);
		writeToEEPROM (THS_BAT_ADD,auxVolt);
		// 		testar escrita na eeprom

		/*dados de fabricacao*/
		uint32_t aux;
		// codigo do produto
		aux = readFromEEPROM(COID_PRD_ADD);
		if(aux == 0){
			writeToEEPROM (COID_PRD_ADD,CODIGO_PRODUTO);
		}
		aux = 0;
		// numero de serie
		aux = readFromEEPROM(NUM_SERIE_ADD);
		if(aux == 0){
			writeToEEPROM (NUM_SERIE_ADD,NUMERO_SERIE);
		}
		aux = 0;
		// data de fabricacao
		aux = readFromEEPROM(DATE_FAB_ADD);
		if(aux == 0){
			writeToEEPROM (DATE_FAB_ADD,DATA_FAB);
		}

		/*versao de firmware e de hardware */
		// versao do hardware de fabricacao
		writeToEEPROM (HW_VER_ADD,(uint32_t)(HW_VERSION));
		// versao do software de fabricacao
		writeToEEPROM (SW_VER_ADD,(uint32_t)(SW_VERSION));

		// WRITE FLAG
		writeToEEPROM (FLAG_ADDRESS,FLAG_VALUE);

		// quando estiver escrito usar dados passados.
	} else if (readFromEEPROM(FLAG_ADDRESS) == (uint32_t)FLAG_VALUE ) {

		uint32_t mov_aux;
		// se flag ocupada logo ............
		// recuperar valores
		// KEEP ALIVE
		APP_TX_DUTYCYCLE = (uint32_t)readFromEEPROM (KEEP_ALIVE_TIME_ADD);
		if(APP_TX_DUTYCYCLE == 0) {
			APP_TX_DUTYCYCLE = KEEP_ALIVE_DEFAULT;
		}

		// TEMPO total DE ENVIO DE ALERTA (PERIODO)
		WARN_PERIOD = (uint32_t)readFromEEPROM (WARN_TOTAL_TIME_PERIOD_ADD);
		if(WARN_PERIOD == 0) {
			WARN_PERIOD = WARN_PERIOD_DEFAULT;
		}

		// TEMPO de frequencia de envio DE ALERTA (FREQUENCIA)
		WARN_TX_SEND = (uint32_t)readFromEEPROM (ALERT_TX_WARN_TIME_ADD);
		if(WARN_TX_SEND == 0) {
			WARN_TX_SEND = WARN_FREQ_TX_DEFAULT;
		}

		// limiar de luminosidade
		luminosity_limit = readFromEEPROM (THS_LUM_ADD);
		if(luminosity_limit == 0) {
			luminosity_limit = LUM_THS_DEFAULT;
		}

		// limiar de movimento
		mov_aux = readFromEEPROM (THS_ACC_ADD);
		mov_FF_duration = 		(uint8_t)((mov_aux & 0x000000FF));
		mov_FF_ths = 		(uint8_t)((mov_aux & 0x0000FF00) >> 8);
		mov_6D_limit = 	(uint8_t)((mov_aux & 0x00FF0000) >> 16);
		if(mov_6D_limit == 0) {
			mov_6D_limit = MOV_6D_THS_DEFAULT;
		}
		if(mov_FF_ths == 0){
			mov_FF_ths = MOV_FF_THS_DEFAULT;
		}
		if (mov_FF_duration == 0){
			mov_FF_duration = MOV_FF_DUR_DEFAULT;
		}

		// limiar de bateria
		auxVolt = readFromEEPROM (THS_BAT_ADD);
		battery_limit = ((float) auxVolt) * 0.001;
		if(battery_limit == 0.0f) {
			battery_limit = VOLT_THS_DEFAULT;
		}
	}



	/* Disable Stand-by mode*/
	LPM_SetOffMode (LPM_APPLI_Id, LPM_Disable);

	/* Configure the Lora Stack*/
	LORA_Init (&LoRaMainCallbacks, &LoRaParamInit);

	/* Inicia transmissao*/
	LoraStartTx(TX_ON_TIMER, 0);

	/* Inicia maquina de estados e dos demais timers*/
	WarnStart(TX_ON_TIMER, 0);
	WDStart(TX_ON_TIMER);
	WarnPeriodStart(TX_ON_TIMER, 0);
	Flag15sStart(TX_ON_TIMER);
	EndWarnPeriodStart(TX_ON_TIMER);


#if TEST_MODE


#else
	/* Configuracao da interrupcao do sensor de luminosidade*/
	initIRQ_luminosidade();

	setIRQ_luminosidade(luminosity_limit); // threshold

	/* Acionamento do enable para o sensor de movimento*/
	HAL_GPIO_WritePin(EN_1V8_GPIO_Port, EN_1V8_Pin, GPIO_PIN_SET);
	/* Configuracao da interrupcao do sensor de movimento*/
	setIRQ_movimento();

	setThs_mov(mov_6D_limit, mov_FF_ths, mov_FF_duration); // threshold

#endif

	while (1)
	{
		DISABLE_IRQ();
		LPM_EnterLowPower();
		ENABLE_IRQ();
	}
}

/**
* @brief: Funcao para tratamento de donwlinks recebidos
* @param: AppData payload recebido no downlink
* @retval: None
*/
static void LORA_RxData (lora_AppData_t *AppData)
{
	uint32_t aux_timer;

	uint16_t decode = ((AppData->Buff[0] << 8) | (AppData->Buff[1]));
	uint32_t aux = (decode & 0x0FFF);
	uint8_t aux_mov;
	/* DOWNLINK MULTIPLO*/
	// Total bytes : 11 bytes
	// flag init 0xEA 2 bytes para cada item que se deseja alterar
	// [MSB_KeepAlive(0)] [LSB_KeepAlive(1)] [MSB_WarnDuty(2)] [LSB_WarnDuty(3)] [MSB_WarnTx(4)] [LSB_WarnTx(5)] [MSB_Battery_ths] [LSB_Battery_ths] [MSB_lumen_ths] [LSB_lumen_ths]
	if (AppData->Port == MULT_DL_PORT && AppData->BuffSize == 11) {
		uint16_t timers_aux, ths_aux;
		uint32_t warnTx_aux, warn_period_aux, keep_alive_aux;

		// pegando valores antes  de ter usado
		keep_alive_aux = (AppData->Buff[0] << 8) | (AppData->Buff[1]);
		if(keep_alive_aux != 0){
			keep_alive_aux *= 30000;
		} else {
			keep_alive_aux = APP_TX_DUTYCYCLE;
		}
		// warnTx eh o envio (frequencia).[MSB_WarnTx(4)] [LSB_WarnTx(5)]
		warnTx_aux = (AppData->Buff[4] << 8) | (AppData->Buff[5]);
		if(warnTx_aux != 0){
			warnTx_aux *= 5000;
		} else {
			warnTx_aux = WARN_TX_SEND;
		}
		// *periodo* do tempo de alerta [MSB_WarnDuty(2)] [LSB_WarnDuty(3)]
		warn_period_aux = (AppData->Buff[2] << 8) | (AppData->Buff[3]);
		if(warn_period_aux != 0){
			warn_period_aux *= 5000;
		}
		else {
			warn_period_aux = WARN_PERIOD;
		}

		// prevendo os valores dos timers a fim de usa-los corretamente
		// dado que nao funcionamento basico ou seja KA > W_DUTY > WARN_Tx
		if(!(keep_alive_aux > warn_period_aux && keep_alive_aux > warnTx_aux && warn_period_aux > warnTx_aux )) {
			// SE VIOLAR QUALQUER UMA DAS REGRAS VOLTA PARA O QUE ESTAVA ANTES.
			if (keep_alive_aux < warn_period_aux || keep_alive_aux < warnTx_aux ){
				keep_alive_aux = APP_TX_DUTYCYCLE;
			}
			if (warn_period_aux > keep_alive_aux || warn_period_aux < warnTx_aux ) {
				warn_period_aux = WARN_PERIOD;
			}
			if (warnTx_aux > keep_alive_aux || warnTx_aux > warn_period_aux ) {
				warnTx_aux = WARN_TX_SEND;
			}

		}

		timers_aux = ((AppData->Buff[0] << 8) | (AppData->Buff[1])); // se 0000 manter configuracao atual
		if(timers_aux != 0x0000 ){
			// Vai de 30s (0001) ate 546h7min (FFFF)
			// Tempo representado em milisegundos
			aux_timer = timers_aux * 30000; // Altera de 30 em 30 segundos
			if (aux_timer > warnTx_aux && aux_timer > warn_period_aux ) { // (NORMAL > ALERTA) & (NORMAL > Tx)
				APP_TX_DUTYCYCLE = aux_timer;
				// salvar na eeprom
				writeToEEPROM (KEEP_ALIVE_TIME_ADD,aux_timer);
				LoraStartTx(TX_ON_TIMER, 1);
			}else{
				if(aux_timer <= warn_period_aux){
					CONFIG_ERROR |= 0x01;	
				} 
				if (aux_timer <= warnTx_aux ) {
					CONFIG_ERROR |= 0x04;
				}
			}
		}
		// warn period
		timers_aux = ((AppData->Buff[2] << 8) | (AppData->Buff[3])); // se 0000 manter configuracao atual
		if(timers_aux != 0x0000 ){
			// Vai de 5s (0001) ate 91 horas (FFFF)
			// Tempo representado em milisegundos
			aux_timer = timers_aux * 5000; // Altera de 5 em 5 segundos
			if (aux_timer < APP_TX_DUTYCYCLE && aux_timer > warnTx_aux) { // (ALERTA < NORMAL & ALERTA > TX_ALERTA)
				WARN_PERIOD = aux_timer;
				// salvar na eeprom
				writeToEEPROM (WARN_TOTAL_TIME_PERIOD_ADD,WARN_PERIOD);
				WarnPeriodStart(TX_ON_TIMER, 1);
			}else{
				if(aux_timer >= APP_TX_DUTYCYCLE){
					CONFIG_ERROR |= 0x80;
				} 
				if (aux_timer <= warnTx_aux ) {
					CONFIG_ERROR |= 0x02;
				}
			}

		}
		// warn tx timeout
		timers_aux = ((AppData->Buff[4] << 8) | (AppData->Buff[5])); // se 0000 manter configuracao atual
		if(timers_aux != 0x0000 ){
			// Vai de 5s (0001) ate 916 horas (FFFF)
			// Tempo representado em milisegundos
			aux_timer = timers_aux * 5000; // Altera de 5 em 5 segundos
			if (aux_timer < WARN_PERIOD && aux_timer < APP_TX_DUTYCYCLE) { // (TX_ALERTA < ALERTA)
				WARN_TX_SEND = aux_timer;
				// salvar na eeprom
				writeToEEPROM (ALERT_TX_WARN_TIME_ADD,WARN_TX_SEND);
				WarnStart(TX_ON_TIMER, 1);
			}else{
				// erro! deve wtx < wto
				if(aux_timer >= APP_TX_DUTYCYCLE){
					CONFIG_ERROR |= 0x04;	
				} 
				if (aux_timer >= WARN_PERIOD ) {
					CONFIG_ERROR |= 0x02;
				}
			}
		}

		ths_aux = (uint16_t)(AppData->Buff[6]); // se 0000 manter configuracao atual
		if(ths_aux != 0x0000 ){
			// 0,02V (0001) ate 65,535V (FFFF)
			battery_limit = (float) (ths_aux * 0.02); // Altera de 0.02 em 0.02 Volts
			// salvar na eeprom// LIMIAR DA BATERIA
			writeToEEPROM (THS_BAT_ADD,(uint32_t)(ths_aux*20));
		}
		ths_aux = ((AppData->Buff[7] << 8) | (AppData->Buff[8])); // se 0000 manter configuracao atual
		if(ths_aux != 0x0000 ){
			// 0 lux (0001) ate  65,535 lux (FFFF)
			luminosity_limit = ths_aux;
			// salvar na eeprom LIMIAR DA luminosidade
			writeToEEPROM (THS_LUM_ADD,luminosity_limit);
			setIRQ_luminosidade(luminosity_limit);
		}

		//Configuracao do sensor de movimento
		// Verifica se a configuracao esta dentro dos limiares
		aux_mov = (AppData->Buff[9] >> 4); 	//mov_6D_limit
		if(aux_mov > 0X04){
			CONFIG_ERROR |= 0x10;
		}
		aux_mov = AppData->Buff[9] & 0x0F; 		//mov_FF_ths
		if(aux_mov > 0X08){
			CONFIG_ERROR |= 0x20;
		}
		aux_mov = AppData->Buff[10];  		//mov_FF_duration
		if(aux_mov > 0X40){
			CONFIG_ERROR |= 0x40;
		}

		if((CONFIG_ERROR & 0xF0) == 0x00){
			mov_6D_limit = (AppData->Buff[9] >> 4); //3 bits no primeiro nibble do primeiro byte
			mov_FF_ths = AppData->Buff[9] & 0x0F; 		 //segundo nibble do primeiro byte
			mov_FF_duration = AppData->Buff[10];  //segundo byte
			writeToEEPROM (THS_ACC_ADD, ((0x00 << 24) | (mov_6D_limit << 16) | (mov_FF_ths << 8) | (mov_FF_duration) ));
			setThs_mov(mov_6D_limit, mov_FF_ths, mov_FF_duration);
		}

	}


	// Sai do estado de alerta
	else if (decode == 0x0000 && AppData->BuffSize == 2){
		switch (fsm_state)
		{
		case WARN:
			fsm(rIDLE);
			break;

		case WARN2:
			fsm(rIDLE);
			break;

		default:
			fsm(rIDLE);
		}
	}


	/*	Configuracoes que podem ser pedidas via downlink	*/
	// transmite primeira configuracao
	else if (decode == 0x0011 && AppData->Port == SENS_PORT && AppData->BuffSize == 2) {
		Send(DOWN_MEDIDAS);
	}
	// transmite configuracao de hw
	else if (decode == 0x0033 && AppData->Port == HW_CONF_PORT && AppData->BuffSize == 2) {
		Send(DOWN_CONF_HW);
	}
	// transmite configuracao de sw
	else if (decode == 0x0077 && AppData->Port == SW_CONF_PORT && AppData->BuffSize == 2) {
		Send(DOWN_CONF_SW);
	}

	else if (decode == 0xFFFF) {
		// forcar reset do dispositivo
		TimerStop(&WDTimer);
	}

	// Muda o tempo de transmissao para um dia
	else if(decode == 0x00FE && AppData->BuffSize == 2){
		APP_TX_DUTYCYCLE = DAY;
		writeToEEPROM (KEEP_ALIVE_TIME_ADD,DAY);
		LoraStartTx(TX_ON_TIMER, 1);
	}
	/* 1 - Tempo de transmissao normal */
	else if (decode >= 0x1001 && decode <= 0x1FFF && AppData->BuffSize == 2)
	{
		// Vai de 30s (001) ate 34hs (FFF)
		// Tempo representado em milisegundos
		aux_timer = aux * 30000; // Altera de 30 em 30 segundos
		if (aux_timer > WARN_TX_SEND &&  aux_timer > WARN_PERIOD) { // (ALERTA < NORMAL) & KA > TX
			APP_TX_DUTYCYCLE = aux_timer;
			// salvar na eeprom
			writeToEEPROM (KEEP_ALIVE_TIME_ADD,aux_timer);
			LoraStartTx(TX_ON_TIMER, 1);
		}else{
			if(aux_timer <= WARN_TX_SEND){
				CONFIG_ERROR |= 0x02;	
			} 
			if (aux_timer <= WARN_PERIOD ) {
				CONFIG_ERROR |= 0x04;
			}
		}
	}
	/* 2 - Tempo em estado de alerta (ALERTA < NORMAL) [PERIODO] */
	else if (decode >= 0x2001 && decode <= 0x2FFF && AppData->BuffSize == 2)
	{
		// Vai de 5s (001) ate 5hs40min (FFF)
		// Tempo representado em milisegundos
		aux_timer = aux * 5000; // Altera de 5 em 5 segundos
		if (aux_timer < APP_TX_DUTYCYCLE && aux_timer > WARN_TX_SEND) { // (NORMAL > ALERTA)
			WARN_PERIOD = aux_timer;
			// salvar na eeprom
			writeToEEPROM (WARN_TOTAL_TIME_PERIOD_ADD,WARN_PERIOD);
			WarnPeriodStart(TX_ON_TIMER, 1);
			if(fsm_state == WARN){
				fsm(WARN);
			}
		}else{
			if(aux_timer >= APP_TX_DUTYCYCLE){
				CONFIG_ERROR |= 0x80;
			} 
			if( aux_timer <= WARN_TX_SEND) {
				CONFIG_ERROR |= 0x02;
			}

		}
	}
	/* 3 - Tempo de transmissao entre as msgs de alerta (TX_ALERTA < ALERTA & ALERTA > TX_ALERTA)  Frequency */
	else if (decode >= 0x3001 && decode <= 0x3FFF && AppData->BuffSize == 2)
	{
		// Vai de 5s (001) ate 5hs40min (FFF)
		aux_timer = aux * 5000; // Altera de 5 em 5 segundos
		if (aux_timer < WARN_PERIOD && aux_timer < APP_TX_DUTYCYCLE ) { // (TX_ALERTA < ALERTA) & TX < KA
			WARN_TX_SEND = aux_timer;
			// salvar na eeprom
			writeToEEPROM (ALERT_TX_WARN_TIME_ADD,WARN_TX_SEND);
			WarnStart(TX_ON_TIMER, 1);
		}else{
			if(aux_timer >= WARN_PERIOD){
				CONFIG_ERROR |= 0x02;	
			} 
			if (aux_timer >= APP_TX_DUTYCYCLE ) {
				CONFIG_ERROR |= 0x04;
			}
		}
	}
	/* 5 - Limiar de bateria baixa */
	else if (decode >= 0x5001 && decode <= 0x5FFF && AppData->BuffSize == 2)
	{
		// 0,001V (1) ate 4,095V (FFF)
		battery_limit = aux * 0.001; // Altera de 0.001 em 0.001 Volts
		// salvar na eeprom// LIMIAR DA BATERIA
		writeToEEPROM (THS_BAT_ADD,(uint32_t)aux);

	}
	/* 8 - limite de interrupcao da luminosidade */
	else if (decode >= 0x8000 && decode <= 0x8FFF && AppData->BuffSize == 2)
	{
		// 0 C (1) ate  4095 C (FFF)
		luminosity_limit = aux;
		// salvar na eeprom
		// LIMIAR DA luminosidade
		writeToEEPROM (THS_LUM_ADD,luminosity_limit);
		setIRQ_luminosidade(luminosity_limit);
	}
	/* 9 - limiteS de interrupcao da movimento */
	else if (AppData->BuffSize == 3 && ((AppData->Buff[0] << 16) | (AppData->Buff[1] << 8) | AppData->Buff[2]) >=  0x900000 && ((AppData->Buff[0] << 16) | (AppData->Buff[1] << 8) | AppData->Buff[2]) <=  0x9FFFFF)
	{
		//Configuracao do sensor de movimento
		// Verifica se a configuracao esta dentro dos limiares
		aux_mov = (AppData->Buff[1] >> 4); 	//mov_6D_limit
		if(aux_mov > 0X04){
			CONFIG_ERROR |= 0x10;
		}
		aux_mov = AppData->Buff[1] & 0x0F; 		//mov_FF_ths
		if(aux_mov > 0X08){
			CONFIG_ERROR |= 0x20;
		}
		aux_mov = AppData->Buff[2];  		//mov_FF_duration
		if(aux_mov > 0X40){
			CONFIG_ERROR |= 0x40;
		}

		if((CONFIG_ERROR & 0xF0) == 0x00){
			mov_6D_limit = (AppData->Buff[1] >> 4); //3 bits no primeiro nibble do primeiro byte
			mov_FF_ths = AppData->Buff[1] & 0x0F; 		 //segundo nibble do primeiro byte
			mov_FF_duration = AppData->Buff[2];  //segundo byte
			writeToEEPROM (THS_ACC_ADD, ((0x00 << 24) | (mov_6D_limit << 16) | (mov_FF_ths << 8) | (mov_FF_duration) ));
			setThs_mov(mov_6D_limit, mov_FF_ths, mov_FF_duration);
		}
	}
	else{
		CONFIG_ERROR |= 0x08;
	}
	if(CONFIG_ERROR != 0x00){
		Send(FAIL_CONFIG);
		CONFIG_ERROR = 0x00;
	}
}
/**
* @brief: Funcao que verifica se o endpoint esta conectado a LoRaWan
* @param: None
* @retval: None
*/
static void LORA_HasJoined( void )
{
#if( OVER_THE_AIR_ACTIVATION != 0 )
	PRINTF("JOINED\n\r");
#endif
	LORA_RequestClass( LORAWAN_DEFAULT_CLASS );
	blink_led(1000,0);

	if(lum_gflag == 1 || mov_gflag == 1){
		//	  Send(WARN);
		interruption_callback();
	}
	else if(fsm_state == IDLE){
		OnKeepAliveEvent();
		TimerReset(&KeepAlive);
	}
}

/**
* @brief: Funcao que altera a classe do endpoint
* @param: Class Nova classe do enpoint
* @retval: None
*/
static void LORA_ConfirmClass (DeviceClass_t Class)
{
	PRINTF("switch to class %c done\n\r", "ABC"[Class]);

	/*Optionnal*/
	/*informs the server that switch has occurred ASAP*/
	AppData.BuffSize = 0;
	AppData.Port = LORAWAN_APP_PORT;

	LORA_send (&AppData, LORAWAN_UNCONFIRMED_MSG);
}

/**
* @brief: Funcao que verifica se o endpoint esta conectado a LoRaWan
* @param: None
* @retval: None
*/
void Send (uint32_t down_op)
{
	uint32_t i = 0; // contador para o tamanho do payload a ser enviado

#if TEST_MODE
	AppData.Port = 10;

	AppData.Buff[i++] = 0xA1;	AppData.Buff[i++] = 0xFE;
	AppData.Buff[i++] = 0x5A;   AppData.Buff[i++] = 0x7F;
	AppData.Buff[i++] = 0x33;  	AppData.Buff[i++] = 0x1E;

#else

	uint8_t battery_gflag = 0, any_flag = 0;
	sensor_flag =false;
	uint8_t fsm_or_down = down_op;

	if(fsm_or_down == FORCE_KEEP_ALIVE ) {
		blink_led(1000,0);
	}

	if(down_op == 0) {
		fsm_or_down = fsm_state;
	}

	if (LORA_JoinStatus () != LORA_SET)
	{
		/*Not joined, try again later*/
		LORA_Join();
		return;
	}

	switch(fsm_or_down) {
	case WARN_END_KEEP_FLAG:
	case FORCE_KEEP_ALIVE:
	case rIDLE:
	case IDLE: {
			AppData.Port = KA_PORT;
			// 2 bytes bateria
			battery_gflag = leitura_bateria(&i, 0);
			// 2 bytes temperatura
			leitura_temp(&i,0);
			// 1 bytes flags
			leitura_flags(&i,battery_gflag);
			// para flags nao interruptivas maior que 0x00 e menor que 0x40 - logo verifica bateria e sensores.
			if(AppData.Buff[i - 1] > 0x00 && AppData.Buff[i - 1] < 0x40 ) {
				any_flag = 1;
			}

			break;
		}
	case KEEP_ALV_FLAG: // envio de flag caso em que ha flag nao interruptivo, vulgo bateria e sensores
	case WARN:
	case WARN2:{
			AppData.Port = WARN_PORT;
			// somente ler a flag de bateria, nao faz parte do payload!!!!!!
			battery_gflag = leitura_bateria(&i, 1);
			// 1 bytes flags
			leitura_flags(&i,battery_gflag);
			// 1 byte luminosidade
			leitura_luminosidade(&i,1);
			// 6 bytes acelerometro
			leitura_acc(&i);
			break;
		}
	case DOWN_MEDIDAS:{
			AppData.Port = SENS_PORT;

			// 2 bytes bateria
			battery_gflag = leitura_bateria(&i, 0);
			// 1 byte temperatura
			leitura_temp(&i,1);
			// 2 byte luminosidade
			leitura_luminosidade(&i,1);
			// 6 bytes acelerometro
			leitura_acc(&i);

			break;
		}
	case DOWN_CONF_HW:{
			AppData.Port = HW_CONF_PORT;
			// leitura de hardware 2 bytes cada, total 4 bytes
			leitura_versoes(&i);

			break;
		}
	case DOWN_CONF_SW:{
			AppData.Port = SW_CONF_PORT;
			// 9 bytes leitura de configuracoes hardware
			leitura_config_ths(&i);

			break;
		}
	case FAIL_CONFIG:{
			AppData.Port = FAIL_PORT;
			// 2 Bytes
			erroConfig(&i);
			break;
		}
	default: {
			AppData.Port = 8;
			battery_gflag = leitura_bateria(&i, 0);
			// 2 bytes temperatura
			leitura_temp(&i,0);
			// 2 bytes temperatura
			leitura_luminosidade(&i,0);
			// 6 bytes acelerometro
			leitura_acc(&i);
			// 1 bytes flags
			leitura_flags(&i,battery_gflag);

			break;
	}


	}

#endif
	AppData.BuffSize = i;

	LORA_send (&AppData, LORAWAN_DEFAULT_CONFIRM_MSG_STATE);

	bool send_warn = any_flag == 1 && AppData.Port == 1	&& (fsm_state == IDLE || fsm_state == rIDLE || fsm_or_down == WARN_END_KEEP_FLAG);

	if (send_warn ){
		if(fsm_state == rIDLE){
			fsm(IDLE);
		}
		if(send_warn) {
			// espera 15 segundos para enviar flag de send
			TimerReset(&Flag15sTimer);
			any_flag = 0;
		}
	} else {
		if(send_warn) {
			// espera 15 segundos para enviar flag de send
			TimerReset(&Flag15sTimer);
			any_flag = 0;
		}
		if(fsm_state == rIDLE){
			fsm(IDLE);
		}
	}

}

/**
* @brief: Funcao que implementa a maquina de estados
* @param: next_state proximo estado da maquina de estados
* @retval: None
*/
void fsm(int next_state){

	switch (next_state){
	case IDLE:
		// Desliga o keep alive next timer caso haja mudanca de qualquer estado [sempre se desliga - so ha um funcionamento]
		TimerStop(&Flag15sTimer);

		fsm_state = next_state;
		// Zera as flags de interrupcao
		lum_gflag = 0;
		mov_gflag = 0;
		// clean flags
		// Send(0);
		break;

	case WARN:
		// Start/Reset Timer Timeout [comecando o periodo de warn]
		TimerReset(&WarnPeriodTimeoutTimer);
		// Desliga o keep alive next timer caso haja mudanca de qualquer estado
		TimerStop(&Flag15sTimer);
		fsm_state = next_state;
		break;

	case WARN2:
		// Desliga o keep alive next timer caso haja mudanca de qualquer estado
		TimerStop(&Flag15sTimer);
		// Reseta timer se for interrupcao
		if(fsm_state == WARN2){
			TimerReset(&WarnPeriodTimeoutTimer);
		}
		fsm_state = next_state;
		break;

	case rIDLE:
		//evento c
		//end_warn = 1;
		TimerReset(&EndWarn15sTimer);
		// chamar timerReset
		TimerStop (&Flag15sTimer);
		// Parar Timers Warn e Timeout
		TimerStop(&WarnPeriodTimeoutTimer);
		TimerStop(&WarnTxSendTimer);
		// Zera as flags de interrupcao
		lum_gflag = 0;
		mov_gflag = 0;
		// Send scheduled msg
		fsm_state = next_state;
		// Send(0)
		break;

	}
}

/**
* @brief: Funcao que trata as interrupcoes de luminosidade e movimento
* @param: next_state proximo estado da maquina de estados
* @retval: None
*/
void interruption_callback (void) {
	// Evento a
	// Entra no estado de alarme
	if(fsm_state == IDLE && LORA_JoinStatus () == LORA_SET){
		// entra no estado de alerta e envia uma mensagem
		fsm(WARN);
		OnWarnTxTimerEvent();
	}
	else{
		if(fsm_state != rIDLE && fsm_state != IDLE){
			fsm(fsm_state);
		}
	}
}

/**
* @brief: Funcao que trata as interrupcoes de luminosidade e movimento
* @param: next_state proximo estado da maquina de estados
* @retval: None
*/
void keepAlive_callback (void) {
	// WARN_END_KEEP_FLAG == Force Keep Alive
	if (LORA_JoinStatus () == LORA_SET)
		Send(FORCE_KEEP_ALIVE);
}

/**
* @brief: Funcao que realiza a transmissao em si 
* @param: None
* @retval: None
*/
static void OnKeepAliveEvent (void)
{
	/*Wait for next timer slot*/
	TimerStart(&KeepAlive);

	// Evento d
	switch(fsm_state){
		// programa envio de keep alive sms
	case WARN:
		fsm(WARN2);
		break;

	case WARN2:
		fsm(WARN2);
		break;

	default:
		Send(0);
	}
}
/**
* @brief: Funcao que aguarda o timer para transmissao
* @param: None
* @retval: None
*/
static void LoraStartTx (TxEventType_t EventType, uint8_t change)
{
	if (EventType == TX_ON_TIMER)
	{
		/* send everytime timer elapses */
		if (change == 0){
			TimerInit (&KeepAlive, OnKeepAliveEvent);
		}
		TimerSetValue (&KeepAlive, APP_TX_DUTYCYCLE);
		// Se for mudanca por uplink reseta o timer sem enviar msg
		if (change == 1){
			TimerReset(&KeepAlive);
		}else{
			OnKeepAliveEvent ();
		}
	}
}

/**
* @brief: Funcao que realiza o refresh no Watchdog interno
* @param: None
* @retval: None
*/
void OnWDTimerEvent (void)
{
	/*Wait for next timer slot*/

	TimerStart(&WDTimer);
	HW_IDWDG_Refresh();

}
/**
* @brief: Funcao que inicia o timer do refresh do Watchdog interno
* @param: None
* @retval: None
*/
static void WDStart (TxEventType_t EventType)
{
	if (EventType == TX_ON_TIMER)
	{
		/* send everytime timer elapses */
		TimerInit (&WDTimer, OnWDTimerEvent);
		TimerSetValue (&WDTimer, WD_DUTYCYCLE);
		OnWDTimerEvent();
	}
}

/**
* @brief: Funcao que realiza o envio no estado de alerta (Tx)
* @param: None
* @retval: None
*/
void OnWarnTxTimerEvent (void)
{
	/*Wait for next timer slot*/
	TimerStart(&WarnTxSendTimer);
	Send (0);
}
/**
* @brief: Funcao que altera o timer de envio para o estado de alerta (Tx)
* @param: None
* @retval: None
*/
static void WarnStart (TxEventType_t EventType, uint8_t change)
{
	if (EventType == TX_ON_TIMER)
	{
		/* send everytime timer elapses */
		if(change == 0){
			TimerInit (&WarnTxSendTimer, OnWarnTxTimerEvent);
		}
		TimerSetValue (&WarnTxSendTimer, WARN_TX_SEND);

		if(change == 1 && (fsm_state == WARN || fsm_state == WARN2)){
			TimerStart(&WarnTxSendTimer);
		}
	}
}

/**
* @brief: Funcao que realiza encerra o estado de alerta (periodo total)
* @param: None
* @retval: None
*/
static void OnWarnEndPeriodEvent (void)
{
	// return to idle
	fsm(rIDLE);
}
/**
* @brief: Funcao que conta o tempo do estado de alerta [Periodo Total]
* @param: None
* @retval: None
*/
static void WarnPeriodStart (TxEventType_t EventType, uint8_t change)
{
	if (EventType == TX_ON_TIMER)
	{
		/* send everytime timer elapses */
		if(change == 0){
			TimerInit (&WarnPeriodTimeoutTimer, OnWarnEndPeriodEvent);
		}

		TimerSetValue (&WarnPeriodTimeoutTimer, WARN_PERIOD);

		if(change == 1 && (fsm_state == WARN || fsm_state == WARN2)){
			TimerStart(&WarnPeriodTimeoutTimer);
		}
	}
}

/**
* @brief: Funcao que realiza o envio de pacote tipo 2 "alerta" apos keep alive pkt com flag>0
* @param: None
* @retval: None
*/
void SendWarnSMSFlag (void)
{
	Send (KEEP_ALV_FLAG);
}
/**
* @brief: Funcao que conta o tempo para envio de pacote
* @param: None
* @retval: None
*/
void Flag15sStart (TxEventType_t EventType)
{
	if (EventType == TX_ON_TIMER)
	{
		/* send everytime timer elapses */
		TimerInit (&Flag15sTimer, SendWarnSMSFlag);
		TimerSetValue (&Flag15sTimer, SEND_PKTWARN_TIMER);
	}
}

/* keep alive packet after end warn period callback function*/
static void SendEndWarnSMS (void) {

	Send(WARN_END_KEEP_FLAG);

}
/* start the keep alive flag state timeout*/
static void EndWarnPeriodStart (TxEventType_t EventType) {

	if (EventType == TX_ON_TIMER)
	{
		/* send everytime timer elapses */
		TimerInit (&EndWarn15sTimer, SendEndWarnSMS);
		TimerSetValue (&EndWarn15sTimer, SEND_PKTWARN_TIMER);
	}

}


/* finish serial callback function*/
void FinishSerialTimer (void) {

	TimerStop(&Serial10sTimer);
	control_serial = 0;

}


/* start the warning state*/
void SerialTimerStart (TxEventType_t EventType) {

	if (EventType == TX_ON_TIMER)
	{
		/* send everytime timer elapses */
		TimerInit (&Serial10sTimer, FinishSerialTimer);
		TimerSetValue (&Serial10sTimer, SERIAL_TIMER);
		TimerStart( &Serial10sTimer);

	}
}

/*
* funcao que realiza a leitura do valor referente a bateria (2 bytes)
* e retorna devidamente no buffer - appdata.
* Ha opcao em que only_flag eh usado para quando so se quer saber o estado
* da bateria - para ativar ou nao a  flag
*/
uint8_t leitura_bateria(uint32_t * i, int only_flag ) {
	battery_V = HW_AdcReadChannel(ADC_Channel_VBAT); // sensor de bateria
	uint8_t battery_gflag;


	uint16_t vrefint_cal;                        // VREFINT calibration value
	uint16_t Vref_ADCtual = HW_AdcReadChannel(ADC_CHANNEL_VREFINT); // VRef_int
	vrefint_cal= *((uint16_t*)(VREFINT_CAL_ADDR)); // read VREFINT_CAL_ADDR memory location

	// Battery warning
	// Resistores - r =  220 K e 560 K
	// 560 / (220 + 560 ) = 56 / 78
	// v_ref = 3
	// v_ref = vbat * (56 / 78)
	float vref_mais =  ((float)vrefint_cal / (float)Vref_ADCtual);
	float bat_ADCalib = ((float) battery_V / (float) 4095);
	float aux_vbat =	3000 * vref_mais * bat_ADCalib;
	uint16_t batADC =  (aux_vbat * 1);
	float aux_v = (float)((float)(batADC) * 0.0013929);

	if ( aux_v <= battery_limit){

		battery_gflag = 1;

	}
	else {
		battery_gflag = 0;
	}
	//#else
	//	battery_gflag = 0;
	//#endif
	if(only_flag == 0) {
		AppData.Buff[(*i)++] = ((uint16_t)batADC >> 8) & 0xFF;
		AppData.Buff[(*i)++] = (uint16_t)batADC & 0xFF;
	}
	return battery_gflag;

}
/*
* funcao que realiza a leitura do valor referente a temperatura (2 bytes ou 1)
* e retorna devidamente no buffer - appdata.
* only1 - 1 indica para ler so um byte, 0 - ler os 2 bytes.
*/
void leitura_temp(uint32_t * i, uint8_t only1) {

	uint8_t data[2] = {0}; // vetor para montar o payload com os dados dos sensores

	ponteiro_pra_vetor(Temperatura_Mov(), data, 2); // sensor de temperatura

	// sensor register
	temp_C =  (uint16_t)(data[0] << 8);
	temp_C |= (uint16_t)(data[1]);

	if (only1){
		// get only msb  ou seja so a parte inteira
		// se maior que  0.5 (aumentar 1)
		uint8_t delta = 0;
		if(data[1] > 0x80){
			delta = 1;
		}
		AppData.Buff[(*i)++] = (data[0]+delta) & 0xFF; // msb
	} else {
		AppData.Buff[(*i)++] = data[0] & 0xFF; // msb
		AppData.Buff[(*i)++] = data[1] & 0xFF; // lsb
	}

}
/*
* funcao que realiza a leitura do valor referente a luminosidade (2 bytes ou 1)
* e retorna devidamente no buffer - appdata.
* only1 - 1 indica para ler so um byte, 0 - ler os 2 bytes.
*/
void leitura_luminosidade(uint32_t * i, uint8_t only1) {

	uint8_t data[2] = {0}; // vetor para montar o payload com os dados dos sensores

	ponteiro_pra_vetor(Luminosidade(), data, 2); // sensor de luminosidade

	if(only1) {
		if(data[0] /*msb*/ > 0x00){
			AppData.Buff[(*i)++] = 0xFF; // lsb gonna be full
		} else {
			AppData.Buff[(*i)++] = data[1] & 0xFF; // lsb
		}
	} else {
		AppData.Buff[(*i)++] = data[0] & 0xFF; // high
		AppData.Buff[(*i)++] = data[1] & 0xFF; // lsb
	}
}

/*
* funcao que realiza a leitura do valor referente do acelerometro X,Y e Z (2 bytes cada)
* e retorna devidamente no buffer - appdata.
*/
void leitura_acc(uint32_t * i) {

	uint8_t data6[6] = {}; // // vetor para montar o payload com os dados dos sensores com 6 posicoes

	ponteiro_pra_vetor(Movimento_Acc(), data6, 6); // sensor de aceleracao no eixo x
	for(int j = 0; j < 6; j++)
	{
		AppData.Buff[(*i)++] = data6[j] & 0xFF;
	}
	/* -- Leitura de giro --*/
	//	ponteiro_pra_vetor(Movimento_Giro(), data6, 6); // sensor de giroscopio no eixo x
	//	for(int j = 0; j < 6; j++)
	//	{
	//		AppData.Buff[i++] = data6[j] & 0xFF;
	//	}
}

/*
* funcao que realiza a leitura do valor referente das flags 1 byte
* e retorna devidamente no buffer - appdata.
*/
void leitura_flags(uint32_t * i, uint8_t battery_gflag) {

	/* trabalhando com Flags
	* bits
	// 1000 0000 80 - Lum
	// 0100 0000 40 - Mov
	---- alerta - nao interrputivos
	// 0010 0000 20 - Bat
	// 0001 0000 10 - Temp
	// 0000 1000 08 - erro nos sensores
	*/

	uint8_t flag_alarme = 0x00; // flag com o status da luminosidade e movimento


	// Se houve interrupcao de luminosidade
	if (lum_gflag){
		flag_alarme = 0x80;
	}
	//Se houve interrupcao de movimento
	if (mov_gflag){
		flag_alarme |= 0x40;
	}
	//Se houve alarme de bateria
	if (battery_gflag){
		flag_alarme |= 0x20;
	}

	//Se houve erro na leitura de algum sensor
	if (sensor_flag){
		flag_alarme |= 0x04;
	}

	AppData.Buff[(*i)++] = flag_alarme & 0xFF;


}


void leitura_config_ths(uint32_t * i) {

	uint8_t data_msb, data_lsb, data1,data2; // // vetor para montar o payload com os dados dos sensores com 6 posicoes
	uint16_t aux;
	aux = APP_TX_DUTYCYCLE / 30000; // representacao de 30 em 30 segundos
	data_msb = ((aux & 0x0FF0) >> 4); // pega os 2 nibles mais significativo
	// e joga fora o menos significativo
	// APP_TX = [msb][meio][lsb]
	// data_msb = [msb][meio]
	AppData.Buff[(*i)++] = data_msb;

	// pega o nibble menos significativo que sera o nible mais significativo do proximo
	data1 = (aux & 0x000F);
	// desloca um nible data 1 = [lsb_ap_tx] [0]
	data1 = data1 << 4;
	aux = WARN_PERIOD / 5000; // representacao de 5 em 5 segundos
	// WARN PERIOD = [msb] [meio] [lsb]
	// adquirir somente o byte mais significativo
	data2 = ((aux & 0xF00) >> 8);
	// data_lsb = [lsb_ap_tx][msb_warn]
	data_lsb = data1 | data2;
	AppData.Buff[(*i)++] = data_lsb;
	data1 = aux & 0xFF;
	// data1 = [meio_warn] [lsb_warn]
	AppData.Buff[(*i)++] = data1;

	aux = WARN_TX_SEND / 5000; // representacao de 5 em 5 segundos
	data_msb = (aux & 0x0FF0) >> 4; // pega os 2 nibles mais significativo
	// WARN_TX_SEND = [msb_warn_tx] [meio_warn_tx] [lsb_warn_tx]
	// data_msb = [msb_warn_tx] [meio_warn_tx]
	AppData.Buff[(*i)++] = data_msb;
	// pega o nibble menos significativo que sera o nible mais significativo do proximo
	data1 = (aux & 0x000F);
	// desloca um nible data 1 = [lsb_warn_tx] [0]
	data1 = data1 << 4;
	// BATERIA REPRESENTACAO EM 3 NIBBLES valores de 0,00 ate 40,95 volts
	uint32_t v32 = ((battery_limit * 100) / 1);
	aux = (uint16_t) (v32);
	aux = aux & 0x0FFF;
	// byte mais significativo no menos significativo do buffer iniciado
	data2 = ((aux & 0xF00) >> 8);
	// data_lsb = [lsb_warn_tx][msb_bateria]
	data_lsb  = data1 | data2;
	AppData.Buff[(*i)++] = data_lsb;

	data1 = aux & 0x00FF;
	// data1 = [meio_bateria] [lsb_bateria]
	AppData.Buff[(*i)++] = data1;

	// Movimento
	AppData.Buff[(*i)++] = ((mov_6D_limit - 1) << 6) | ((mov_FF_ths - 1) << 3) | ((mov_FF_duration - 1) >> 4);

	// Movimento + Luminosidade
	aux = ((((uint16_t)(mov_FF_duration - 1)) << 12) | (luminosity_limit & 0x0FFF));
	// luminosidade total de 2 bytes
	data_msb = (aux & 0xFF00) >> 8;
	AppData.Buff[(*i)++] = data_msb;

	data_lsb = aux & 0x00FF;
	AppData.Buff[(*i)++] = data_lsb;

}
/*
* funcao que realiza a leitura do valor referente a versao de HW e FW
* e retorna devidamente no buffer - appdata.
*/
void leitura_versoes(uint32_t * i) {

	uint32_t read_aux;
	uint8_t data1;

	// versao do hardware
	read_aux = readFromEEPROM(HW_VER_ADD);
	data1 = (read_aux & 0xFF00) >> 8;
	AppData.Buff[(*i)++] = data1;
	data1 = (read_aux & 0x00FF);
	AppData.Buff[(*i)++] = data1;

	// versao do sw
	read_aux = readFromEEPROM(SW_VER_ADD);
	data1 = (read_aux & 0xFF00) >> 8;
	AppData.Buff[(*i)++] = data1;
	data1 = (read_aux & 0x00FF);
	AppData.Buff[(*i)++] = data1;

}

/*
* funcao que realiza a leitura do valor referente a config_error e monta o buffer
* com o erro
* e retorna devidamente no buffer - appdata.
*/
void erroConfig(uint32_t * i){

	AppData.Buff[(*i)++] = CONFIG_ERROR;
	AppData.Buff[(*i)++] = 0x00;
}

static void MX_USART1_UART_Init(void)
{

	huart1.Instance = USART1;
	huart1.Init.BaudRate = 9600;
	huart1.Init.WordLength = UART_WORDLENGTH_8B;
	huart1.Init.StopBits = UART_STOPBITS_1;
	huart1.Init.Parity = UART_PARITY_NONE;
	huart1.Init.Mode = UART_MODE_TX_RX;
	huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart1.Init.OverSampling = UART_OVERSAMPLING_16;
	huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
	huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
	if (HAL_UART_Init(&huart1) != HAL_OK)
	{
		Error_Handler();
	}

}

static void MX_USART1_UART_DeInit(void) {

	HAL_UART_MspDeInit(&huart1);

}

void HAL_UART_RxCpltCallback (UART_HandleTypeDef *huart)
{
	if(huart->Instance == USART1) {
		HAL_UART_Receive_IT(&huart1,Serial_data,40);
	}
}

uint8_t serialCode( uint8_t*  vector) {
	/*
	- FLAG = MEIO BYTE
	- 1º - Codigo de produto – 5 digitos - 2 BYTES E MEIO
	- 2º - Numero de Serie - 6 digitos - 2 BYTES E MEIO
	- 3º - Data de fabricacao - ddmmaa
							* DD - 1 BYTE
							* MES - MEIO BYTE
							* ANO - 1 BYTE
	-----------------------------------
	total 8 bytes
	- 4º - dev-eui - 8 bytes
	- 5º - app-eui - 8 bytes
	- 6º - app-key - 16 bytes
	-------------------------
	total de tudo = 40 bytes
	*/

	uint8_t dev_eui_aux[8], app_eui_aux[8], app_key_aux[16];
	uint8_t aux;
	uint16_t crc_aux;
	uint8_t serial_ok[SERIAL_TOTAL_LEN_DFT];
	serial_ok[0] = SERIAL_INIT_FLAG;
	serial_ok[1] = SERIAL_DATA_LEN_DEFAULT;
	serial_ok[2] = SERIAL_OK;
	crc_aux = crc16(0xFFFF,serial_ok,3);
	serial_ok[3] = (uint8_t) ((crc_aux & 0xFF00) >> 8) ;
	serial_ok[4] = (uint8_t) (crc_aux & 0x00FF);
	int i = 0;

	if((vector[0] & 0xF0) == 0xE0) {
		// modo de escrita
		aux = vector[0] & 0x0F;
		CODIGO_PRODUTO = ((aux << 16) | (vector[1] << 8) | (vector[2]));
		aux = (vector[5] & 0xF0) >> 4;
		NUMERO_SERIE = ((vector[3] << 12) | (vector[4] << 4) | aux );
		aux = (vector[5] & 0x0F);
		DATA_FAB =  ((aux << 16) | (vector[6] << 8) | (vector[7]));
		for(i = 0; i < 16; i++) {
			if(i < 8){
				dev_eui_aux[i] = (uint8_t)vector[i+8];
				app_eui_aux[i] = (uint8_t)vector[i+16];
			}
			app_key_aux[i] = (uint8_t)vector[i+24];
		}
		// guardando dados na eeprom
		/*dados de fabricacao*/
		// codigo do produto
		writeToEEPROM (COID_PRD_ADD,CODIGO_PRODUTO);
		// numero de serie
		writeToEEPROM (NUM_SERIE_ADD,NUMERO_SERIE);
		// data de fabricacao
		writeToEEPROM (DATE_FAB_ADD,DATA_FAB);

		use_serialConfig(dev_eui_aux,app_eui_aux,app_key_aux);

		HAL_UART_Transmit(&huart1, (uint8_t *)serial_ok, 5, 50);

		return 1;

	} else if ((vector[0] & 0xF0) == 0x50) {
		// preparando para leitura de dados
		uint8_t flag_envio = 0xF,aux_lsb = 0, aux_msb = 0;
		uint8_t buffer_envio_all[44], buffer_serial_any[20],buffer_aux[8], buffer_aux_cred[32],app_eui_buff[8],dev_eui_buff[8],app_key_buff[16];
		uint32_t word_aux4 = 0,word_aux3 = 0 ,word_aux2 = 0 ,word_aux = 0;
		aux = 0;
		int not_all_info = 0;

		if(readFromEEPROM(FLAG_ADDRESS) == FLAG_VALUE ){
			not_all_info = 1;
		}

		// na primeira leitura cod produto
		word_aux = (uint32_t) readFromEEPROM (COID_PRD_ADD);
		if(word_aux == 0) {
			word_aux = CODIGO_PRODUTO;
		}
		aux_lsb = (word_aux & 0xF0000) >> 16;
		aux = (flag_envio << 4) | aux_lsb;
		buffer_aux[0] = aux;
		buffer_aux[1] = (uint8_t) ((word_aux & 0x0FF00) >> 8);
		buffer_aux[2] = (uint8_t) ((word_aux & 0x000FF));
		word_aux = readFromEEPROM (NUM_SERIE_ADD);
		if(word_aux == 0) {
			word_aux = NUMERO_SERIE;
		}
		buffer_aux[3] = (uint8_t) ((word_aux & 0xFF000) >> 12);
		buffer_aux[4] = (uint8_t) ((word_aux & 0x00FF0) >> 4);
		aux_msb = (uint8_t)(word_aux & 0x0000F);
		word_aux = readFromEEPROM (DATE_FAB_ADD);
		if(word_aux == 0) {
			word_aux = DATA_FAB;
		}
		aux_lsb = (uint8_t)((word_aux & 0xF0000) >> 16);
		aux = ((aux_msb << 4) | aux_lsb);
		buffer_aux[5] = aux;
		buffer_aux[6] = (uint8_t)((word_aux & 0x0FF00) >> 8);
		buffer_aux[7] = (uint8_t)((word_aux & 0x000FF));

		word_aux2 = (uint32_t) FLASH_read (ADD_MSB_DEV_EUI);
		word_aux =  (uint32_t) FLASH_read (ADD_LSB_DEV_EUI);
		if(word_aux == 0 && word_aux2 == 0 ){
			// verifica se use serial ja esta com os dados corretos se sim retorna 0 e preenche corretamente os buffer
			if(get_serialDev_EUI(dev_eui_buff) == 0) {
				uint8_t dev_eui_buff2[8] = LORAWAN_DEVICE_EUI;
				for(int i = 0; i < 8 ; i++ ) {
					dev_eui_buff[i] = dev_eui_buff2[i];
				}
			}
		} else {
			if(get_serialDev_EUI(dev_eui_buff) == 0){
				read_data(8,dev_eui_buff,word_aux2,word_aux,0,0);
			}
		}
		word_aux2 = (uint32_t) FLASH_read (ADD_MSB_APP_EUI);
		word_aux =  (uint32_t) FLASH_read (ADD_LSB_APP_EUI);
		if(word_aux == 0 && word_aux2 == 0 ){
			// verifica se use serial ja esta com os dados corretos se sim retorna 0 e preenche corretamente os buffer
			if(get_serialApp_EUI(app_eui_buff) == 0) {
				uint8_t app_eui_buff2[8] = LORAWAN_APPLICATION_EUI;
				for(int i = 0; i < 8 ; i++ ) {
					app_eui_buff[i] = app_eui_buff2[i];
				}
			}
		} else {
			if(get_serialApp_EUI(app_eui_buff) == 0) {
				read_data(8,app_eui_buff,word_aux2,word_aux,0,0);
			}
		}
		word_aux4 = (uint32_t) FLASH_read (ADD_APP_KEY_4);
		word_aux3 = (uint32_t) FLASH_read (ADD_APP_KEY_3);
		word_aux2 = (uint32_t) FLASH_read (ADD_APP_KEY_2);
		word_aux =  (uint32_t) FLASH_read (ADD_APP_KEY_1);
		if(word_aux == 0 && word_aux2 == 0 && word_aux3 == 0 && word_aux4 == 0 ){
			// verifica se use serial ja esta com os dados corretos se sim retorna 0 e preenche corretamente os buffer
			// se nao falhar buffer passado ira ter o valor.
			if(get_serialApp_Key(app_key_buff) == 0) {
				uint8_t app_key_buff2[16] = LORAWAN_APPLICATION_KEY;

				for(int i = 0; i < 16 ; i++ ) {
					app_key_buff[i] = app_key_buff2[i];
				}
			}
		} else {
			if(get_serialApp_Key(app_key_buff) == 0) {
				read_data(16,app_key_buff,word_aux4,word_aux3,word_aux2,word_aux);
			}
		}
		for(int i=0; i < 16; i++) {
			if(i < 8){
				buffer_aux_cred[i] = (uint8_t) dev_eui_buff[i];
				buffer_aux_cred[i+8] = (uint8_t) app_eui_buff[i];
			}

			buffer_aux_cred[i+16] = (uint8_t) app_key_buff[i] ;
		}

		if(not_all_info == 0) {
			int ci = 0, i = 0;
			buffer_envio_all[ci++] = SERIAL_INIT_FLAG;
			buffer_envio_all[ci++] = SERIAL_DATA_SEND_ALL;
			for(i=0; i < 8; i++){
				buffer_envio_all[ci + i] = buffer_aux[i];
			}
			ci += 8;
			for(i=0; i < 32; i++){
				buffer_envio_all[ci + i] = buffer_aux_cred[i];
			}
			ci += 32;
			// do jeito que esta la ........
			// dois ultimos crc
			crc_aux = crc16(0xFFFF,buffer_envio_all,42);
			buffer_envio_all[ci++] = (uint8_t) ((crc_aux & 0xFF00) >> 8) ;
			buffer_envio_all[ci++] = (uint8_t) (crc_aux & 0x00FF);


		} else {

			int ci = 0, i = 0;
			buffer_serial_any[ci++] = SERIAL_INIT_FLAG;
			buffer_serial_any[ci++] = SERIAL_DATA_SEND_ALMOST;
			for(i=0; i < 8; i++){
				buffer_serial_any[ci + i] = buffer_aux[i];
			}
			ci += 8;
			for(i=0; i < 8; i++){
				buffer_serial_any[ci + i] = buffer_aux_cred[i];
			}
			ci += 8;
			// do jeito que esta la ........
			// dois ultimos crc
			crc_aux = crc16(0xFFFF,buffer_serial_any,18);
			buffer_serial_any[ci++] = (uint8_t) ((crc_aux & 0xFF00) >> 8) ;
			buffer_serial_any[ci++] = (uint8_t) (crc_aux & 0x00FF);
		}

		if(not_all_info){
			HAL_UART_Transmit(&huart1, (uint8_t *)buffer_serial_any, 20, 500);
		} else {
			HAL_UART_Transmit(&huart1, (uint8_t *)buffer_envio_all, 44, 500);
		}
		return 1;

	} else {
		uint8_t serial_erro[SERIAL_TOTAL_LEN_DFT];
		serial_erro[0] = SERIAL_INIT_FLAG;
		serial_erro[1] = SERIAL_DATA_LEN_DEFAULT;
		serial_erro[2] = SERIAL_ERRO_LEITURA;
		crc_aux = crc16(0xFFFF,serial_erro,3);
		serial_erro[3] = (uint8_t) ((crc_aux & 0xFF00) >> 8) ;
		serial_erro[4] = (uint8_t) (crc_aux & 0x00FF);

		HAL_UART_Transmit(&huart1, (uint8_t *)serial_erro, 5, 100);

		return 0;

	}


}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
