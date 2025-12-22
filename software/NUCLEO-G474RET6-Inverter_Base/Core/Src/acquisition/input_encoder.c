/*
 * input_encoder.c
 *
 *  Created on: Nov 11, 2025
 *      Author: nicolas
 */
extern TIM_HandleTypeDef htim3;

static uint32_t prev_counter = 0;
static float speed_rpm = 0.0f;
volatile int32_t tour_count = 0;

void input_encoder_init(void)
{
    HAL_TIM_Encoder_Start(&htim3, TIM_CHANNEL_ALL);
}

void encoder_interrupt(void)
{
    tour_count++;
}

void encoder_process_speed(float delta_time_s)
{
    uint32_t curr_counter = __HAL_TIM_GET_COUNTER(&htim3);
    int32_t delta = (int16_t)(curr_counter - prev_counter);

    prev_counter = curr_counter;

    if(delta_time_s > 0.0001f) {
        speed_rpm = ((float)delta / delta_time_s) * (60.0f / (float)PPR);
    }
}

float encoder_get_speed_rpm(void)
{
    return speed_rpm;
}

uint32_t encoder_get_counter(void)
{
    return __HAL_TIM_GET_COUNTER(&htim3);
}

int32_t encoder_get_tours(void)
{
    return tour_count;
}
