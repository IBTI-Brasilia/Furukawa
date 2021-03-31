/*
 * led.c
 *
 *  Created on: 16 de mar de 2020
 *      Author: IBTI-01
 */

#include "led.h"
#include "timeServer.h"

/* LED timer*/
static TimerEvent_t LEDTimer;
uint8_t led_status = 0;
uint8_t blink_counter = 0;
uint8_t times_to_blink = 0;
uint8_t leave_on = 0;

/**
* @brief: Funcao que liga o LED e inicia o timer para desliga-lo
* @param: None
* @retval: None
*/
void blink_led (uint32_t period, uint8_t leave_on_final)
{
	/* send everytime timer elapses */
	leave_on = leave_on_final;
	switch_on_led();
	TimerInit (&LEDTimer, LED_timeOut);
	TimerSetValue (&LEDTimer, period);
	TimerStart(&LEDTimer);
}

void LED_timeOut (void)
{
	// Apaga o led e fica o mesmo periodo de tempo apagado
	if(led_status == 1){
		blink_counter++;
		switch_off_led();
		TimerReset(&LEDTimer);
	// Se ja passou o periodo apagado, verifica se deve piscar mais vezes ou nao
	}else{
		if(blink_counter >= times_to_blink){
			blink_counter = 0;
			times_to_blink = 0;
			TimerStop(&LEDTimer);
			// Verifica se o led deve ficar ligado apos piscar
			if (leave_on == 1){
				switch_on_led();
			}

		}else{
			switch_on_led();
			TimerReset(&LEDTimer);
		}
	}
}

void switch_on_led() {
	// liga led verde
	led_status = 1;
	HAL_GPIO_WritePin (GPIOA,KEEP_LED_Pin,GPIO_PIN_SET);
}
void switch_off_led() {
	// desliga led verde
	led_status = 0;
	HAL_GPIO_WritePin (GPIOA,KEEP_LED_Pin,GPIO_PIN_RESET);
}

void blink_led_xTimes(uint32_t period, uint32_t xtimes, uint8_t leave_on_final) {
	times_to_blink = xtimes;
	blink_counter = 0;
	blink_led(period,leave_on_final);
}
