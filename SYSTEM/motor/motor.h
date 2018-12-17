#ifndef _motor_H
#define _motor_H
#include "stm32f10x.h"
 
void motor_init(void);
void move_forward(void);
void stop(void);
void move_backward(void);

#endif
