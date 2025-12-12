/*
 * motor.h
 *
 *  Created on: Nov 11, 2025
 *      Author: nicolas
 */

#ifndef INC_MOTOR_CONTROL_MOTOR_H_
#define INC_MOTOR_CONTROL_MOTOR_H_

#include "main.h"

#define MOTOR_CCR_MAX 1023

void motor_init(void);
void Motor_SetDutyCycle(uint16_t ccr_value);

#endif /* INC_MOTOR_CONTROL_MOTOR_H_ */
