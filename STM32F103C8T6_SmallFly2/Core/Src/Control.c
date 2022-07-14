#include "control.h"


/****************************************

注意：
     陀螺仪方向

	M1————————————————————————————M2
		|														| 				
		|														| 				|
		|														| 				|
		|	 ————————									| 				|
		|	|				0|								|  -------------->    Y轴
		|	|			   |								| 				|
		|	|				 |		   					| 				|
		|	 ————————									|				  |
		|														| 				↓
		|														| 				X轴
		|														| 
	M4————————————————————————————M3


******************************************/



/****************************************

						+X
						
			M1    ↑    M2
				\   |   /
				 \  |  /
					\ | /
		————————+————————>       Y+
					/ | \
				 /  |  \
				/   |   \
			M4    ↓    M3

注意:
	  M1 和 M3 逆时针旋转
	  M2 和 M4 顺时针旋转

******************************************/

//根据陀螺仪安装方向调整为自己设定X和Y正方向
//调整此函数可以兼容多种陀螺仪安装方式
_Rcoordinate RealCoordinate;
void Sensor_Real_Coordinate(void)
{
		RealCoordinate.Gyro.X = sensor.Gyro_deg.X;
		RealCoordinate.Gyro.Y = sensor.Gyro_deg.Y;
		RealCoordinate.Gyro.Z = sensor.Gyro_deg.Z;
		RealCoordinate.pit =  -imu_data.yaw;
		RealCoordinate.yaw =  -imu_data.pit;
		RealCoordinate.rol =   imu_data.rol;
}


//解锁无人机
uint8 Ulockcout=0;
void FlyUnlock(void)
{

		if( Remoto_data.L_X < 20 && Remoto_data.L_Y < 20 && Remoto_data.R_X < 20 && Remoto_data.R_Y > 110)
		{
				if(++Ulockcout >100)
				{
						Ulockcout = 0;
						System.Flag.Unlock = 1;
						Run_flag = 1;
				}
		}
		else Ulockcout = 0;
}
		

//摇杆数据转换
_Target Targetangle ={0,0,0,0};
void ToTargetangle(void)
{
	
	Targetangle.pit = (float)(Remoto_data.L_Y - 70) * 0.5f;
	Targetangle.yaw = (float)(Remoto_data.L_X - 63) * 0.5f;
	Targetangle.rol = (float)(Remoto_data.R_X - 62) * 0.5f;
	Targetangle.alt = -(float)(Remoto_data.R_Y - 63) * 5.0f;

//	Targetangle.pit = LIMIT(Targetangle.pit,-30,30);
	Targetangle.rol = LIMIT(Targetangle.rol,-30,30);
	Targetangle.yaw = LIMIT(Targetangle.yaw,-30,30);
//	Targetangle.alt = LIMIT(Targetangle.alt,-800,800);
}

////摇杆数据转换
//_Target Targetangle ={0,0,0,0};
//void ToTargetangle(void)
//{
//		Targetangle.pit = (float)(Remoto_data.R_Y ) * 0.5f;
//		Targetangle.rol = (float)Remoto_data.R_X * 0.5f;
//		Targetangle.yaw = (float)Remoto_data.L_X * 0.5f;
//		Targetangle.alt = (float)-(Remoto_data.L_Y - 30);
//	
//		Targetangle.pit = LIMIT(Targetangle.pit,-30,30);
//		Targetangle.rol = LIMIT(Targetangle.rol,-30,30);
//		Targetangle.yaw = LIMIT(Targetangle.yaw,-30,30);
//		Targetangle.alt = LIMIT(Targetangle.alt,-500,500);
//}


//电机控制
void Moto_Control(int16 M1,int16 M2,int16 M3,int16 M4)
{
    //电机限幅
//    LIMIT(M1,0,20);
//    LIMIT(M2,0,20);
//    LIMIT(M3,0,20);
//    LIMIT(M4,0,20);

		
		
//    //输出PWM
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1,M1);		//M1电机
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2,M2);		//M2电机
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_4,M3);		//M3电机
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3,M4);		//M4电机
}



