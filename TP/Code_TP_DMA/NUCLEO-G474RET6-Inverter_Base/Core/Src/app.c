/*
 * app.c
 *
 * Created on: Nov 11, 2025
 * Author: nicolas
 */

#include "app.h"
#include "main.h"
#include "motor_control/motor.h"
#include <stdlib.h> //atoi
#include <stdio.h>  //snprintf
#include "acquisition/input_analog.h"

extern TIM_HandleTypeDef htim1;

#include "user_interface/shell.h"
#include "user_interface/led.h"

static char shell_uart2_received_char;

// Prototypes nécessaires car les fonctions sont appelées dans init_device avant d'être définies
static int sh_set_ccr(h_shell_t* h_shell, int argc, char** argv);
static int sh_start(h_shell_t* h_shell, int argc, char** argv);
static int sh_stop(h_shell_t* h_shell, int argc, char** argv);
static int sh_get_current(h_shell_t* h_shell, int argc, char** argv);


void init_device(void){
	// Initialisation user interface
	// SHELL
	hshell1.drv.transmit = shell_uart2_transmit;
	hshell1.drv.receive = shell_uart2_receive;
	shell_init(&hshell1);
	HAL_UART_Receive_IT(&huart2, (uint8_t *)&shell_uart2_received_char, 1);

	//on ajoute la fonction SET_CCR
	shell_add(&hshell1, "SETCCR", sh_set_ccr, "Set PWM CCR value. Usage: SETCCR <val>");
	shell_add(&hshell1, "START", sh_start, "Start PWM a 50%");
	shell_add(&hshell1, "STOP", sh_stop, "Desactive PWM");
	shell_add(&hshell1, "IMES", sh_get_current, "Affiche le courant mesuré en Ampères");

	// LED
	led_init();

	// Initialisation motor control
	// MOTOR
	motor_init();
	// ASSERV (PID)
	//	asserv_init();


	// BUTTON
	//	button_init();
	//

	//
	// Initialisation data acquistion
	// ANALOG INPUT
	input_analog_init();
	// ENCODER INPUT
	//	input_encoder_init();
}

uint8_t shell_uart2_transmit(const char *pData, uint16_t size)
{
	HAL_UART_Transmit(&huart2, (uint8_t *)pData, size, HAL_MAX_DELAY);
	return size;
}

uint8_t shell_uart2_receive(char *pData, uint16_t size)
{
	*pData = shell_uart2_received_char;
	return 1;
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
	if (huart->Instance == USART2) {
		//		HAL_UART_Transmit(&huart2, (uint8_t *)&shell_uart2_received_char, 1, HAL_MAX_DELAY);
		HAL_UART_Receive_IT(&huart2, (uint8_t *)&shell_uart2_received_char, 1);
		shell_run(&hshell1);
	}
}

void loop(){

}
static int sh_set_ccr(h_shell_t* h_shell, int argc, char** argv)
{
	// 1. Vérification de l'argument
	if (argc < 2) {
		int size = snprintf(h_shell->print_buffer, SHELL_PRINT_BUFFER_SIZE, "Erreur: Argument manquant. Usage: SETCCR <0-100>\r\n");
		h_shell->drv.transmit(h_shell->print_buffer, size);
		return -1;
	}

	int user_percent = atoi(argv[1]);

	if (user_percent < 0 || user_percent > 100) {
		int size = snprintf(h_shell->print_buffer, SHELL_PRINT_BUFFER_SIZE, "Erreur: Valeur %d invalide. Entrez une valeur entre 0 et 100.\r\n", user_percent);
		h_shell->drv.transmit(h_shell->print_buffer, size);
		return -1; // Code erreur
	}

	uint16_t ccr_register_value = (uint16_t)( ((uint32_t)user_percent * MOTOR_CCR_MAX) / 100 );

	Motor_SetDutyCycle(ccr_register_value);

	int size = snprintf(h_shell->print_buffer, SHELL_PRINT_BUFFER_SIZE, "OK: Vitesse reglee a %d%% (Reg: %d/%d)\r\n", user_percent, ccr_register_value, MOTOR_CCR_MAX);
	h_shell->drv.transmit(h_shell->print_buffer, size);

	return 0;
}
static int sh_start(h_shell_t* h_shell, int argc, char** argv)
{
	// Calculer 50%
	uint16_t ccr_register_value = (uint16_t)( ((uint32_t)50 * MOTOR_CCR_MAX) / 100 );

	Motor_SetDutyCycle(ccr_register_value);

	// Activer les PWM
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
	HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_1);
	HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_2);

	int size = snprintf(h_shell->print_buffer, SHELL_PRINT_BUFFER_SIZE, "OK: START (50%%) (Reg: %d/%d)\r\n", ccr_register_value, MOTOR_CCR_MAX);
	h_shell->drv.transmit(h_shell->print_buffer, size);

	return 0;
}

static int sh_stop(h_shell_t* h_shell, int argc, char** argv)
{
	// Utilisez _Stop et non _Disable
	HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);
	HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_2);
	HAL_TIMEx_PWMN_Stop(&htim1, TIM_CHANNEL_1);
	HAL_TIMEx_PWMN_Stop(&htim1, TIM_CHANNEL_2);

	int size = snprintf(h_shell->print_buffer, SHELL_PRINT_BUFFER_SIZE, "OK: STOP PWM generation\r\n");
	h_shell->drv.transmit(h_shell->print_buffer, size);

	return 0;
}
static int sh_get_current(h_shell_t* h_shell, int argc, char** argv)
{

    int size = snprintf(h_shell->print_buffer, SHELL_PRINT_BUFFER_SIZE, "Courant = %.2f A\r\n", I_mes);
    h_shell->drv.transmit(h_shell->print_buffer, size);

    return 0;
}
