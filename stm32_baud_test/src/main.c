//
// This file is part of the GNU ARM Eclipse distribution.
// Copyright (c) 2014 Liviu Ionescu.
//

// ----------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include "diag/Trace.h"

// ----------------------------------------------------------------------------
//
// Standalone STM32F1 empty sample (trace via DEBUG).
//
// Trace support is enabled by adding the TRACE macro definition.
// By default the trace messages are forwarded to the DEBUG output,
// but can be rerouted to any device or completely suppressed, by
// changing the definitions required in system/src/diag/trace_impl.c
// (currently OS_USE_TRACE_ITM, OS_USE_TRACE_SEMIHOSTING_DEBUG/_STDOUT).
//

// ----- main() ---------------------------------------------------------------

// Sample pragmas to cope with warnings. Please note the related line at
// the end of this function, used to pop the compiler diagnostics status.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wmissing-declarations"
#pragma GCC diagnostic ignored "-Wreturn-type"
#include "stm32f10x.h"
#include "bsp_usart.h"
#include "bsp_tim.h"

ErrorStatus HSEStartUpStatus;


void system_clk_init(void)
{
	RCC_DeInit();

	RCC_HSEConfig(RCC_HSE_ON);

	HSEStartUpStatus = RCC_WaitForHSEStartUp();

	if(HSEStartUpStatus)
	{
		RCC_HCLKConfig(RCC_SYSCLK_Div1);
		RCC_PCLK2Config(RCC_HCLK_Div1);
		RCC_PCLK1Config(RCC_HCLK_Div2);
	}

	RCC_PLLConfig(RCC_PLLSource_HSE_Div1,RCC_PLLMul_9);
	RCC_PLLCmd(ENABLE);

    while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
    {
    }

	RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

    while(RCC_GetSYSCLKSource() != 0x08)
    {
    }
}

void is_download_pin_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

uint8_t key_backup = 1;
uint8_t key_status = 1;

void light_pin_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;//ok nok now
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_ResetBits(GPIOB,GPIO_Pin_13);
	GPIO_SetBits(GPIOB,GPIO_Pin_14);
	GPIO_SetBits(GPIOB,GPIO_Pin_15);
}

int main(int argc, char* argv[])
{
	uint8_t c = 0x55;

	//system_clk_init();
	is_download_pin_init();
	usart1_init();
	tim_init();
	Debug_usart_init();
	light_pin_init();

	while (1)
    {
		if(key_status==0)
		{
			if(key_status != key_backup)
			{
				usart1_write(&c,1);
			}
			key_backup = key_status;
		}
    }
}


void TIM2_IRQHandler(void)
{
	static uint32_t key_time = 0;
	uint8_t c= 0x55;

	if ( TIM_GetITStatus(TIM2 , TIM_IT_Update) != RESET )
	{
#if 0
		if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_12)==0)
		{
			key_time++;
			if(key_time>=25)
			{
				key_time = 0;
				key_status=0;
			}
		}
		else
		{
			key_status = 1;
		}
#endif
		key_time++;
		if(key_time >= 1000)
		{
			key_time = 0;
			Debug_usart_write(&c,1,'Y');
		}
		TIM_ClearITPendingBit(TIM2 , TIM_FLAG_Update);
	}
}

#pragma GCC diagnostic pop



// ----------------------------------------------------------------------------
