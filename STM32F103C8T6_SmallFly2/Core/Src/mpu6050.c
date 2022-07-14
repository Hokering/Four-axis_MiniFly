#include "mpu6050.h"

//S_INT16_XYZ	MPU_ACC,MPU_GYRO,MPU_OFFSET={15,-24,10};    //big
S_INT16_XYZ	MPU_ACC,MPU_GYRO,MPU_OFFSET={-18,11,-10};        //small
//S_INT16_XYZ	MPU_ACC,MPU_GYRO,MPU_OFFSET={-2,0,115};
//设置MPU6050的数字低通滤波器
//lpf:数字低通滤波频率(Hz)
//返回值:0,设置成功
//    其他,设置失败 
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
		//MPU_Write_Byte(MPU_CFG_REG,data);//设置数字低通滤波器  
}


void mpu6050_init(void)
{
		HAL_StatusTypeDef status;
		HAL_I2C_StateTypeDef flag;
		unsigned char pdata;
		//检查设备是否准备好  地址   检查 次数 超时时间  
		status=HAL_I2C_IsDeviceReady(&hi2c1, ADDRESS_W, 10, HAL_MAX_DELAY);
		//检查总线是否准备好
		flag=HAL_I2C_GetState(&hi2c1);
		//发送写寄存器命令
		pdata=0x80; //复位MPU
		HAL_I2C_Mem_Write(&hi2c1, ADDRESS_W, MPU_PWR_MGMT1_REG, 1, &pdata, 1, HAL_MAX_DELAY); //写0x80复位
		status=HAL_I2C_IsDeviceReady(&hi2c1, ADDRESS_W, 10, HAL_MAX_DELAY);
		HAL_Delay(1000);  //复位后需要等待一段时间 等待芯片复位完成 
		

		while (pdata != 0x68)
		{
			//唤醒MPU
			pdata=0x01; 
			HAL_I2C_Mem_Write(&hi2c1, ADDRESS_W, MPU_PWR_MGMT1_REG, 1, &pdata, 1, HAL_MAX_DELAY); //写0x80复位
			
			//设陀螺仪 寄存器  2000  3
			pdata=0x18; 
			HAL_I2C_Mem_Write(&hi2c1, ADDRESS_W, MPU_GYRO_CFG_REG, 1, &pdata, 1, HAL_MAX_DELAY); 
			HAL_Delay(10);
			
			//设置加速度传感器量程 8g
			pdata=0x10; 
			HAL_I2C_Mem_Write(&hi2c1, ADDRESS_W, MPU_ACCEL_CFG_REG, 1, &pdata, 1, HAL_MAX_DELAY); 
			HAL_Delay(10);

			//陀螺仪采样分频设置
			pdata=1; //1000/500-1  这个还需要查资料查看  原因 和计算方法
			HAL_I2C_Mem_Write(&hi2c1, ADDRESS_W, MPU_SAMPLE_RATE_REG, 1, &pdata, 1, HAL_MAX_DELAY); 
			HAL_Delay(10);

			//关闭所有中断
			pdata=0;
			HAL_I2C_Mem_Write(&hi2c1, ADDRESS_W, MPU_INT_EN_REG, 1, &pdata, 1, HAL_MAX_DELAY); 
			HAL_Delay(10);

			//I2C 主模式的 外接磁力传感器接口关闭
			pdata=0;
			HAL_I2C_Mem_Write(&hi2c1, ADDRESS_W, MPU_USER_CTRL_REG, 1, &pdata, 1, HAL_MAX_DELAY); 
			HAL_Delay(10);

			//关闭FIFO 
			pdata=0;
			HAL_I2C_Mem_Write(&hi2c1, ADDRESS_W, MPU_FIFO_EN_REG, 1, &pdata, 1, HAL_MAX_DELAY); 
			HAL_Delay(10);

			//中断旁路设置 低电平有效
			pdata=0X80;
			HAL_I2C_Mem_Write(&hi2c1, ADDRESS_W, MPU_INTBP_CFG_REG, 1, &pdata, 1, HAL_MAX_DELAY); 
			HAL_Delay(10);

			//设置低通滤波器
			MPU_Set_LPF(100);
			HAL_Delay(10);

			//使能 陀螺仪 和加速度 工作
			pdata=0;
			HAL_I2C_Mem_Write(&hi2c1, ADDRESS_W, MPU_PWR_MGMT2_REG, 1, &pdata, 1, HAL_MAX_DELAY);
			
			//读器件ID  默认是 0x68 
			pdata=MPU_DEVICE_ID_REG;
			HAL_I2C_Mem_Read(&hi2c1, ADDRESS_R, MPU_DEVICE_ID_REG, 1, &pdata, 1, HAL_MAX_DELAY); 
			HAL_Delay(10);
	}
	
}	

//读取MPU6050数据
void mpu_get(void)
{

  uint8 dat[6];
  
	HAL_I2C_Mem_Read(&hi2c1, ADDRESS_W, MPU_ACCEL_XOUTH_REG, 1, dat, 6, HAL_MAX_DELAY);   //加速度
	System.imu_data.ACC_RAW.X = (int16)(((uint16)dat[0]<<8 | dat[1]));
	System.imu_data.ACC_RAW.Y = (int16)(((uint16)dat[2]<<8 | dat[3]));
	System.imu_data.ACC_RAW.Z = (int16)(((uint16)dat[4]<<8 | dat[5]));

	HAL_I2C_Mem_Read(&hi2c1, ADDRESS_W, MPU_GYRO_XOUTH_REG, 1, dat, 6, HAL_MAX_DELAY);    //陀螺仪
	System.imu_data.GYRO_RAW.X = (int16)(((uint16)dat[0]<<8 | dat[1]));
	System.imu_data.GYRO_RAW.Y = (int16)(((uint16)dat[2]<<8 | dat[3]));
	System.imu_data.GYRO_RAW.Z = (int16)(((uint16)dat[4]<<8 | dat[5]));
  
  System.imu_data.GYRO_RAW.X -= MPU_OFFSET.X;     //消除零飘
  System.imu_data.GYRO_RAW.Y -= MPU_OFFSET.Y;
  System.imu_data.GYRO_RAW.Z -= MPU_OFFSET.Z;
}