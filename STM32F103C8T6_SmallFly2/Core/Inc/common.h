#ifndef _common_h
#define _common_h

#define u8 	uint8
#define s8  int8
#define s16 int16
#define u16 uint16

//数据类型声明
typedef unsigned char		uint8;					//  8 bits 
typedef unsigned short int	uint16;					// 16 bits 
typedef unsigned long int	uint32;					// 32 bits 
typedef unsigned long long	uint64;					// 64 bits 

typedef signed char				int8;					//  8 bits 
typedef short int			int16;					// 16 bits 
typedef long  int			int32;					// 32 bits 
typedef long  long			int64;					// 64 bits 

typedef volatile int8		vint8;					//  8 bits 
typedef volatile int16		vint16;					// 16 bits 
typedef volatile int32		vint32;					// 32 bits 
typedef volatile int64		vint64;					// 64 bits 

typedef volatile uint8		vuint8;					//  8 bits 
typedef volatile uint16		vuint16;				// 16 bits 
typedef volatile uint32		vuint32;				// 32 bits 
typedef volatile uint64		vuint64;				// 64 bits 



/*
 * 姿态结算结构体
 */
typedef struct
{
    uint16 x;
    uint16 y;
} Site_t;

typedef struct
{
	float X;
	float Y;
	float Z;
} S_FLOAT_XYZ;

typedef struct
{
	int32 X;
	int32 Y;
	int32 Z;
} S_INT32_XYZ;

typedef struct
{
	int16 X;
	int16 Y;
	int16 Z;
} S_INT16_XYZ;



#endif
