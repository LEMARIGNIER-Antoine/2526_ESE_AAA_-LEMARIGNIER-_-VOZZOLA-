/*
 * analog_input.c
 *
 *  Created on: Nov 11, 2025
 *      Author: nicolas
 */

#include "acquisition/input_analog.h"
#include "main.h"

extern ADC_HandleTypeDef hadc1;
extern TIM_HandleTypeDef htim4;

#define ADC_RESOLUTION  4096.0f  // 12 bits
#define V_SUPPLY        3.3f
#define SENSOR_SENS     0.05f    // Sensibilité 50mV/A = 0.05 V/A

uint16_t adc_dma_buffer[10];

volatile float I_mes = 0.0f;

uint8_t flag_init = 0;

float SENSOR_OFFSET = 0;

void input_analog_init(void){
	HAL_ADCEx_Calibration_Start(&hadc1, ADC_SINGLE_ENDED);
	HAL_ADC_Start_DMA(&hadc1, (uint16_t*)&adc_dma_buffer, 10);
	HAL_TIM_Base_Start(&htim4);

}

//en ADC
//float ADC_ReadCurrent(void)
//{
//    uint32_t raw_adc_value = 0;
//    float Uout = 0.0f;
//
//    HAL_ADC_Start(&hadc1);
//
//    if (HAL_ADC_PollForConversion(&hadc1, 10) == HAL_OK)
//    {
//        raw_adc_value = HAL_ADC_GetValue(&hadc1);
//
//        // Formule :Uout= (Valeur_Brute * 3.3) / 4096
//        Uout = ((float)raw_adc_value * V_SUPPLY) / ADC_RESOLUTION;
//
//        // Formule :Imes= (Uout - 1.65V) / 0.05 V/A
//        I_mes = (Uout - SENSOR_OFFSET) / SENSOR_SENS;
//    }
//    HAL_ADC_Stop(&hadc1);
//    return I_mes;
//}

//DMA
void ADC_Process_DMA_Conversion(void)
{
	float Uout = 0.0f;
	int i=0;
	float sum=0;

	for (int i=0;i<10;i++)
	{
		Uout = ((float)adc_dma_buffer[i] * V_SUPPLY) / ADC_RESOLUTION;
		sum=sum+Uout;
	}
	sum=sum/10.0;

	if (flag_init == 0)
	{
		SENSOR_OFFSET = sum ;
		flag_init = 1;

	}

	I_mes = (sum - SENSOR_OFFSET) / SENSOR_SENS;


}
