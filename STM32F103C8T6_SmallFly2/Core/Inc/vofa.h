/*
 * @Description: 伏特加上位机协议发送头文件
 * @Author: EnmaAi
 * @Github: 
 * @Date: 2020-4-19 
 * @LastEditors: EnmaAi
 * @LastEditTime: 2020-4-19 
 */
#ifndef _VOLKA_H_
#define _VOLKA_H_
#include "headfile.h"



//定义结构体,用于发送float数据
//https://blog.csdn.net/ls667/article/details/50811519博客
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
