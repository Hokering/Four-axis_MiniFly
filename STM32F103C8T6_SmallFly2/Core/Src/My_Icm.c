#include "My_Icm.h"


_imu_st imu_data =  {1,0,0,0,
    {0,0,0},
    {0,0,0},
    {0,0,0},
    {0,0,0},
    {0,0,0},
    0,0,0
};

_lf_t err_lf_x;
_lf_t err_lf_y;
_lf_t err_lf_z;

S_FLOAT_XYZ vec_err_i;

// 四元数处理
void IMU_update(float dT,S_FLOAT_XYZ *gyr, S_FLOAT_XYZ *acc,_imu_st *imu)
{
    float kp = 0.8,ki = 0;


    float q0q1,q0q2,q1q1,q1q3,q2q2,q2q3,q3q3,q1q2,q0q3;//q0q0,
    float w_q,x_q,y_q,z_q;
    float acc_length,q_length;
    S_FLOAT_XYZ acc_norm;
    S_FLOAT_XYZ vec_err;
    S_FLOAT_XYZ d_angle;


    if(acc->X * acc->Y *acc->Z ==0)
        return ;


    w_q = imu->w;
    x_q = imu->x;
    y_q = imu->y;
    z_q = imu->z;

//	q0q0 = w_q * w_q;
    q0q1 = w_q * x_q;
    q0q2 = w_q * y_q;
    q1q1 = x_q * x_q;
    q1q3 = x_q * z_q;
    q2q2 = y_q * y_q;
    q2q3 = y_q * z_q;
    q3q3 = z_q * z_q;
    q1q2 = x_q * y_q;
    q0q3 = w_q * z_q;


    // 加速度计的读数，单位化。
    acc_length = my_sqrt(my_pow(acc->X) + my_pow(acc->Y) + my_pow(acc->Z));
    acc_norm.X = acc->X / acc_length;
    acc_norm.Y = acc->Y / acc_length;
    acc_norm.Z = acc->Z / acc_length;


    // 载体坐标下的x方向向量，单位化。
    imu->x_vec.X = 1 - (2*q2q2 + 2*q3q3);
    imu->x_vec.Y = 2*q1q2 - 2*q0q3;
    imu->x_vec.Z = 2*q1q3 + 2*q0q2;

    // 载体坐标下的y方向向量，单位化。
    imu->y_vec.X = 2*q1q2 + 2*q0q3;
    imu->y_vec.Y = 1 - (2*q1q1 + 2*q3q3);
    imu->y_vec.Z = 2*q2q3 - 2*q0q1;

    // 载体坐标下的z方向向量（等效重力向量、重力加速度向量），单位化。
    imu->z_vec.X = 2*q1q3 - 2*q0q2;
    imu->z_vec.Y = 2*q2q3 + 2*q0q1;
    imu->z_vec.Z = 1 - (2*q1q1 + 2*q2q2);

    // 测量值与等效重力向量的叉积（计算向量误差）。
    vec_err.X =  (acc_norm.Y * imu->z_vec.Z - imu->z_vec.Y * acc_norm.Z);
    vec_err.Y = -(acc_norm.X * imu->z_vec.Z - imu->z_vec.X * acc_norm.Z);
    vec_err.Z = -(acc_norm.Y * imu->z_vec.X - imu->z_vec.Y * acc_norm.X);

    //低通限幅滤波
    limit_filter(dT,0.5f,&err_lf_x,vec_err.X);
    limit_filter(dT,0.5f,&err_lf_y,vec_err.Y);
    limit_filter(dT,0.5f,&err_lf_z,vec_err.Z);

    //误差积分
    vec_err_i.X += err_lf_x.out *dT *ki;
    vec_err_i.Y += err_lf_y.out *dT *ki;
    vec_err_i.Z += err_lf_z.out *dT *ki;

    // 构造增量旋转（含融合纠正）。
    d_angle.X = (gyr->X *RAD_PER_DEG + (err_lf_x.out + vec_err_i.X) * kp) * dT / 2 ;
    d_angle.Y = (gyr->Y *RAD_PER_DEG + (err_lf_y.out + vec_err_i.Y) * kp) * dT / 2 ;
    d_angle.Z = (gyr->Z *RAD_PER_DEG + (err_lf_z.out + vec_err_i.Z) * kp) * dT / 2 ;

    // 计算姿态。
    imu->w = w_q           - x_q*d_angle.X - y_q*d_angle.Y - z_q*d_angle.Z;
    imu->x = w_q*d_angle.X + x_q           + y_q*d_angle.Z - z_q*d_angle.Y;
    imu->y = w_q*d_angle.Y - x_q*d_angle.Z + y_q           + z_q*d_angle.X;
    imu->z = w_q*d_angle.Z + x_q*d_angle.Y - y_q*d_angle.X + z_q;

    q_length = my_sqrt(imu->w*imu->w + imu->x*imu->x + imu->y*imu->y + imu->z*imu->z);
    imu->w /= q_length;
    imu->x /= q_length;
    imu->y /= q_length;
    imu->z /= q_length;

    imu->yaw = asin(2*q1q3 - 2*q0q2)*57.30f;                                   //实际的y
    imu->pit = -fast_atan2(2*q2q3 + 2*q0q1, -2*q1q1-2*q2q2 + 1)*57.30f;        //实际的X
    imu->rol = -fast_atan2(2*q1q2 + 2*q0q3, -2*q2q2-2*q3q3 + 1)*57.30f;				 //实际的Z
		Sensor_Real_Coordinate();
}
