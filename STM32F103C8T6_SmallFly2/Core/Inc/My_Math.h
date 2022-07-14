#ifndef _MY_MATH_H
#define _MY_MATH_H

#include "headfile.h"
#include "My_Icm.h"

#define TAN_MAP_RES    		0.003921569f     /* (smallest non-zero value in table) */
#define RAD_PER_DEG   	  0.017453293f
#define DEG_PER_RAD   	  57.29577951f
#define TAN_MAP_SIZE  	  256
#define MY_PPPIII 		  	3.14159f
#define MY_PPPIII_HALF  	1.570796f

#define PI									3.1415f
#define RADIAN_TO_ANGLE    	57.29f   	//	180/PI
#define	ANGLE_TO_RADIAN			0.017453f 	//	PI/180
#define BodyRadius   				8.25f

#define LIMIT( x,min,max )    ( ((x) <= (min)) ? (min) : ( ((x) > (max))? (max) : (x) ) )

#define REVERSE(x)	(x = 100 - x)
#define ABS(x) ( (x)>0?(x):-(x) )
#define _MIN(a, b) ((a) < (b) ? (a) : (b))
#define _MAX(a, b) ((a) > (b) ? (a) : (b))
#define _MMax(value, value2, value3)  (value > value2) ? ((value > value3) ? value : value3) : (value2 > value3 ? value2 : value3)


#define my_pow(a) ((a)*(a))


void  QuiteSort(float* a,int low,int high);
float FindPos(float*a,int low,int high);
float my_abs(float f);
float fast_atan2(float y, float x);
float my_sqrt(float number);



#endif
