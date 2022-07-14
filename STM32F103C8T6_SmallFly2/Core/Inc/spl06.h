#ifndef _SPL06_H_
#define _SPL06_H_


#include "common.h"
#include "headfile.h"

#define SPL06_CS(x)	HAL_GPIO_WritePin(CS_SP_GPIO_Port,CS_SP_Pin,x);





#define CONTINUOUS_PRESSURE     1
#define CONTINUOUS_TEMPERATURE  2
#define CONTINUOUS_P_AND_T      3
#define PRESSURE_SENSOR     0
#define TEMPERATURE_SENSOR  1

struct spl0601_calib_param_t {	
    int16 c0;
    int16 c1;
    int32 c00;
    int32 c10;
    int16 c01;
    int16 c11;
    int16 c20;
    int16 c21;
    int16 c30;       
};




struct spl0601_t {	
    struct spl0601_calib_param_t calib_param;/**<calibration data*/	
    uint8 			chip_id; /**<chip id*/	
    int32 	i32rawPressure;
    int32 	i32rawTemperature;
    int32 	i32kP;    
    int32 	i32kT;
};

extern float alt_high;
void spl_filter(void);
uint8 Spl_init(void);
void Spl_Get_Pressure_Data ( void );
void Spl_Get_Temp_Data( void );
float Spl_Get_Pressure(void);
float Spl_Get_Temp ( void );
float Spl_Get( void );

#endif
