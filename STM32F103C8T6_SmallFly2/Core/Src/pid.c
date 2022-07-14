#include "pid.h"

//角度环PID
PID rol_angle,
    pit_angle,
    yaw_angle;

float Pit_PID[4] = {	5,	0,	0,	0};
float Yaw_PID[4] = {	5,	0,	0,	0};
float Rol_PID[4] = {	0.1,	0,	0,	0}; 

//角速度内环PID
PID rol_rate,
    pit_rate,
    yaw_rate;  


float Pit_Rate_PID[4] = {	2.2,	0.00,	12.0,	100};
float Yaw_Rate_PID[4] = {	2.2,	0.00,	12.0,	100};
float Rol_Rate_PID[4] = {	0.1,	0.00,	12.0,	100};

//高度环PID
PID alt_rate,
    alt;

float alt_rate_PID[4] =  {0,0,0,0};
float alt_PID[4]      =  {0,0,0,0};



// PID参数初始化
void PID_Parameter_Init(PID *sptr)
{
    sptr->SumError  = 0;
    sptr->LastError = 0;	//Error[-1]
    sptr->PrevError = 0;	//Error[-2]
    sptr->LastData  = 0;
    sptr->Output    = 0;
}


// 位置式PID控制
void PID_Realize(PID *sptr, float *PID, float NowData, float Point)
{
    //当前误差，定义为寄存器变量，只能用于整型和字符型变量，提高运算速度
    float iError,	// 当前误差
          Realize;	// 最后得出的实际增量

    iError = Point - NowData;	// 计算当前误差
    sptr->SumError += PID[KI] * iError;	// 误差积分
    if (sptr->SumError >= PID[KT])
    {
        sptr->SumError = PID[KT];
    }
    else if (sptr->SumError <= -PID[KT])
    {
        sptr->SumError = -PID[KT];
    }

    Realize = PID[KP] * iError
              + sptr->SumError
              + PID[KD] * (iError - sptr->LastError);
    
    sptr->PrevError = sptr->LastError;	// 更新前次误差
    sptr->LastError = iError;		  	// 更新上次误差
    sptr->LastData  = NowData;			// 更新上次数据

    sptr->Output = Realize;				//输出PID
}
