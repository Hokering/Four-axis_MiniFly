#include "mpu6050.h"

//S_INT16_XYZ	MPU_ACC,MPU_GYRO,MPU_OFFSET={15,-24,10};    //big
S_INT16_XYZ	MPU_ACC,MPU_GYRO,MPU_OFFSET={-18,11,-10};        //small
//S_INT16_XYZ	MPU_ACC,MPU_GYRO,MPU_OFFSET={-2,0,115};
//����MPU6050�����ֵ�ͨ�˲���
//lpf:���ֵ�ͨ�˲�Ƶ��(Hz)
//����ֵ:0,���óɹ�
//    ����,����ʧ�� 
unsigned char  MPU_Set_LPF(unsigned short lpf)
{
	unsigned char data=0;
	if(lpf>=188)data=1;
	else if(lpf>=98)data=2;
	else if(lpf>=42)data=3;
	else if(lpf>=20)data=4;
	else if(lpf>=10)data=5;
	else data=6; 
	
	return HAL_I2C_Mem_Write(&hi2c1, ADDRESS_W, MPU_CFG_REG, 1, &data, 1, HAL_MAX_DELAY); 
		//MPU_Write_Byte(MPU_CFG_REG,data);//�������ֵ�ͨ�˲���  
}


void mpu6050_init(void)
{
		HAL_StatusTypeDef status;
		HAL_I2C_StateTypeDef flag;
		unsigned char pdata;
		//����豸�Ƿ�׼����  ��ַ   ��� ���� ��ʱʱ��  
		status=HAL_I2C_IsDeviceReady(&hi2c1, ADDRESS_W, 10, HAL_MAX_DELAY);
		//��������Ƿ�׼����
		flag=HAL_I2C_GetState(&hi2c1);
		//����д�Ĵ�������
		pdata=0x80; //��λMPU
		HAL_I2C_Mem_Write(&hi2c1, ADDRESS_W, MPU_PWR_MGMT1_REG, 1, &pdata, 1, HAL_MAX_DELAY); //д0x80��λ
		status=HAL_I2C_IsDeviceReady(&hi2c1, ADDRESS_W, 10, HAL_MAX_DELAY);
		HAL_Delay(1000);  //��λ����Ҫ�ȴ�һ��ʱ�� �ȴ�оƬ��λ��� 
		

		while (pdata != 0x68)
		{
			//����MPU
			pdata=0x01; 
			HAL_I2C_Mem_Write(&hi2c1, ADDRESS_W, MPU_PWR_MGMT1_REG, 1, &pdata, 1, HAL_MAX_DELAY); //д0x80��λ
			
			//�������� �Ĵ���  2000  3
			pdata=0x18; 
			HAL_I2C_Mem_Write(&hi2c1, ADDRESS_W, MPU_GYRO_CFG_REG, 1, &pdata, 1, HAL_MAX_DELAY); 
			HAL_Delay(10);
			
			//���ü��ٶȴ��������� 8g
			pdata=0x10; 
			HAL_I2C_Mem_Write(&hi2c1, ADDRESS_W, MPU_ACCEL_CFG_REG, 1, &pdata, 1, HAL_MAX_DELAY); 
			HAL_Delay(10);

			//�����ǲ�����Ƶ����
			pdata=1; //1000/500-1  �������Ҫ�����ϲ鿴  ԭ�� �ͼ��㷽��
			HAL_I2C_Mem_Write(&hi2c1, ADDRESS_W, MPU_SAMPLE_RATE_REG, 1, &pdata, 1, HAL_MAX_DELAY); 
			HAL_Delay(10);

			//�ر������ж�
			pdata=0;
			HAL_I2C_Mem_Write(&hi2c1, ADDRESS_W, MPU_INT_EN_REG, 1, &pdata, 1, HAL_MAX_DELAY); 
			HAL_Delay(10);

			//I2C ��ģʽ�� ��Ӵ����������ӿڹر�
			pdata=0;
			HAL_I2C_Mem_Write(&hi2c1, ADDRESS_W, MPU_USER_CTRL_REG, 1, &pdata, 1, HAL_MAX_DELAY); 
			HAL_Delay(10);

			//�ر�FIFO 
			pdata=0;
			HAL_I2C_Mem_Write(&hi2c1, ADDRESS_W, MPU_FIFO_EN_REG, 1, &pdata, 1, HAL_MAX_DELAY); 
			HAL_Delay(10);

			//�ж���·���� �͵�ƽ��Ч
			pdata=0X80;
			HAL_I2C_Mem_Write(&hi2c1, ADDRESS_W, MPU_INTBP_CFG_REG, 1, &pdata, 1, HAL_MAX_DELAY); 
			HAL_Delay(10);

			//���õ�ͨ�˲���
			MPU_Set_LPF(100);
			HAL_Delay(10);

			//ʹ�� ������ �ͼ��ٶ� ����
			pdata=0;
			HAL_I2C_Mem_Write(&hi2c1, ADDRESS_W, MPU_PWR_MGMT2_REG, 1, &pdata, 1, HAL_MAX_DELAY);
			
			//������ID  Ĭ���� 0x68 
			pdata=MPU_DEVICE_ID_REG;
			HAL_I2C_Mem_Read(&hi2c1, ADDRESS_R, MPU_DEVICE_ID_REG, 1, &pdata, 1, HAL_MAX_DELAY); 
			HAL_Delay(10);
	}
	
}	

//��ȡMPU6050����
void mpu_get(void)
{

  uint8 dat[6];
  
	HAL_I2C_Mem_Read(&hi2c1, ADDRESS_W, MPU_ACCEL_XOUTH_REG, 1, dat, 6, HAL_MAX_DELAY);   //���ٶ�
	System.imu_data.ACC_RAW.X = (int16)(((uint16)dat[0]<<8 | dat[1]));
	System.imu_data.ACC_RAW.Y = (int16)(((uint16)dat[2]<<8 | dat[3]));
	System.imu_data.ACC_RAW.Z = (int16)(((uint16)dat[4]<<8 | dat[5]));

	HAL_I2C_Mem_Read(&hi2c1, ADDRESS_W, MPU_GYRO_XOUTH_REG, 1, dat, 6, HAL_MAX_DELAY);    //������
	System.imu_data.GYRO_RAW.X = (int16)(((uint16)dat[0]<<8 | dat[1]));
	System.imu_data.GYRO_RAW.Y = (int16)(((uint16)dat[2]<<8 | dat[3]));
	System.imu_data.GYRO_RAW.Z = (int16)(((uint16)dat[4]<<8 | dat[5]));
  
  System.imu_data.GYRO_RAW.X -= MPU_OFFSET.X;     //������Ʈ
  System.imu_data.GYRO_RAW.Y -= MPU_OFFSET.Y;
  System.imu_data.GYRO_RAW.Z -= MPU_OFFSET.Z;
}