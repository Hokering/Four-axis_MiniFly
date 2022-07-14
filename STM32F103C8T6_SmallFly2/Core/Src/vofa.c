#include "vofa.h"

/*
��������fת��Ϊ4���ֽ����ݴ����byte[4]�У�������
*/
void SendVofa_Float(float f)
{
    FloatLongType fl;
	  uint8 byte[4];
    fl.fdata=f;
    byte[0]=(unsigned char)fl.ldata;
    byte[1]=(unsigned char)(fl.ldata>>8);
    byte[2]=(unsigned char)(fl.ldata>>16);
    byte[3]=(unsigned char)(fl.ldata>>24);
		printf("%c%c%c%c",byte[0],byte[1],byte[2],byte[3]);
}
/*
��4���ֽ�����byte[4]ת��Ϊ�����������*f��
*/
void Byte_to_Float(float *f,uint8 byte[])
{
    FloatLongType fl;
    fl.ldata=0;
    fl.ldata=byte[3];
    fl.ldata=(fl.ldata<<8)|byte[2];
    fl.ldata=(fl.ldata<<8)|byte[1];
    fl.ldata=(fl.ldata<<8)|byte[0];
    *f=fl.fdata;
}



void SendUart_to_Vofa(float *vofa,uint8 len)
{
	uint8 i;
	for(i=0;i<len;i++)
	{
		SendVofa_Float(vofa[i]);
	}
	printf("%c%c%c%c",0x00,0x00,0x80,0x7f);


}