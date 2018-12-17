#include "stervo\stervo.h"
#include "stm32f10x.h"
void stervo_init(void)
{
	TIM_TimeBaseInitTypeDef  tim;
	TIM_OCInitTypeDef        oc;
	GPIO_InitTypeDef         gpio;
    
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA, ENABLE);	
	RCC_APB1PeriphClockCmd( RCC_APB1Periph_TIM2, ENABLE);	

	gpio.GPIO_Pin = GPIO_Pin_1;
	gpio.GPIO_Mode = GPIO_Mode_AF_PP;
	gpio.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &gpio);

	tim.TIM_Period = 19999;
	tim.TIM_Prescaler = 71;
	tim.TIM_ClockDivision = TIM_CKD_DIV1;
	tim.TIM_CounterMode=TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM2, &tim);
	
	oc.TIM_OCMode = TIM_OCMode_PWM1;
	oc.TIM_OutputState = TIM_OutputState_Enable;
	oc.TIM_OutputNState = TIM_OutputNState_Disable;
	oc.TIM_Pulse = 0;
	oc.TIM_OCPolarity = TIM_OCPolarity_High;
	oc.TIM_OCNPolarity = TIM_OCPolarity_Low;
	oc.TIM_OCIdleState = TIM_OCIdleState_Reset;
	oc.TIM_OCNIdleState = TIM_OCIdleState_Reset;
//	TIM_OC1Init(TIM2, &oc);
	TIM_OC2Init(TIM2, &oc);
	
	TIM_ARRPreloadConfig(TIM2, ENABLE);
	TIM_CtrlPWMOutputs(TIM2,ENABLE);
	TIM_Cmd(TIM2, ENABLE);
	TIM2->CCR2 = 1500;
}

void set_stervo_angle(int angle)
{
	TIM2 -> CCR2 = 500 + angle * 11.11f;
}
