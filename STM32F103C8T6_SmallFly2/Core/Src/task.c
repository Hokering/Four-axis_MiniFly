#include "task.h"

float vofa_data[10];

int16 	M1_PWM,
        M2_PWM,
        M3_PWM,
        M4_PWM;

uint8 Run_flag = 1;


//ϵͳ�������ã�������ִͬ��Ƶ�ʵġ��̡߳�
static sched_task_t sched_tasks[] =
{
    //����n,        ����ms,   �ϴ�ʱ��us
    {Loop_Task_1,      2,  	0 },
    {Loop_Task_2,      4,  	0 },
    {Loop_Task_3,      8, 	0 },
    {Loop_Task_4,     16,   0 },
    {Loop_Task_5,     32,   0 },
		{Led_Task,      1000,   0 },
};

//�������鳤�ȣ��ж��߳�����
#define TASK_NUM (sizeof(sched_tasks)/sizeof(sched_task_t))
uint8 task_running(void)
{
    uint8_t index = 0;

    //ѭ���ж������߳�����
    uint32_t time_now;
    for(index=0; index < TASK_NUM; index++)
    {
        //��ȡϵͳ��ǰʱ�䣬��λUS
        time_now = uwTick;
        //�����жϣ������ǰʱ���ȥ��һ��ִ�е�ʱ�䣬���ڵ��ڸ��̵߳�ִ�����ڣ���ִ���߳�
        if(time_now - sched_tasks[index].last_run >= sched_tasks[index].interval_ticks)
        {
            //ִ���̺߳���				������ʱ�������
            sched_tasks[index].task_func(time_now - sched_tasks[index].last_run);
            //�����̵߳�ִ��ʱ��
            sched_tasks[index].last_run = time_now;

        }
    }

    return 0;
}



void Loop_Task_1(uint8_t take_time)		//2ms
{
    mpu_get();
    Data_steepest();
		Sensor_Real_Coordinate();
	
		//���ٶ��ڻ�    
    PID_Realize(&pit_rate, Pit_Rate_PID, RealCoordinate.Gyro.X, pit_angle.Output);
    PID_Realize(&yaw_rate, Yaw_Rate_PID, RealCoordinate.Gyro.Y, yaw_angle.Output);
		PID_Realize(&rol_rate, Rol_Rate_PID, RealCoordinate.Gyro.Z, rol_angle.Output);
  
	    
		M1_PWM = M2_PWM = M3_PWM = M4_PWM = Targetangle.alt;
		
//		M1_PWM += - pit_rate.Output + yaw_rate.Output + rol_rate.Output; 
//		M2_PWM += - pit_rate.Output - yaw_rate.Output - rol_rate.Output;
//		M3_PWM += + pit_rate.Output - yaw_rate.Output - rol_rate.Output;
//		M4_PWM += + pit_rate.Output + yaw_rate.Output + rol_rate.Output;

    M1_PWM = LIMIT(M1_PWM,0,1200);
    M2_PWM = LIMIT(M2_PWM,0,1200);
    M3_PWM = LIMIT(M3_PWM,0,1200);
    M4_PWM = LIMIT(M4_PWM,0,1200);
    
    if(Run_flag &&  uwTick >3000)
			Moto_Control(M1_PWM,M2_PWM,M3_PWM,M4_PWM);
		else 
			Moto_Control(0,0,0,0);
    
}



void Loop_Task_2(uint8_t take_time)		//4ms
{
		//��Ԫ������ŷ����
    IMU_update(0.004f,&(sensor.Gyro_deg), &(sensor.Acc_mmss),&imu_data);

    //�ǶȻ�PID
    PID_Realize(&pit_angle, Pit_PID, Targetangle.pit,RealCoordinate.pit);	//�ǶȷŴ�100�����pid����
    PID_Realize(&yaw_angle, Yaw_PID, -Targetangle.yaw,RealCoordinate.yaw);
    PID_Realize(&rol_angle, Rol_PID,       0        ,Targetangle.rol);              //M1 M3����������ת
   
		

    


}

void Loop_Task_3(uint8_t take_time)		//8ms
{

    //����ң��������
    Reciver_Buff();
	
    if(Remoto_data.Flag)			//�������
    {
				//ҡ��������  �����½���
//				FlyUnlock();			
				Run_flag = 1;
				//ҡ������ת��Ŀ��Ƕ�
				ToTargetangle();
				
        //��������ָ��λ
        switch ( Remoto_data.control)
        {
        case 0x01:
            break;
        case 0x02:
            break;
        case 0x03:
            break;
        }
        Remoto_data.Flag = 0;
    }




}


void Loop_Task_4(uint8_t take_time)		//16ms
{
    vofa_data[0] = M1_PWM;
    vofa_data[1] = M2_PWM;
    vofa_data[2] = M3_PWM;
    vofa_data[3] = M4_PWM;
    vofa_data[4] = Targetangle.alt;
    vofa_data[5] = Run_flag;
  
	
//	  vofa_data[0] = Remoto_data.L_X;
//    vofa_data[1] = Remoto_data.L_Y;
//    vofa_data[2] = Remoto_data.R_X;
//    vofa_data[3] = Remoto_data.R_Y;
//    vofa_data[4] = 0;
//    vofa_data[5] = 0;
    SendUart_to_Vofa(vofa_data,6);
//    
}


void Loop_Task_5(uint8_t take_time)		//32ms
{


    //�����������У�����������
//	RGB_Breathing();
//	AK8975_Read();
//		Spl_Get();
//		spl_filter();
  Get_Power();

    //1sδ���յ�ң�������ݣ��ж�ң����ʧ��
    if(uwTick - Remoto_data.Last_Time > 1000)
    {
       Run_flag  =0;
    }

    //�ɻ��෭���
    if(ABS(imu_data.rol) >45.0f || ABS(imu_data.pit) >45.0f)
    {
        Run_flag =0;
    }

//    //��������
//    if(POWER <=3.8f)
//    {

//    }

}


void Led_Task(uint8_t take_time)
{

	HAL_GPIO_TogglePin(LED1_GPIO_Port,LED1_Pin);
}