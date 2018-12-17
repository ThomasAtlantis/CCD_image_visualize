#include "stm32f10x.h"
#include "delay\delay.h"
#include "usart\usart.h"
#include "adc\adc.h"
#include "motor\motor.h"
#include "stervo\stervo.h"
#define TSL_SI    PAout(7)   //SI  7
#define TSL_CLK   PAout(2)   //CLK 2
							 //PA0        AO
u16 ADV[128] = {0};
void RD_TSL(void) {
	u8 i = 0, tslp = 0;
	TSL_CLK = 1;
	TSL_SI = 0; 
	delay_us(1);

	TSL_SI = 1; 
	TSL_CLK = 0;
	delay_us(1);

	TSL_CLK = 1;
	TSL_SI = 0;
	delay_us(1); 
	for(i = 0; i < 128; i++) { 
		TSL_CLK = 0; 
		delay_us(1);
		ADV[tslp] = (get_Adc(0))>>4;
		++ tslp;
		TSL_CLK = 1;
		delay_us(1);
	}  
}

void CCD(void)   
{  
	int i, j;
	u16 mean = 0;
	u8 data[128];
	u8 l_index, r_index, flag, mid;
	int tmp;
	int angle;
    for(i = 0; i < 100; i++) { 
	    RD_TSL();
			// 二值化算法
			for (j = 0; j < 128; j++) {
				if (ADV[j] == 0XFF) ADV[j] --;
				data[j] = ADV[j];
				//printf("%d ", data[j]);
				mean += data[j];
			}
			mean = mean >> 7;
			for (j = 0; j < 128; j++) {
				if (data[j] < mean) data[j] = 0;
				else data[j] = 255;
				//printf("%d ", data[j]);
			}
			flag = 0;
			l_index = r_index = 128;
			for (j = 0; j < 128; j++) {
				if (data[j] == 0 && flag == 0) {
					l_index = j;
					flag = 1;
				}
				if (data[j] == 255 && flag == 1) {
					r_index = j;
					break;
				}
			}
			mid = (l_index + r_index) / 2;
			tmp = mid - 63;
			angle = (mid - 63) * 40 / 64 + 120;//6999 搭配 40 线性
			//if (tmp < 0) angle = 120 - angle;
			//else angle = 120 + angle;
			angle = angle > 160 ? 160: angle;
			angle = angle < 80 ? 80: angle;
			set_stervo_angle(angle);
			//printf("\r\n");
	}
}

void ccd_Init(void) {    
	//初始化I=?
 	RCC->APB2ENR|=1<<2;    //使能PORTA口时钟 
	GPIOA->CRL&=0XFFFF0FFF;//PA3 anolog输入
	GPIOA->CRL&=0X0FFFF0FF;//PA2   7 
	GPIOA->CRL|=0X20000200;//Pa2   7 推挽输出 2MHZ   
}

int main() {
	SystemInit();
	delay_init();
	stervo_init();
	motor_init();
	adc_init();
	uart_init(9600);
	ccd_Init();
	set_stervo_angle(120);
	delay_ms(100);
	move_forward();
	while (1) {
		CCD();
	}
}
