#ifndef _TASK_H_
#define _TASK_H_

#include "headfile.h"

typedef struct
{
	void(*task_func)(uint8_t);
	uint32_t interval_ticks;
	uint32_t last_run;
}sched_task_t;



extern int16 	
		M1_PWM,
		M2_PWM,
		M3_PWM,
		M4_PWM;

extern float vofa_data[10];
extern uint8 Run_flag;

uint8 task_running(void);

void Loop_Task_1(uint8_t take_time);
void Loop_Task_2(uint8_t take_time);
void Loop_Task_3(uint8_t take_time);
void Loop_Task_4(uint8_t take_time);
void Loop_Task_5(uint8_t take_time);
void Led_Task(uint8_t take_time);

#endif
