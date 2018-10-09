#ifndef _CAMERA_H_
#define _CAMERA_H_

#define DEBUG_CAMERA

#include "stm32f10x.h"

void camera_init(void);
void get_pic(void);
void RD_TSL(void);

float getCameraAngle(void);

void SendImageData(unsigned char * ImageData);

#endif
