#ifndef _CONTROL_H_
#define _CONTROL_H_

#include "headfile.h"





typedef struct 
{
	S_FLOAT_XYZ Gyro;
	float rol;
	float pit;
	float yaw; 
}_Rcoordinate;//ת��������PID����

typedef struct 
{
	float rol;
	float pit;
	float yaw; 
	float alt;
}_Target;//ת��������PID����

extern _Rcoordinate RealCoordinate;
extern _Target Targetangle;

void ToTargetangle(void);
void Sensor_Real_Coordinate(void);
void FlyUnlock(void);

void Moto_Control(int16 M1,int16 M2,int16 M3,int16 M4);


#endif
