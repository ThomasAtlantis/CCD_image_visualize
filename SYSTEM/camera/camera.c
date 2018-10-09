#include "camera.h"
#include "sys\sys.h"
#include "stm32f10x.h"
#include "usart\usart.h"
#include "delay\delay.h"

#define TSL_SI    PAout(3)   //SI  
#define TSL_CLK   PAout(2)   //CLK 

u8 ADV[128]={0};
u8 mid; // 中值位置

#define P 1.2
#define I 0.1
#define D 0
int bias = 0, lastBias = 0, llB = 0;
u8 threshold;
u8 allWhite = 0;

int lastDletaAngle = 90, dletaAngle = 0, angle = 0;

extern u16 DEBUG_SIGN;

void calculateMiddle(void);

u16 Get_Adc(u8 ch)   
{
  	//设置指定ADC的规则组通道，一个序列，采样时间
	ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_239Cycles5 );	//ADC1,ADC通道,采样时间为239.5周期	  			    
  
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);		//使能指定的ADC1的软件转换启动功能	
	 
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));//等待转换结束

	return ADC_GetConversionValue(ADC1);	//返回最近一次ADC1规则组的转换结果
}

float getCameraAngle() {
	
	RD_TSL();
	calculateMiddle();
	
	if (allWhite) {
		if (angle < 90)
			bias = 0;
		else
			bias = 116;
	} else {
		bias = mid - 58;   //提取偏差
	}
	
//	if (DEBUG_SIGN & 0x02)
	// printf("last bias %d, bias %d ", lastBias, bias);
	// angle = bias * 0.1 + (bias + lastBias) * 0.8 + (bias - lastBias) * 0.15 + 90; //PID控制
	
	dletaAngle = D * (bias - 2 * lastBias + llB) +
			I * (bias + lastBias) / 2 +
			P * (bias - lastBias);
	
	angle = lastDletaAngle + dletaAngle;
	lastDletaAngle = angle;
	// printf(" mid =  %d  angle = %d  b*0.5=%.2f (b-l)*0.085=%.2f\n ", mid, angle, bias*0.5, (bias - lastBias) * 0.085);
	llB = lastBias;
	lastBias= bias;   //保存上一次的偏差
	return angle;
}

void SamplingDelay(void){
   volatile uint8_t i ;
   for(i=0;i<1;i++) {
    __NOP();
    __NOP();}
   
}


void RD_TSL(void) 
{
    uint8_t i, t = 0;
    extern uint8_t AtemP ;

    TSL_SI = 1;            /* SI  = 1 */
    delay_us(20);
    TSL_CLK = 1;           /* CLK = 1 */
	delay_us(20);
    TSL_SI = 0;            /* SI  = 0 */
    delay_us(30);
//Delay 10us for sample the first pixel
    for(i = 0; i < 200; i++) {                    //更改250，让CCD的图像看上去比较平滑，
      SamplingDelay();  //200ns                  //把该值改大或者改小达到自己满意的结果。
    }
//Sampling Pixel 1

    ADV[t++] = Get_Adc(ADC_Channel_8)>>4;
    TSL_CLK = 0;           /* CLK = 0 */

    for(i=1; i<128; i++) {
        delay_us(30);
        TSL_CLK = 1;       /* CLK = 1 */
        delay_us(30);
        //Sampling Pixel 2~128
		ADV[i] = Get_Adc(ADC_Channel_8)>>4;
        TSL_CLK = 0;       /* CLK = 0 */
    }
    delay_us(30);
    TSL_CLK = 1;           /* CLK = 1 */
	delay_us(30);
    TSL_CLK = 0;           /* CLK = 0 */
}
	

void get_pic(void)   
{  
	int j;	
    //for(i=0; i<100;i++) 
	//{ 
    RD_TSL();           
   	j=0;
	for(j=0;j<128;j++)
	{
		if(ADV[j] ==0XFF)  ADV[j]--;
	}   
	//}
}	
void camera_init(void)
{
	ADC_InitTypeDef ADC_InitStructure; 
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB |RCC_APB2Periph_ADC1, ENABLE );	  //使能ADC1通道时钟
 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_SetBits(GPIOA, GPIO_Pin_2| GPIO_Pin_3);
	
	RCC_ADCCLKConfig(RCC_PCLK2_Div4);   //设置ADC分频因子6 72M/6=12,ADC最大时间不能超过14M

	//PB0 作为模拟通道输入引脚                         
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;		//模拟输入引脚
	GPIO_Init(GPIOB, &GPIO_InitStructure);	

	ADC_DeInit(ADC1);  //复位ADC1,将外设 ADC1 的全部寄存器重设为缺省值

	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;	//ADC工作模式:ADC1和ADC2工作在独立模式
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;	//模数转换工作在单通道模式
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;	//模数转换工作在单次转换模式
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	//转换由软件而不是外部触发启动
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;	//ADC数据右对齐
	ADC_InitStructure.ADC_NbrOfChannel = 1;	//顺序进行规则转换的ADC通道的数目
	ADC_Init(ADC1, &ADC_InitStructure);	//根据ADC_InitStruct中指定的参数初始化外设ADCx的寄存器   

  
	ADC_Cmd(ADC1, ENABLE);	//使能指定的ADC1
	
	ADC_ResetCalibration(ADC1);	//使能复位校准  
	 
	while(ADC_GetResetCalibrationStatus(ADC1));	//等待复位校准结束
	
	ADC_StartCalibration(ADC1);	 //开启AD校准
 
	while(ADC_GetCalibrationStatus(ADC1));	 //等待校准结束
 
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);		//使能指定的ADC1的软件转换启动功能
}

void calculateMiddle() {
	static u16 i,j,Left,Right;
	static u16 value1_max,value1_min;
	
	value1_max=ADV[0];  //动态阈值算法，读取最大和最小值
	value1_min=ADV[0];  //最小值
    for(i=5;i<123;i++)   //两边各去掉5个点
    {
       if(value1_max<=ADV[i])
			value1_max=ADV[i];
	   if(value1_min>=ADV[i])
			value1_min=ADV[i];
    }
	if (value1_max - value1_min < 50 ) {
		allWhite = 1;
		return;
	} else 
		allWhite = 0;
	
	threshold=(value1_max + value1_min)/2;	  //计算出本次中线提取的阈值

	for(i = 5;i<118; i++)   //寻找左边跳变沿
	{
		if( ADV[i  ]>threshold&&   // 连续三个点比阈值大，连续三个点比阈值小
			ADV[i+1]>threshold&&
			ADV[i+2]>threshold&&
			ADV[i+3]<threshold&&
			ADV[i+4]<threshold&&
			ADV[i+5]<threshold)
			{	
				Left=i;
				break;	
			}
	}
	 for(j = 118;j>5; j--)//寻找右边跳变沿
	{
		if( ADV[j  ]<threshold&&
			ADV[j+1]<threshold&&
			ADV[j+2]<threshold&&
			ADV[j+3]>threshold&&
			ADV[j+4]>threshold&&
			ADV[j+5]>threshold)
			{	
			  Right=j;
			  break;	
			}
	}
	
	if (Right == 5 && Left < 118)
		Right = 118;
	else if (Left == 118 && Right > 5)
		Left = 5;
	
	if (Left == 5 && Right == 118)
		allWhite = 1;
	else 
		allWhite = 0;
	
	mid=(Right+Left)/2;//计算中线位置
	// printf("max: %d min: %d max-min: %d \n", value1_max, value1_min, value1_max - value1_min);
}

void SCI0_SendChar1(char c) {
	printf("%c", c);
}

void SendHex(unsigned char hex) {
  unsigned char temp;
  temp = hex >> 4;
  if(temp < 10) {
    SCI0_SendChar1(temp + '0');
  } else {
    SCI0_SendChar1(temp - 10 + 'A');
  }
  temp = hex & 0x0F;
  if(temp < 10) {
    SCI0_SendChar1(temp + '0');
  } else {
    SCI0_SendChar1(temp - 10 + 'A');
  }
}

void SendImageData(unsigned char * ImageData) {

    unsigned char i;
    unsigned char crc = 0;

    /* Send Data */
    SCI0_SendChar1('*');
    SCI0_SendChar1('L');
    SCI0_SendChar1('D');

    SendHex(0);
    SendHex(0);
    SendHex(0);
    SendHex(0);

    for(i=0; i<128; i++) {
      SendHex(*ImageData++);
    }

    SendHex(crc);
    SCI0_SendChar1('#');
}

