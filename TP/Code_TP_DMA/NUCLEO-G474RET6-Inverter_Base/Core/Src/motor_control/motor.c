/*
 * motor.c
 *
 *  Created on: Nov 11, 2025
 *      Author: nicolas
 */

#include "motor_control/motor.h"
#include "main.h"

extern TIM_HandleTypeDef htim1;

void motor_init(void)
{

    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 614);
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, 1023 - 614);

    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);

    HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_1);//complémentaire
    HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_2);

    __HAL_TIM_MOE_ENABLE(&htim1);
}
void Motor_SetDutyCycle(uint16_t ccr_value)
{
	//clamping
    if (ccr_value > MOTOR_CCR_MAX) {
        ccr_value = MOTOR_CCR_MAX;
    }

    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, ccr_value);
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, MOTOR_CCR_MAX- ccr_value);
}
