#include "camera.h"
#include "sys\sys.h"
#include "stm32f10x.h"
#include "usart\usart.h"
#include "delay\delay.h"

#define TSL_SI    PAout(3)   //SI  
#define TSL_CLK   PAout(2)   //CLK 

u8 ADV[128]={0};
u8 mid; // ��ֵλ��

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
  	//����ָ��ADC�Ĺ�����ͨ����һ�����У�����ʱ��
	ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_239Cycles5 );	//ADC1,ADCͨ��,����ʱ��Ϊ239.5����	  			    
  
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);		//ʹ��ָ����ADC1�����ת����������	
	 
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));//�ȴ�ת������

	return ADC_GetConversionValue(ADC1);	//�������һ��ADC1�������ת�����
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
		bias = mid - 58;   //��ȡƫ��
	}
	
//	if (DEBUG_SIGN & 0x02)
	// printf("last bias %d, bias %d ", lastBias, bias);
	// angle = bias * 0.1 + (bias + lastBias) * 0.8 + (bias - lastBias) * 0.15 + 90; //PID����
	
	dletaAngle = D * (bias - 2 * lastBias + llB) +
			I * (bias + lastBias) / 2 +
			P * (bias - lastBias);
	
	angle = lastDletaAngle + dletaAngle;
	lastDletaAngle = angle;
	// printf(" mid =  %d  angle = %d  b*0.5=%.2f (b-l)*0.085=%.2f\n ", mid, angle, bias*0.5, (bias - lastBias) * 0.085);
	llB = lastBias;
	lastBias= bias;   //������һ�ε�ƫ��
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
    for(i = 0; i < 200; i++) {                    //����250����CCD��ͼ����ȥ�Ƚ�ƽ����
      SamplingDelay();  //200ns                  //�Ѹ�ֵ�Ĵ���߸�С�ﵽ�Լ�����Ľ����
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

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB |RCC_APB2Periph_ADC1, ENABLE );	  //ʹ��ADC1ͨ��ʱ��
 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_SetBits(GPIOA, GPIO_Pin_2| GPIO_Pin_3);
	
	RCC_ADCCLKConfig(RCC_PCLK2_Div4);   //����ADC��Ƶ����6 72M/6=12,ADC���ʱ�䲻�ܳ���14M

	//PB0 ��Ϊģ��ͨ����������                         
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;		//ģ����������
	GPIO_Init(GPIOB, &GPIO_InitStructure);	

	ADC_DeInit(ADC1);  //��λADC1,������ ADC1 ��ȫ���Ĵ�������Ϊȱʡֵ

	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;	//ADC����ģʽ:ADC1��ADC2�����ڶ���ģʽ
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;	//ģ��ת�������ڵ�ͨ��ģʽ
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;	//ģ��ת�������ڵ���ת��ģʽ
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	//ת��������������ⲿ��������
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;	//ADC�����Ҷ���
	ADC_InitStructure.ADC_NbrOfChannel = 1;	//˳����й���ת����ADCͨ������Ŀ
	ADC_Init(ADC1, &ADC_InitStructure);	//����ADC_InitStruct��ָ���Ĳ�����ʼ������ADCx�ļĴ���   

  
	ADC_Cmd(ADC1, ENABLE);	//ʹ��ָ����ADC1
	
	ADC_ResetCalibration(ADC1);	//ʹ�ܸ�λУ׼  
	 
	while(ADC_GetResetCalibrationStatus(ADC1));	//�ȴ���λУ׼����
	
	ADC_StartCalibration(ADC1);	 //����ADУ׼
 
	while(ADC_GetCalibrationStatus(ADC1));	 //�ȴ�У׼����
 
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);		//ʹ��ָ����ADC1�����ת����������
}

void calculateMiddle() {
	static u16 i,j,Left,Right;
	static u16 value1_max,value1_min;
	
	value1_max=ADV[0];  //��̬��ֵ�㷨����ȡ������Сֵ
	value1_min=ADV[0];  //��Сֵ
    for(i=5;i<123;i++)   //���߸�ȥ��5����
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
	
	threshold=(value1_max + value1_min)/2;	  //���������������ȡ����ֵ

	for(i = 5;i<118; i++)   //Ѱ�����������
	{
		if( ADV[i  ]>threshold&&   // �������������ֵ���������������ֵС
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
	 for(j = 118;j>5; j--)//Ѱ���ұ�������
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
	
	mid=(Right+Left)/2;//��������λ��
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

