#include "pid.h"

//�ǶȻ�PID
PID rol_angle,
    pit_angle,
    yaw_angle;

float Pit_PID[4] = {	5,	0,	0,	0};
float Yaw_PID[4] = {	5,	0,	0,	0};
float Rol_PID[4] = {	0.1,	0,	0,	0}; 

//���ٶ��ڻ�PID
PID rol_rate,
    pit_rate,
    yaw_rate;  


float Pit_Rate_PID[4] = {	2.2,	0.00,	12.0,	100};
float Yaw_Rate_PID[4] = {	2.2,	0.00,	12.0,	100};
float Rol_Rate_PID[4] = {	0.1,	0.00,	12.0,	100};

//�߶Ȼ�PID
PID alt_rate,
    alt;

float alt_rate_PID[4] =  {0,0,0,0};
float alt_PID[4]      =  {0,0,0,0};



// PID������ʼ��
void PID_Parameter_Init(PID *sptr)
{
    sptr->SumError  = 0;
    sptr->LastError = 0;	//Error[-1]
    sptr->PrevError = 0;	//Error[-2]
    sptr->LastData  = 0;
    sptr->Output    = 0;
}


// λ��ʽPID����
void PID_Realize(PID *sptr, float *PID, float NowData, float Point)
{
    //��ǰ������Ϊ�Ĵ���������ֻ���������ͺ��ַ��ͱ�������������ٶ�
    float iError,	// ��ǰ���
          Realize;	// ���ó���ʵ������

    iError = Point - NowData;	// ���㵱ǰ���
    sptr->SumError += PID[KI] * iError;	// ������
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
    
    sptr->PrevError = sptr->LastError;	// ����ǰ�����
    sptr->LastError = iError;		  	// �����ϴ����
    sptr->LastData  = NowData;			// �����ϴ�����

    sptr->Output = Realize;				//���PID
}
