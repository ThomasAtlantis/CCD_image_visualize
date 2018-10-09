#include "stm32f10x.h"
#include "delay\delay.h"
#include "usart\usart.h"
#include "adc\adc.h"
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
	int i,j;
	printf("START\r\n");
    for(i = 0; i < 100; i++) { 
	    RD_TSL();           
	   	j = 0;
		for(j = 0; j < 128; j++) {
		    if(ADV[j] == 0XFF)  ADV[j]--;    
		    printf("%d ", ADV[j]);
	    }
		printf("\r\n");
	}
}

void ccd_Init(void) {    
	//初始化IO口
 	RCC->APB2ENR|=1<<2;    //使能PORTA口时钟 
	GPIOA->CRL&=0XFFFF0FFF;//PA3 anolog输入
	GPIOA->CRL&=0X0FFFF0FF;//PA2   7 
	GPIOA->CRL|=0X20000200;//Pa2   7 推挽输出 2MHZ   
}
int main() {
	SystemInit();
	delay_init();
	adc_init();
	uart_init(9600);
	ccd_Init();
	while (1) {
		CCD();
		delay_ms(1);
	}
}
