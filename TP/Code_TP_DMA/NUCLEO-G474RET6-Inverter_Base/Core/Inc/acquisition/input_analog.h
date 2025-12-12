/*
 * input_analog.h
 *
 *  Created on: Nov 11, 2025
 *      Author: nicolas
 */

#ifndef INC_INPUT_ANALOG_H_
#define INC_INPUT_ANALOG_H_

#include "main.h"

extern volatile float I_mes;

void input_analog_init(void);
void ADC_Process_DMA_Conversion(void);

#endif /* INC_INPUT_ANALOG_H_ */
