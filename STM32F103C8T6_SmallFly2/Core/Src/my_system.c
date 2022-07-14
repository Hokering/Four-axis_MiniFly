#include "my_system.h"
#include "main.h"
float POWER=0;

_System System;

void System_init(void )
{
		printf("The quadrotor uav is being initialized! \n");
		printf("Hardware:%.2f\n",System.Version.Hard);
		printf("Software:%.2f\n",System.Version.Soft);

		//��ȡ��ص�ѹ
		Get_Power();
		System.Power.Start = System.Power.Now;
    printf("Battery Voltage:%f\n",System.Power.Start);
		
    //NRF��ʼ��
    NRF_Dev_Init();
		printf("2.4G NRF Initialization Succeeded \n");

    //RGB��ʼ��
//	RGB_Reset();

    //���õδ�ʱ��	1ms
    SysTick_Config(SystemCoreClock/1000);

    //���PWM��ʼ��
    HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_2);
    HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_3);
    HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_4);
		Moto_Control(0,0,0,0);
		
//		while(1);

    //�����ǳ�ʼ��
    mpu6050_init();
    Butterworth_Parameter_Init();
		printf("IMU Initialization Succeeded \n");

    //��ѹ�Ƴ�ʼ��
    while(!Spl_init());
		printf("Barometer Initialization Succeeded \n");


    for(uint8 _cnt=0;_cnt<100;_cnt++)				//�����ں���̬ 
    {
				mpu_get();
        Data_steepest();
        IMU_update(2.f,&(sensor.Gyro_deg), &(sensor.Acc_mmss),&imu_data);
    }

    //��ʼ��ͨ���ر�ָʾ��
    HAL_GPIO_WritePin(LED1_GPIO_Port,LED1_Pin,1);
		printf("Initialization Succeeded \n");
}



void Get_Power(void)
{
    uint16 _temp;
    HAL_ADC_Start(&hadc1);
    HAL_ADC_PollForConversion(&hadc1,100);
    _temp = HAL_ADC_GetValue(&hadc1);
    System.Power.Now = _temp *0.0015871f;
}