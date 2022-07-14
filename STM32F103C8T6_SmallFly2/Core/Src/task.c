#include "task.h"

float vofa_data[10];

int16 	M1_PWM,
        M2_PWM,
        M3_PWM,
        M4_PWM;

uint8 Run_flag = 1;


//系统任务配置，创建不同执行频率的“线程”
static sched_task_t sched_tasks[] =
{
    //任务n,        周期ms,   上次时间us
    {Loop_Task_1,      2,  	0 },
    {Loop_Task_2,      4,  	0 },
    {Loop_Task_3,      8, 	0 },
    {Loop_Task_4,     16,   0 },
    {Loop_Task_5,     32,   0 },
		{Led_Task,      1000,   0 },
};

//根据数组长度，判断线程数量
#define TASK_NUM (sizeof(sched_tasks)/sizeof(sched_task_t))
uint8 task_running(void)
{
    uint8_t index = 0;

    //循环判断所有线程任务
    uint32_t time_now;
    for(index=0; index < TASK_NUM; index++)
    {
        //获取系统当前时间，单位US
        time_now = uwTick;
        //进行判断，如果当前时间减去上一次执行的时间，大于等于该线程的执行周期，则执行线程
        if(time_now - sched_tasks[index].last_run >= sched_tasks[index].interval_ticks)
        {
            //执行线程函数				将调度时间差传入参数
            sched_tasks[index].task_func(time_now - sched_tasks[index].last_run);
            //更新线程的执行时间
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
	
		//角速度内环    
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
		//四元数结算欧拉角
    IMU_update(0.004f,&(sensor.Gyro_deg), &(sensor.Acc_mmss),&imu_data);

    //角度环PID
    PID_Realize(&pit_angle, Pit_PID, Targetangle.pit,RealCoordinate.pit);	//角度放大100倍提高pid精度
    PID_Realize(&yaw_angle, Yaw_PID, -Targetangle.yaw,RealCoordinate.yaw);
    PID_Realize(&rol_angle, Rol_PID,       0        ,Targetangle.rol);              //M1 M3提速向左旋转
   
		

    


}

void Loop_Task_3(uint8_t take_time)		//8ms
{

    //接收遥控器数据
    Reciver_Buff();
	
    if(Remoto_data.Flag)			//接受完成
    {
				//摇杆左左上  右左下解锁
//				FlyUnlock();			
				Run_flag = 1;
				//摇杆数据转换目标角度
				ToTargetangle();
				
        //处理特殊指令位
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


    //程序正常运行，启动呼吸灯
//	RGB_Breathing();
//	AK8975_Read();
//		Spl_Get();
//		spl_filter();
  Get_Power();

    //1s未接收到遥控器数据，判断遥控器失联
    if(uwTick - Remoto_data.Last_Time > 1000)
    {
       Run_flag  =0;
    }

    //飞机侧翻检测
    if(ABS(imu_data.rol) >45.0f || ABS(imu_data.pit) >45.0f)
    {
        Run_flag =0;
    }

//    //电量过低
//    if(POWER <=3.8f)
//    {

//    }

}


void Led_Task(uint8_t take_time)
{

	HAL_GPIO_TogglePin(LED1_GPIO_Port,LED1_Pin);
}