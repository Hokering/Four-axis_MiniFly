#include "spl06.h"

static struct spl0601_t spl0601;
static struct spl0601_t *p_spl0601;

static void Spl_Wtire ( uint8 cmd, uint8 val )
{
    uint8 dat[2];
    SPL06_CS(0);
    dat[0] = cmd ;
    dat[1] = val;
    HAL_SPI_TransmitReceive(&hspi1,dat,dat,2,100);
    SPL06_CS(1);
}

static uint8 Spl_Read (uint8 cmd)
{
    uint8 dat[2];
    SPL06_CS(0);
    dat[0] = cmd |0x80 ;
    dat[1] = 0xff;
    HAL_SPI_TransmitReceive(&hspi1,dat,dat,2,100);
    SPL06_CS(1);
    return dat[1];
}


/********************************************************************
		设置温度传感器的每秒采样次数以及过采样率
		输入参数  : uint8 u8OverSmpl  过采样率         Maximal = 128
					uint8 u8SmplRate  每秒采样次数(Hz) Maximal = 128
					uint8 iSensor     0: Pressure; 1: Temperature
********************************************************************/
void Spl_Rateset ( uint8 iSensor, uint8 u8SmplRate, uint8 u8OverSmpl )
{
    uint8 reg = 0;
    int32_t i32kPkT = 0;
    switch ( u8SmplRate )
    {
    case 2:
        reg |= ( 1 << 4 );
        break;
    case 4:
        reg |= ( 2 << 4 );
        break;
    case 8:
        reg |= ( 3 << 4 );
        break;
    case 16:
        reg |= ( 4 << 4 );
        break;
    case 32:
        reg |= ( 5 << 4 );
        break;
    case 64:
        reg |= ( 6 << 4 );
        break;
    case 128:
        reg |= ( 7 << 4 );
        break;
    case 1:
    default:
        break;
    }
    switch ( u8OverSmpl )
    {
    case 2:
        reg |= 1;
        i32kPkT = 1572864;
        break;
    case 4:
        reg |= 2;
        i32kPkT = 3670016;
        break;
    case 8:
        reg |= 3;
        i32kPkT = 7864320;
        break;
    case 16:
        i32kPkT = 253952;
        reg |= 4;
        break;
    case 32:
        i32kPkT = 516096;
        reg |= 5;
        break;
    case 64:
        i32kPkT = 1040384;
        reg |= 6;
        break;
    case 128:
        i32kPkT = 2088960;
        reg |= 7;
        break;
    case 1:
    default:
        i32kPkT = 524288;
        break;
    }

    if ( iSensor == 0 )
    {
        p_spl0601->i32kP = i32kPkT;
        Spl_Wtire ( 0x06, reg );
        if ( u8OverSmpl > 8 )
        {
            reg = Spl_Read ( 0x09 );
            Spl_Wtire ( 0x09, reg | 0x04 );
        }
    }
    if ( iSensor == 1 )
    {
        p_spl0601->i32kT = i32kPkT;
        Spl_Wtire ( 0x07, reg | 0x80 ); //Using mems temperature
        if ( u8OverSmpl > 8 )
        {
            reg = Spl_Read ( 0x09 );
            Spl_Wtire ( 0x09, reg | 0x08 );
        }
    }

}

void Spl_Start_Temp( void )
{
    Spl_Wtire ( 0x08, 0x02 );
}
void Spl_Start_Pressure( void )
{
    Spl_Wtire ( 0x08, 0x01 );
}
void Spl_Start_Continuous (uint8 mode )
{
    //mode  1: pressure; 2: temperature; 3: pressure and temperature
    Spl_Wtire ( 0x08, mode + 4 );
}


//开机校准气压计
void Spl_Get_Calib_Param ( void )
{
    uint32 h;
    uint32 m;
    uint32 l;
    h =  Spl_Read ( 0x10 );
    l  =  Spl_Read ( 0x11 );
    p_spl0601->calib_param.c0 = ( int16_t ) h << 4 | l >> 4;
    p_spl0601->calib_param.c0 = ( p_spl0601->calib_param.c0 & 0x0800 ) ? ( 0xF000 | p_spl0601->calib_param.c0 ) : p_spl0601->calib_param.c0;
    h =  Spl_Read ( 0x11 );
    l  =  Spl_Read ( 0x12 );
    p_spl0601->calib_param.c1 = ( int16_t ) ( h & 0x0F ) << 8 | l;
    p_spl0601->calib_param.c1 = ( p_spl0601->calib_param.c1 & 0x0800 ) ? ( 0xF000 | p_spl0601->calib_param.c1 ) : p_spl0601->calib_param.c1;
    h =  Spl_Read ( 0x13 );
    m =  Spl_Read ( 0x14 );
    l =  Spl_Read ( 0x15 );
    p_spl0601->calib_param.c00 = ( int32_t ) h << 12 | ( int32_t ) m << 4 | ( int32_t ) l >> 4;
    p_spl0601->calib_param.c00 = ( p_spl0601->calib_param.c00 & 0x080000 ) ? ( 0xFFF00000 | p_spl0601->calib_param.c00 ) : p_spl0601->calib_param.c00;
    h =  Spl_Read ( 0x15 );
    m =  Spl_Read ( 0x16 );
    l =  Spl_Read ( 0x17 );
    p_spl0601->calib_param.c10 = ( int32_t ) h << 16 | ( int32_t ) m << 8 | l;
    p_spl0601->calib_param.c10 = ( p_spl0601->calib_param.c10 & 0x080000 ) ? ( 0xFFF00000 | p_spl0601->calib_param.c10 ) : p_spl0601->calib_param.c10;
    h =  Spl_Read ( 0x18 );
    l  =  Spl_Read ( 0x19 );
    p_spl0601->calib_param.c01 = ( int16_t ) h << 8 | l;
    h =  Spl_Read ( 0x1A );
    l  =  Spl_Read ( 0x1B );
    p_spl0601->calib_param.c11 = ( int16_t ) h << 8 | l;
    h =  Spl_Read ( 0x1C );
    l  =  Spl_Read ( 0x1D );
    p_spl0601->calib_param.c20 = ( int16_t ) h << 8 | l;
    h =  Spl_Read ( 0x1E );
    l  =  Spl_Read ( 0x1F );
    p_spl0601->calib_param.c21 = ( int16_t ) h << 8 | l;
    h =  Spl_Read ( 0x20 );
    l  =  Spl_Read ( 0x21 );
    p_spl0601->calib_param.c30 = ( int16_t ) h << 8 | l;
}



//获取温度原始值
void Spl_Get_Temp_Data( void )
{
    uint8 h[3] = {0};

    h[0] = Spl_Read ( 0x03 );
    h[1] = Spl_Read ( 0x04 );
    h[2] = Spl_Read ( 0x05 );

    p_spl0601->i32rawTemperature = ( int32_t ) h[0] << 16 | ( int32_t ) h[1] << 8 | ( int32_t ) h[2];
    p_spl0601->i32rawTemperature = ( p_spl0601->i32rawTemperature & 0x800000 ) ? ( 0xFF000000 | p_spl0601->i32rawTemperature ) : p_spl0601->i32rawTemperature;
}

//获取气压原始值
void Spl_Get_Pressure_Data ( void )
{
    uint8 h[3];

    h[0] = Spl_Read ( 0x00 );
    h[1] = Spl_Read ( 0x01 );
    h[2] = Spl_Read ( 0x02 );

    p_spl0601->i32rawPressure = ( int32_t ) h[0] << 16 | ( int32_t ) h[1] << 8 | ( int32_t ) h[2];
    p_spl0601->i32rawPressure = ( p_spl0601->i32rawPressure & 0x800000 ) ? ( 0xFF000000 | p_spl0601->i32rawPressure ) : p_spl0601->i32rawPressure;
}
//SPL06初始化
uint8 Spl_init(void)
{
    p_spl0601 = &spl0601; /* read Chip Id */
    p_spl0601->i32rawPressure = 0;
    p_spl0601->i32rawTemperature = 0;
    p_spl0601->chip_id = Spl_Read ( 0x0D );// 0x34  0x10

    Spl_Get_Calib_Param();

    Spl_Rateset ( PRESSURE_SENSOR, 128, 16 );

    Spl_Rateset ( TEMPERATURE_SENSOR, 8, 8 );

    Spl_Start_Continuous ( CONTINUOUS_P_AND_T );

    if(p_spl0601->chip_id == 0x10)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}


//获取校准后压力值
float Spl_Get_Temp ( void )
{
    float fTCompensate;
    float fTsc;

    fTsc = p_spl0601->i32rawTemperature / ( float ) p_spl0601->i32kT;
    fTCompensate =  p_spl0601->calib_param.c0 * 0.5 + p_spl0601->calib_param.c1 * fTsc;
    return fTCompensate;
}

//获取校准后压力值
float Spl_Get_Pressure(void)
{
    float fTsc, fPsc;
    float qua2, qua3;
    float fPCompensate;

    fTsc = p_spl0601->i32rawTemperature / ( float ) p_spl0601->i32kT;
    fPsc = p_spl0601->i32rawPressure / ( float ) p_spl0601->i32kP;
    qua2 = p_spl0601->calib_param.c10 + fPsc * ( p_spl0601->calib_param.c20 + fPsc * p_spl0601->calib_param.c30 );
    qua3 = fTsc * fPsc * ( p_spl0601->calib_param.c11 + fPsc * p_spl0601->calib_param.c21 );
    //qua3 = 0.9f *fTsc * fPsc * (p_spl0601->calib_param.c11 + fPsc * p_spl0601->calib_param.c21);

    fPCompensate = p_spl0601->calib_param.c00 + fPsc * qua2 + fTsc * p_spl0601->calib_param.c01 + qua3;
    //fPCompensate = p_spl0601->calib_param.c00 + fPsc * qua2 + 0.9f *fTsc  * p_spl0601->calib_param.c01 + qua3;
    return fPCompensate;
}


float baro_Offset=16300, alt_3, height;
unsigned char baro_start;
float temperature, alt_high,alt_high_last=0;
float baro_pressure;

float Spl_Get( void )
{


    Spl_Get_Temp_Data();
    temperature = Spl_Get_Temp();

    Spl_Get_Pressure_Data();
    baro_pressure = Spl_Get_Pressure();

    //alt_high = powf((temp/101325),1/5.255f);

    alt_3 = ( 101400 - baro_pressure ) / 1000.0f;
    height = 0.82f * alt_3 * alt_3 * alt_3 + 0.09f * ( 101400 - baro_pressure ) * 100.0f ;


    alt_high = ( height - baro_Offset ); //cm +

//    if(alt_high > 10000)
//        baro_Offset = alt_high;

    return alt_high;
}

float SPL_Data[3];
void spl_filter(void)
{
    SPL_Data[2]=SPL_Data[1];
    SPL_Data[1]=SPL_Data[0];
    SPL_Data[0]=alt_high;
    alt_high=_MMax(SPL_Data[0],SPL_Data[1],SPL_Data[2]);

    alt_high = alt_high_last * 0.6f + alt_high * 0.4f;
    alt_high_last =alt_high;

}