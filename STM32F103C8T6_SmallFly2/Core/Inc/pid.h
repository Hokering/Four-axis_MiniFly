#ifndef _PID_H_
#define _PID_H_

#include "headfile.h"



#define KP 0  
#define KI 1
#define KD 2
#define KT 3
#define KB 4
#define KF 5



typedef struct
{
	float SumError;					//误差累计	
	float LastError;				//上次误差
	float PrevError;				//上上次误差	
	float LastData;					//上次数据
	float Output;
} PID;


extern PID rol_angle,
	pit_angle,
	yaw_angle;

extern float Rol_PID[4];
extern float Pit_PID[4];
extern float Yaw_PID[4];

//角速度环PID
extern PID rol_rate,
	pit_rate,
	yaw_rate;

extern float Rol_Rate_PID[4];
extern float Pit_Rate_PID[4];
extern float Yaw_Rate_PID[4];

//高度环PID
extern PID alt_rate,
	alt;

extern float alt_rate_PID[4];
extern float alt_PID[4];


void PID_Realize(PID *sptr, float *PID, float NowData, float Point);



#endif