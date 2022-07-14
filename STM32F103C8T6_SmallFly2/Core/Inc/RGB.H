#ifndef _RGB_H_
#define _RGB_H_


#include "headfile.h"



void RGB_LED_Write0(void );
void RGB_LED_Write1(void );
void RGB_Reset(void);
void RGB_Write_Byte(uint8 byte);
void RGB_Run_24bit(uint8 red,uint8 green,uint8 blue);
void RGB_Cool_Run(void);
void HSVtoRGB(uint8 *r, uint8 *g, uint8 *b, uint16 h, uint16 s, uint16 v);
void RGB_Breathing(void);
void RGB_LED_FLY(void);


#endif
