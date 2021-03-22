/**
  ******************************************************************************
  * @file    main.c
  * @author  Ac6
  * @version V1.0
  * @date    01-December-2013
  * @brief   Default main function.
  ******************************************************************************
*/

#include<stdio.h>
#include<stdint.h>
#include<string.h>
#include "stm32f4xx.h"
#include "FreeRTOS.h"

#include "task.h"

TaskHandle_t xTaskHandle1 = NULL;
TaskHandle_t xTaskHandle2 = NULL;

void vTask1_handler(void *params);
void vTask2_handler(void *params);

extern void initialise_monitor_handles();

int main(void)
{
	initialise_monitor_handles();
	printf("Main function start\n");

	// Enable cycle counter
	DWT->CYCCNT |= (1 <<0 );

	// Start recording events
	SEGGER_SYSVIEW_Conf();
	SEGGER_SYSVIEW_Start();


	/*
	 * Resets the RCC clock configuration to the default reset state.
	 * HSE OFF, PLL OFF, HSI ON and used as system clock source (16 MHz)
	 * AHB, APB1 and APB2 prescaler set to 1.
	 */
	RCC_DeInit();

	// Update SystemCoreClock variable
	SystemCoreClockUpdate();

	xTaskCreate(vTask1_handler,"Task-1",configMINIMAL_STACK_SIZE,NULL,configMAX_PRIORITIES,&xTaskHandle1);
	xTaskCreate(vTask2_handler,"Task-2",configMINIMAL_STACK_SIZE,NULL,configMAX_PRIORITIES,&xTaskHandle1);

	vTaskStartScheduler();

}

void vTask1_handler(void *params)
{
	while(1)
	{
		printf("task-1\n");
		taskYIELD();
	}
}

void vTask2_handler(void *params)
{
	while(1)
	{
		printf("task-2\n");
		taskYIELD();
	}
}
