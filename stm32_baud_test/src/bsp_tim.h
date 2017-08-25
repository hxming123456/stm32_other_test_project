#ifndef __BSP_TIM_H
#define __BSP_TIM_H

#include "stm32f10x.h"

void tim_init(void);
void delay_ms(int32_t timecnt);
void delay_s(int32_t scnt);
void change_light_status(uint8_t status);

#endif
