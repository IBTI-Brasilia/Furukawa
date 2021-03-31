/*
 * led.h
 *
 *  Created on: 16 de mar de 2020
 *      Author: IBTI-01
 */

#ifndef INC_LED_H_
#define INC_LED_H_


/* Includes ------------------------------------------------------------------*/
#include "hw.h"

/* Private define ------------------------------------------------------------*/

void blink_led (uint32_t period, uint8_t leave_on_final);
void LED_timeOut (void);
void switch_on_led();
void switch_off_led();
void blink_led_xTimes(uint32_t period, uint32_t xtimes, uint8_t leave_on_final);


#endif /* INC_LED_H_ */
