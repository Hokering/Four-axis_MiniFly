#ifndef _CONTROL_H_
#define _CONTROL_H_

#include "headfile.h"





typedef struct 
{
	S_FLOAT_XYZ Gyro;
	float rol;
	float pit;
	float yaw; 
}_Rcoordinate;//转换后用于PID运算

typedef struct 
{
	float rol;
	float pit;
	float yaw; 
	float alt;
}_Target;//转换后用于PID运算

extern _Rcoordinate RealCoordinate;
extern _Target Targetangle;

void ToTargetangle(void);
void Sensor_Real_Coordinate(void);
void FlyUnlock(void);

void Moto_Control(int16 M1,int16 M2,int16 M3,int16 M4);


#endif
