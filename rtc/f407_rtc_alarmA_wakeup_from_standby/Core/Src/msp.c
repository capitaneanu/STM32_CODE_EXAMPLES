#include "main.h"


// API for arm cortex processor (not STM32) peripherals defined in stm32f4xx_hal_cortex.c

void HAL_MspInit(void) {
	// configure processor interrupt group priority
	// preemption priority and subpriority bits distributed in 4bits
	// if two interrupts with same preemption priority are pending, then interrupt with lower
	// sub-priority wins.
	//Default is NVIC_PRIORITY_GROUP_4 : 4 bits for preemption priority, 0 bits for subpriority
	// this is set already in HAL_Init()
	__HAL_RCC_SYSCFG_CLK_ENABLE();
	__HAL_RCC_PWR_CLK_ENABLE();
	HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);
	// processor system exceptions
	// System Control Block (SCB) -> SHCSR has the enable bits
	// enable usage, memory and bus fault exceptions
	SCB->SHCSR |= SCB_SHCSR_BUSFAULTENA_Msk | SCB_SHCSR_USGFAULTENA_Msk | SCB_SHCSR_MEMFAULTENA_Msk;
	// default priority for these  exceptions is 0 (highest priority),
	// subpriority is ignored as group priority 4 is already set
	// SysTick irq and priority is already set up in HAL_Init()
	// as examples
	HAL_NVIC_SetPriority(BusFault_IRQn, 0, 0);
	HAL_NVIC_SetPriority(UsageFault_IRQn, 0, 0);
	HAL_NVIC_SetPriority(MemoryManagement_IRQn, 0, 0);
	// configure MPU, FPU, sleep mode etc
}





void HAL_UART_MspInit(UART_HandleTypeDef* huart){
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	if(huart->Instance==USART1)  {
		__HAL_RCC_USART1_CLK_ENABLE();
		// gate clock to uart1 only when cpu is not sleeping
		// to save on power consumption
		__HAL_RCC_USART1_CLK_SLEEP_DISABLE();
		__HAL_RCC_GPIOA_CLK_ENABLE();
		// PA9     ------> USART1_TX
		// PA10     ------> USART1_RX

		GPIO_InitStruct.Pin = GPIO_PIN_9|GPIO_PIN_10;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
		GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

		HAL_NVIC_SetPriority(USART1_IRQn, 0, 0);
		HAL_NVIC_EnableIRQ(USART1_IRQn);
		}
	}



void HAL_RTC_MspInit(RTC_HandleTypeDef* hrtc) {
	// apb clock enable macros don't apply
	// clock source is directly LSE, HSE or LSI
	// Turn on LSE
	RCC_OscInitTypeDef oscConfig;
	oscConfig.OscillatorType = RCC_OSCILLATORTYPE_LSE;
	oscConfig.LSEState = RCC_LSE_ON;
	oscConfig.PLL.PLLState = RCC_PLL_NONE;
	if (HAL_OK != HAL_RCC_OscConfig(&oscConfig)){
		Error_Handler();
		}
	// select LSE as RTC clock
	RCC_PeriphCLKInitTypeDef clkInit;
	// only two fields are relevant for our application
	clkInit.PeriphClockSelection = RCC_PERIPHCLK_RTC;
	clkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
	if (HAL_OK != HAL_RCCEx_PeriphCLKConfig(&clkInit)) {
		Error_Handler();
		}
	// Enable the RTC clock
	__HAL_RCC_RTC_ENABLE();
	// calendar starts ticking now

	// Enable the RTC Alarm IRQ in NVIC
	HAL_NVIC_SetPriority(RTC_Alarm_IRQn, 15, 0);
	HAL_NVIC_EnableIRQ(RTC_Alarm_IRQn);
	}


