#include "control.h"


/****************************************

ע�⣺
     �����Ƿ���

	M1��������������������������������������������������������M2
		|														| 				
		|														| 				|
		|														| 				|
		|	 ����������������									| 				|
		|	|				0|								|  -------------->    Y��
		|	|			   |								| 				|
		|	|				 |		   					| 				|
		|	 ����������������									|				  |
		|														| 				��
		|														| 				X��
		|														| 
	M4��������������������������������������������������������M3


******************************************/



/****************************************

						+X
						
			M1    ��    M2
				\   |   /
				 \  |  /
					\ | /
		����������������+����������������>       Y+
					/ | \
				 /  |  \
				/   |   \
			M4    ��    M3

ע��:
	  M1 �� M3 ��ʱ����ת
	  M2 �� M4 ˳ʱ����ת

******************************************/

//���������ǰ�װ�������Ϊ�Լ��趨X��Y������
//�����˺������Լ��ݶ��������ǰ�װ��ʽ
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


//�������˻�
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
		

//ҡ������ת��
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

////ҡ������ת��
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


//�������
void Moto_Control(int16 M1,int16 M2,int16 M3,int16 M4)
{
    //����޷�
//    LIMIT(M1,0,20);
//    LIMIT(M2,0,20);
//    LIMIT(M3,0,20);
//    LIMIT(M4,0,20);

		
		
//    //���PWM
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1,M1);		//M1���
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2,M2);		//M2���
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_4,M3);		//M3���
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3,M4);		//M4���
}



