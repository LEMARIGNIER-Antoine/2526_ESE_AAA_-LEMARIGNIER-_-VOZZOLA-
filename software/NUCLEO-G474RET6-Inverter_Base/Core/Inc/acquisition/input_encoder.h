/*
 * input_encoder.h
 *
 *  Created on: Nov 11, 2025
 *      Author: nicolas
 */

#ifndef INC_ACQUISITION_INPUT_ENCODER_H_
#define INC_ACQUISITION_INPUT_ENCODER_H_

#include <stdint.h>

#define PPR (1024 * 4)

void input_encoder_init(void);
void encoder_interrupt(void);

void encoder_process_speed(float delta_time_s);
float encoder_get_speed_rpm(void);
uint32_t encoder_get_counter(void);
int32_t encoder_get_tours(void);

#endif /* INC_ACQUISITION_INPUT_ENCODER_H_ */
