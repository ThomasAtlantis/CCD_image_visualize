#ifndef _motor_H
#define _motor_H
#include "stm32f10x.h"
#define MOTOR GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15
 
void motor_init(void);
void motor_forward(void);
void motor_stop(void);

void set_left_speed(float);
void set_right_speed(float);

#endif 
