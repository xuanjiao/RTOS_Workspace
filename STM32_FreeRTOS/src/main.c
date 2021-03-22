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

// Macros
#define AVAILABLE 		1
#define NOT_AVAILABLE 0

// Task handles
TaskHandle_t xTaskHandle1 = NULL;
TaskHandle_t xTaskHandle2 = NULL;

// Task handle functions
void vTask1_handler(void *params);
void vTask2_handler(void *params);

// Global values
uint8_t UART_ACCESS_KEY = AVAILABLE;

void hardware_setup(void);
void uart_setup(void);

void send_uart_msg(char* msg);

#ifdef USE_SEMIHOSTING
	extern void initialise_monitor_handles();
#endif



int main(void)
{
#ifdef USE_SEMIHOSTING
	initialise_monitor_handles();
	printf("Main function start\n");
#endif



	/*
	 * Resets the RCC clock configuration to the default reset state.
	 * HSE OFF, PLL OFF, HSI ON and used as system clock source (16 MHz)
	 * AHB, APB1 and APB2 prescaler set to 1.
	 */
	RCC_DeInit();

	// Enable cycle counter
	DWT->CTRL |= (1 <<0 );

	// Start recording events
	SEGGER_SYSVIEW_Conf();
	SEGGER_SYSVIEW_Start();


	hardware_setup();

	// Update SystemCoreClock variable
	SystemCoreClockUpdate();

	xTaskCreate(vTask1_handler,"Task-1",configMINIMAL_STACK_SIZE,NULL,configMAX_PRIORITIES,&xTaskHandle1);
	xTaskCreate(vTask2_handler,"Task-2",configMINIMAL_STACK_SIZE,NULL,configMAX_PRIORITIES,&xTaskHandle1);

	vTaskStartScheduler();

}

void hardware_setup(void)
{
	uart_setup();
}

void uart_setup(void)
{
	// Enable clock
	// GPIOA is hanging on AHB1 bus. UART is hanging on APB1 bus
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);

	// Initialize GPIO pins PA2 and PA3 to UART mode
	GPIO_InitTypeDef gpio_init;
	USART_InitTypeDef uart_pins;
	memset(&gpio_init,0,sizeof(gpio_init));
	memset(&uart_pins,0,sizeof(uart_pins));

	gpio_init.GPIO_Mode = GPIO_Mode_AF;
	gpio_init.GPIO_OType = GPIO_OType_PP;
	gpio_init.GPIO_PuPd = GPIO_PuPd_UP;
	gpio_init.GPIO_Speed = GPIO_Low_Speed;

	gpio_init.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
	GPIO_Init(GPIOA,&gpio_init);

	GPIO_PinAFConfig(GPIOA,GPIO_PinSource2,GPIO_AF_USART2);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource3,GPIO_AF_USART2);

	// Initialize UART2

	uart_pins.USART_BaudRate = 115200;
	uart_pins.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	uart_pins.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	uart_pins.USART_Parity = USART_Parity_No;
	uart_pins.USART_StopBits = USART_StopBits_1;
	uart_pins.USART_WordLength = USART_WordLength_8b;

	USART_Init(USART2,&uart_pins);

	// Enable UART2
	USART_Cmd(USART2,ENABLE);
}

/*
 * Send a message over UART
 */
void send_uart_msg(char* msg){

	for (uint32_t i = 0; i < strlen(msg);i++){
		while(USART_GetFlagStatus(USART2,USART_FLAG_TXE) == RESET); // Wait until transmit data register is empty
		USART_SendData(USART2,msg[i]);	// Send a byte
	}

}

void vTask1_handler(void *params)
{
	while(1)
	{
		if(UART_ACCESS_KEY == AVAILABLE){
			UART_ACCESS_KEY = NOT_AVAILABLE;
			send_uart_msg("task-1\n");
			UART_ACCESS_KEY = AVAILABLE;
			taskYIELD();

		}
	}
}

void vTask2_handler(void *params)
{
	while(1)
	{
		if(UART_ACCESS_KEY == AVAILABLE){
			UART_ACCESS_KEY = NOT_AVAILABLE;
			send_uart_msg("task-2\n");
			UART_ACCESS_KEY = AVAILABLE;
			taskYIELD();
		}

	}
}
