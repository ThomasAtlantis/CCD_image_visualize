#include "infrared.h"
#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "adc/adc.h"

void infrared_init(void) {
	GPIO_InitTypeDef gpio_init;
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA, ENABLE);	
	gpio_init.GPIO_Mode = GPIO_Mode_AIN;
	gpio_init.GPIO_Pin = GPIO_Pin_1| GPIO_Pin_2| GPIO_Pin_3| GPIO_Pin_4;
	gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &gpio_init);
}
