#include "filter.h"
#include <math.h>
#include "common.h"


//-----Butterworth变量-----//
Butter_Parameter Butter_80HZ_Parameter_Acce,Butter_60HZ_Parameter_Acce,Butter_51HZ_Parameter_Acce,
                 Butter_30HZ_Parameter_Acce,Butter_20HZ_Parameter_Acce,Butter_15HZ_Parameter_Acce,
                 Butter_10HZ_Parameter_Acce,Butter_5HZ_Parameter_Acce,Butter_2HZ_Parameter_Acce;
Butter_BufferData Butter_Buffer[3];
Butter_BufferData Butter_Buffer_Feedback[3];

/****************************************
     Butterworth低通滤波器参数初始化：http://blog.csdn.net/u011992534/article/details/73743955
***************************************/
void Butterworth_Parameter_Init(void)
{
    set_cutoff_frequency(200, 80,&Butter_80HZ_Parameter_Acce);
    set_cutoff_frequency(200, 60,&Butter_60HZ_Parameter_Acce);
    set_cutoff_frequency(200, 51,&Butter_51HZ_Parameter_Acce);
    set_cutoff_frequency(200, 30,&Butter_30HZ_Parameter_Acce);
    set_cutoff_frequency(200, 20,&Butter_20HZ_Parameter_Acce);
    set_cutoff_frequency(200, 15,&Butter_15HZ_Parameter_Acce);
    set_cutoff_frequency(200, 10,&Butter_10HZ_Parameter_Acce);
    set_cutoff_frequency(200, 5,&Butter_5HZ_Parameter_Acce);
    set_cutoff_frequency(200, 2,&Butter_2HZ_Parameter_Acce);
}

/*************************************************
函数名:	float LPButterworth(float curr_input,Butter_BufferData *Buffer,Butter_Parameter *Parameter)
说明:	加速度计低通滤波器
入口:	float curr_input 当前输入加速度计,滤波器参数，滤波器缓存
出口:	无
备注:	2阶Butterworth低通滤波器
*************************************************/
float LPButterworth(float curr_input,Butter_BufferData *Buffer,Butter_Parameter *Parameter)
{
    /* 加速度计Butterworth滤波 */
    /* 获取最新x(n) */
    static int LPF_Cnt=0;
    Buffer->Input_Butter[2]=curr_input;
    if(LPF_Cnt>=100)
    {
        /* Butterworth滤波 */
        Buffer->Output_Butter[2]=
            Parameter->b[0] * Buffer->Input_Butter[2]
            +Parameter->b[1] * Buffer->Input_Butter[1]
            +Parameter->b[2] * Buffer->Input_Butter[0]
            -Parameter->a[1] * Buffer->Output_Butter[1]
            -Parameter->a[2] * Buffer->Output_Butter[0];
    }
    else
    {
        Buffer->Output_Butter[2]=Buffer->Input_Butter[2];
        LPF_Cnt++;
    }
    /* x(n) 序列保存 */
    Buffer->Input_Butter[0]=Buffer->Input_Butter[1];
    Buffer->Input_Butter[1]=Buffer->Input_Butter[2];
    /* y(n) 序列保存 */
    Buffer->Output_Butter[0]=Buffer->Output_Butter[1];
    Buffer->Output_Butter[1]=Buffer->Output_Butter[2];
    return Buffer->Output_Butter[2];
}

// discrete low pass filter, cuts out the
// high frequency noise that can drive the controller crazy
//derivative = _last_derivative + _d_lpf_alpha * (derivative - _last_derivative);
float set_lpf_alpha(int16_t cutoff_frequency, float time_step)
{
    // calculate alpha
    float lpf_alpha;
    float rc = 1/(2*3.14*cutoff_frequency);
    lpf_alpha = time_step / (time_step + rc);
    return lpf_alpha;
}

/************************************************************************/
void set_cutoff_frequency(float sample_frequent, float cutoff_frequent,Butter_Parameter *LPF)
{
    float fr = sample_frequent / cutoff_frequent;
    float ohm = tanf(M_PI_F / fr);
    float c = 1.0f + 2.0f * cosf(M_PI_F / 4.0f) * ohm + ohm * ohm;
    if (cutoff_frequent <= 0.0f) {
        // no filtering
        return;
    }
    LPF->b[0] = ohm * ohm / c;
    LPF->b[1] = 2.0f * LPF->b[0];
    LPF->b[2] = LPF->b[0];
    LPF->a[0]=1.0f;
    LPF->a[1] = 2.0f * (ohm * ohm - 1.0f) / c;
    LPF->a[2] = (1.0f - 2.0f * cosf(M_PI_F / 4.0f) * ohm + ohm * ohm) / c;
}

void steepest_descend(int32 arr[],uint8 len,_steepest_st *steepest,uint8 step_num,int32 in)
{
    uint8 updw = 1;//0 dw,1up
    s16 i;
    uint8 step_cnt=0;
    uint8 step_slope_factor=1;
    uint8 on = 1;
    s8 pn = 1;
    //float last = 0;
    float step = 0;
    int32 start_point = 0;
    int32 pow_sum = 0;

    steepest->lst_out = steepest->now_out;

    if( ++(steepest->cnt) >= len )
    {
        (steepest->cnt) = 0; //now
    }

    //last = arr[ (steepest->cnt) ];

    arr[ (steepest->cnt) ] = in;

    step = (float)(in - steepest->lst_out)/step_num ;//梯度

    if(ABS(step)<1)//整形数据<1的有效判定
    {
        if(ABS(step)*step_num<2)
        {
            step = 0;
        }
        else
        {
            step = (step > 0) ? 1 : -1;
        }
    }

    start_point = steepest->lst_out;
    do
    {
        //start_point = steepest->lst_out;
        for(i=0; i<len; i++)
        {
// 			j = steepest->cnt + i + 1;
// 			if( j >= len )
// 			{
// 				j = j - len; //顺序排列
// 			}
            pow_sum += my_pow(arr[i] - start_point );// /step_num;//除法减小比例**

            //start_point += pn *(step_slope_factor *step/len);
        }

        if(pow_sum - steepest->lst_pow_sum > 0)
        {
            if(updw==0)
            {
                on = 0;
            }
            updw = 1;//上升了
            pn = (pn == 1 )? -1:1;

        }
        else
        {
            updw = 0; //正在下降
            if(step_slope_factor<step_num)
            {
                step_slope_factor++;
            }
        }

        steepest->lst_pow_sum = pow_sum;
        pow_sum = 0;
        start_point += pn *step;//调整

        if(++step_cnt > step_num)//限制计算次数
        {
            on = 0;
        }
        //////
        if(step_slope_factor>=2)//限制下降次数1次，节省时间，但会增大滞后，若cpu时间充裕可不用。
        {
            on = 0;

        }
        //////

    }
    while(on==1);

    steepest->now_out = start_point ;//0.5f *(start_point + steepest->lst_out);//

    steepest->now_velocity_xdt = steepest->now_out - steepest->lst_out;
}
void Moving_Average(float moavarray[],u16 len,u16 *fil_cnt,float in,float *out)
{
    u16 width_num;
    float last;

    width_num = len ;

    if( ++*fil_cnt >= width_num )
    {
        *fil_cnt = 0; //now
    }

    last = moavarray[ *fil_cnt ];

    moavarray[ *fil_cnt ] = in;

    *out += ( in - ( last  ) )/(float)( width_num ) ;
    //*out += 0.00001f *(in - *out);
    *out += 0.00001f *LIMIT((in - *out),-1,1);  //数据精度误差修正

}
void LPF_1(float hz,float time,float in,float *out)
{
    *out += ( 1 / ( 1 + 1 / ( hz *6.28f *time ) ) ) *( in - *out );

}

void limit_filter(float T,float hz,_lf_t *data,float in)
{
    float abs_t;
    LPF_1(hz,T,	 in,&(data->lpf_1));
    abs_t = ABS(data->lpf_1);
    data->out = LIMIT(in,-abs_t,abs_t);

}
void fix_inte_filter(float dT,_fix_inte_filter_st *data)
{

    data->out += (data->in_est_d + data->e ) *dT;

    data->e = data->fix_kp *(data->in_obs - data->out);

    if(data->e_limit>0)
    {
        data->e = LIMIT(data->e,-data->e_limit,data->e_limit);
    }
}




/********************************************************************************
* 函  数 ：void  SortAver_FilterXYZ(INT16_XYZ *acc,FLOAT_XYZ *Acc_filt,uint8_t n)
* 功  能 ：去最值平均值滤波三组数据
* 参  数 ：*acc 要滤波数据地址
*          *Acc_filt 滤波后数据地址
* 返回值 ：无
* 备  注 : 无
********************************************************************************/
void  SortAver_FilterXYZ(S_INT16_XYZ *acc,S_FLOAT_XYZ *Acc_filt,uint8 n)
{
    static float bufx[20],bufy[20],bufz[20];
    static uint8_t cnt =0,flag = 1;
    float temp1=0,temp2=0,temp3=0;
    uint8_t i;
    bufx[cnt] = acc->X;
    bufy[cnt] = acc->Y;
    bufz[cnt] = acc->Z;
    cnt++;      //这个的位置必须在赋值语句后，否则bufx[0]不会被赋值
    if(cnt<n && flag)
        return;   //数组填不满不计算
    else
        flag = 0;

    QuiteSort(bufx,0,n-1);
    QuiteSort(bufy,0,n-1);
    QuiteSort(bufz,0,n-1);
    for(i=1; i<n-1; i++)
    {
        temp1 += bufx[i];
        temp2 += bufy[i];
        temp3 += bufz[i];
    }

    if(cnt>=n) cnt = 0;
    Acc_filt->X  = temp1/(n-2);
    Acc_filt->Y  = temp2/(n-2);
    Acc_filt->Z  = temp3/(n-2);
}




S_FLOAT_XYZ MPU_ACC_FILT;
_sensor_st sensor;

void Data_steepest(void)
{

	//对加速度原始数据进行去极值滑动窗口滤波
	SortAver_FilterXYZ(&System.imu_data.ACC_RAW,&System.imu_data.ACC_FILTE,12);

	//二阶BUTTER WORTH	滤波器   传入加速度 得到滤波后的加速度
	System.imu_data.ACC_FILTE.X = LPButterworth(System.imu_data.ACC_RAW.X,&Butter_Buffer[0],&Butter_60HZ_Parameter_Acce);
	System.imu_data.ACC_FILTE.Y = LPButterworth(System.imu_data.ACC_RAW.Y,&Butter_Buffer[1],&Butter_60HZ_Parameter_Acce);
	System.imu_data.ACC_FILTE.Z = LPButterworth(System.imu_data.ACC_RAW.Z,&Butter_Buffer[2],&Butter_60HZ_Parameter_Acce);

	//转换真实值  2000量程   2000/65535*2
	sensor.Gyro_deg.X  =  System.imu_data.GYRO_RAW.X *0.061036f;
	sensor.Gyro_deg.Y  =  System.imu_data.GYRO_RAW.Y *0.061036f;
	sensor.Gyro_deg.Z  =  System.imu_data.GYRO_RAW.Z *0.061036f;
  
  // 转换单位 m/s2
	sensor.Acc_mmss.X  =  System.imu_data.ACC_FILTE.X *2.3926f ;
	sensor.Acc_mmss.Y  =  System.imu_data.ACC_FILTE.Y *2.3926f;
	sensor.Acc_mmss.Z  =  System.imu_data.ACC_FILTE.Z *2.3926f ;

}

