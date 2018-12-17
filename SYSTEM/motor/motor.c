#include "motor\motor.h"

// right  B4 B5 
// left B6 B7

void motor_init() {
	TIM_TimeBaseInitTypeDef  tim_3, tim_4;
	TIM_OCInitTypeDef        oc_a, oc_b;
	GPIO_InitTypeDef         gpio_a, gpio_b;
	
	GPIO_InitTypeDef gpio_l, gpio_r;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	
	gpio_l.GPIO_Mode = GPIO_Mode_Out_PP;
	gpio_l.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_5;
	gpio_l.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &gpio_l);

	gpio_r.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
	gpio_r.GPIO_Mode = GPIO_Mode_Out_PP;
	gpio_r.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &gpio_r);
    
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB1PeriphClockCmd( RCC_APB1Periph_TIM3, ENABLE);	
	RCC_APB1PeriphClockCmd( RCC_APB1Periph_TIM4, ENABLE);	

	//A6 pwm_a
	gpio_a.GPIO_Pin = GPIO_Pin_6;
	gpio_a.GPIO_Mode = GPIO_Mode_AF_PP;
	gpio_a.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &gpio_a);

	//B8 pwm_b
	gpio_b.GPIO_Pin = GPIO_Pin_8;
	gpio_b.GPIO_Mode = GPIO_Mode_AF_PP;
	gpio_b.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &gpio_b);

	tim_3.TIM_Period = 9999;
	tim_3.TIM_Prescaler = 71;
	tim_3.TIM_ClockDivision = TIM_CKD_DIV1;
	tim_3.TIM_CounterMode=TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM3, &tim_3);
	
	tim_4.TIM_Period = 9999;
	tim_4.TIM_Prescaler = 71;
	tim_4.TIM_ClockDivision = TIM_CKD_DIV1;
	tim_4.TIM_CounterMode=TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM4, &tim_4);
	
	oc_a.TIM_OCMode = TIM_OCMode_PWM1;
	oc_a.TIM_OutputState = TIM_OutputState_Enable;
	oc_a.TIM_OutputNState = TIM_OutputNState_Disable;
	oc_a.TIM_Pulse = 0;
	oc_a.TIM_OCPolarity = TIM_OCPolarity_High;
	oc_a.TIM_OCNPolarity = TIM_OCPolarity_Low;
	oc_a.TIM_OCIdleState = TIM_OCIdleState_Reset;
	oc_a.TIM_OCNIdleState = TIM_OCIdleState_Reset;
	TIM_OC1Init(TIM3, &oc_a);
	
	oc_b.TIM_OCMode = TIM_OCMode_PWM1;
	oc_b.TIM_OutputState = TIM_OutputState_Enable;
	oc_b.TIM_OutputNState = TIM_OutputNState_Disable;
	oc_b.TIM_Pulse = 0;
	oc_b.TIM_OCPolarity = TIM_OCPolarity_High;
	oc_b.TIM_OCNPolarity = TIM_OCPolarity_Low;
	oc_b.TIM_OCIdleState = TIM_OCIdleState_Reset;
	oc_b.TIM_OCNIdleState = TIM_OCIdleState_Reset;
	TIM_OC3Init(TIM4, &oc_b);
	
	TIM_ARRPreloadConfig(TIM3, ENABLE);
	TIM_CtrlPWMOutputs(TIM3,ENABLE);
	TIM_Cmd(TIM3, ENABLE);
	
	TIM_ARRPreloadConfig(TIM4, ENABLE);
	TIM_CtrlPWMOutputs(TIM4,ENABLE);
	TIM_Cmd(TIM4, ENABLE);
	TIM3->CCR1 = 6999;
	TIM4->CCR3 = 6999;
}

void move_forward() {
	GPIO_SetBits(GPIOB, GPIO_Pin_9);      //9 1
	GPIO_ResetBits(GPIOB, GPIO_Pin_5);    //5 0
	GPIO_SetBits(GPIOB, GPIO_Pin_6);      //6 1
	GPIO_ResetBits(GPIOB, GPIO_Pin_7);    //7 0
}

void move_backward() {
	GPIO_ResetBits(GPIOB, GPIO_Pin_9);    //9 0
	GPIO_SetBits(GPIOB, GPIO_Pin_5);      //5 1
	GPIO_ResetBits(GPIOB, GPIO_Pin_6);    //6 0
	GPIO_SetBits(GPIOB, GPIO_Pin_7);      //7 1
}

void stop() {
	GPIO_ResetBits(GPIOB, GPIO_Pin_9);
	GPIO_ResetBits(GPIOB, GPIO_Pin_5);
	GPIO_ResetBits(GPIOB, GPIO_Pin_6);
	GPIO_ResetBits(GPIOB, GPIO_Pin_7);
}
