#include "motor.h"

// left  B5 B7 
// right B4 B6

void motor_init()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  tim;
	TIM_OCInitTypeDef        oc;
	GPIO_InitTypeDef         gpio;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);	
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	gpio.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;  // CH1  PB6,   CH2  PB7
	gpio.GPIO_Mode = GPIO_Mode_AF_PP;
	gpio.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &gpio);
    
	tim.TIM_Period=19999;    //arr             Fpwm=72M/((arr+1)*(psc+1))
	tim.TIM_Prescaler=71;   //psc             duty circle= TIMx->CCRx/arr
	tim.TIM_ClockDivision=TIM_CKD_DIV1;
	tim.TIM_CounterMode=TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM4, &tim);
	
	oc.TIM_OCMode = TIM_OCMode_PWM2;
	oc.TIM_OutputState = TIM_OutputState_Enable;
	oc.TIM_OutputNState = TIM_OutputNState_Disable;
	oc.TIM_Pulse = 0;
	oc.TIM_OCPolarity = TIM_OCPolarity_High;
	oc.TIM_OCNPolarity = TIM_OCPolarity_Low;
	oc.TIM_OCIdleState = TIM_OCIdleState_Reset;
	oc.TIM_OCNIdleState = TIM_OCIdleState_Reset;
	TIM_OC1Init(TIM4, &oc);
	TIM_OC2Init(TIM4, &oc);
	
	TIM_ARRPreloadConfig(TIM4, ENABLE);
    
	TIM_CtrlPWMOutputs(TIM4,ENABLE);

	TIM_Cmd(TIM4, ENABLE);
	GPIO_ResetBits(GPIOB, GPIO_Pin_4 | GPIO_Pin_5);
}

void set_left_speed(float percent) {
	percent = 1 - percent;
	TIM4 -> CCR2 = percent * 19999;
}

void set_right_speed(float percent) {
	percent = 1 - percent;
	TIM4 -> CCR1 = percent * 19999;
}

void motor_forward() {
	set_left_speed(1);
	set_right_speed(1);
}


void motor_stop() {
	set_left_speed(0);
	set_right_speed(0);
}
