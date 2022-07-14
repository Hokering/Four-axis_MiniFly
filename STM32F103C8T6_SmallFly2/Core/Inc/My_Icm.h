
#ifndef _My_Icm_h
#define _My_Icm_h

#include "headfile.h"
#include "common.h"




typedef struct
{
	float w;//q0;
	float x;//q1;
	float y;//q2;
	float z;//q3;

	S_FLOAT_XYZ x_vec;
	S_FLOAT_XYZ y_vec;
	S_FLOAT_XYZ z_vec;

	S_FLOAT_XYZ a_acc;
	S_FLOAT_XYZ w_acc;
	
	float rol;
	float pit;
	float yaw;
} _imu_st ;

typedef struct 
{
	uint8 acc_CALIBRATE;
	uint8 gyr_CALIBRATE;
	uint8 vec3d_CALIBRATE;
	
	S_INT16_XYZ Acc_I16;
	S_INT16_XYZ Gyro_I16;

	S_FLOAT_XYZ Acc;
	S_FLOAT_XYZ Acc_mmss;
	S_FLOAT_XYZ Gyro;
	S_FLOAT_XYZ Gyro_deg;

	
}_sensor_st;//__attribute__((packed)) 



extern _sensor_st sensor;
extern _imu_st imu_data;

//姿态处理部分
void IMU_update(float dT,S_FLOAT_XYZ *gyr, S_FLOAT_XYZ *acc,_imu_st *imu);

#endif
