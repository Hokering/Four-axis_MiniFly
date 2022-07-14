#ifndef _MY_SYSTEM_H_
#define _MY_SYSTEM_H_


#include "headfile.h"
#include "common.h"

typedef struct {
	
	struct{
		float Hard;
		float Soft;
	}Version;
	
	struct{
		float Start;
		float Now;
	}Power;
	
	struct{
		uint8_t Unlock;
		
		
	}Flag;
	
	struct{
		S_INT16_XYZ ACC_RAW;
		S_FLOAT_XYZ ACC_FILTE;
		S_INT16_XYZ GYRO_RAW;
		S_INT16_XYZ GYRO_RAW_Offset;
		S_FLOAT_XYZ OUT;
		
	}imu_data;
	
	

}_System;






extern _System System;
extern TIM_HandleTypeDef htim2;
extern ADC_HandleTypeDef hadc1;
extern float POWER;


void System_init(void );
void Get_Power(void);

#endif
