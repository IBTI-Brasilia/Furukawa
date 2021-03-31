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
#ifndef INC_HW_IWDG_H_
#define INC_HW_IWDG_H_

/* Includes ------------------------------------------------------------------*/
#include "hw.h"



void HW_IWDG_Init(void);
void HW_IDWDG_Refresh(void);
void HW_IDWDG_Refresh_Simple(void);

#endif /* INC_HW_IWDG_H_ */
