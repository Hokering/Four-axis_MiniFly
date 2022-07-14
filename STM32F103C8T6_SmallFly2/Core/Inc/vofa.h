/*
 * @Description: ���ؼ���λ��Э�鷢��ͷ�ļ�
 * @Author: EnmaAi
 * @Github: 
 * @Date: 2020-4-19 
 * @LastEditors: EnmaAi
 * @LastEditTime: 2020-4-19 
 */
#ifndef _VOLKA_H_
#define _VOLKA_H_
#include "headfile.h"



//����ṹ��,���ڷ���float����
//https://blog.csdn.net/ls667/article/details/50811519����
typedef union     
{
	float fdata;
	unsigned long ldata;
}FloatLongType;

extern UART_HandleTypeDef huart1;

void SendVofa_Float(float f);
void Byte_to_Float(float *f,uint8 byte[]);
void Send_to_Vofa(float *vofa,uint8 len);
void SendUart_to_Vofa(float *vofa,uint8 len);

#endif
