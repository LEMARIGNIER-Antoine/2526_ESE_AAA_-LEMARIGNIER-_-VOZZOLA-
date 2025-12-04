/*
 * motor.h
 *
 *  Created on: Nov 11, 2025
 *      Author: nicolas
 */

#ifndef INC_MOTOR_CONTROL_MOTOR_H_
#define INC_MOTOR_CONTROL_MOTOR_H_

#include "main.h"

#include "user_interface/shell.h"


void motor_init();

void SET_PWM(h_shell_t* h_shell, int argc, char** argv);


#endif /* INC_MOTOR_CONTROL_MOTOR_H_ */
