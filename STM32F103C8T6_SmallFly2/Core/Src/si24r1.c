#include "si24r1.h"
uint8 NRF_RF_Addr[NRF_ADDR_WIDTH]= {0x00,0xFF,0x00,0xFF,0x00};  // TX,RX地址,可自行更改

uint8 NRF_RX_Buff[NRF_DATA_WIDTH];                      	// 接收数据缓冲区
uint8 NRF_TX_Buff[NRF_DATA_WIDTH];                      	// 发送数据缓冲区
struct remoto_data Remoto_data;														//	遥控器接收数据


/**************-----↓硬件SPI部分-------***************/
// 函数: NRF_SPI_WR_Byte()
// 描述: 通过SPI写一个byte到nRF24L01+,同时从nRF24L01+读取一个byte
// 参数: byte   待写入字节数据
//       return 读取到的字节数据
//		内部使用用户无需调用
uint8 NRF_SPI_WR_Byte(uint8 byte)
{
    HAL_SPI_TransmitReceive(&hspi1,&byte,&byte,1,100);
    return(byte);                                      		// 返回读出的一字节
}

// 函数: NRF_SPI_W_Cmd_Byte()
// 描述: 将命令字写入nRF24L01+,同时返回status字节
// 参数: cmd    命令字
//       return 返回status状态
//		内部使用用户无需调用
uint8 NRF_SPI_W_Cmd_Byte(uint8 cmd)
{
    uint8 status;
    NRF_CSN (0);                                    		// NRF_CSN_PIN置低,开始传输数据
    status = NRF_SPI_WR_Byte(cmd);                      	// 写入寄存器地址,同时返回状态字
    NRF_CSN (1);                                    		// NRF_CSN_PIN拉高,结束数据传输
    return(status);                                     	// 返回状态寄存器
}

// 函数: NRF_SPI_W_Reg_Byte()
// 描述: 将val写入addr对应的寄存器地址,同时返回status字节
// 参数: cmd    命令字
//       val    待写入寄存器的数值
//       return 返回status状态
//		内部使用用户无需调用
uint8 NRF_SPI_W_Reg_Byte(uint8 cmd, uint8 val)
{
    uint8 status;
    NRF_CSN (0);                                    		// NRF_CSN_PIN置低,开始传输数据
    status = NRF_SPI_WR_Byte(cmd);                      	// 写入寄存器地址,同时返回状态字
    NRF_SPI_WR_Byte(val);                               	// 然后写数据到该寄存器
    NRF_CSN (1);                                    		// NRF_CSN_PIN拉高,结束数据传输
    return(status);                                     	// 返回状态寄存器
}

// 函数: NRF_SPI_W_Reg_Byte()
// 描述: 读取addr所对应的寄存器地址
// 参数: cmd    命令字
//       return 返回寄存器内的数值
//		内部使用用户无需调用
uint8 NRF_SPI_R_Reg_Byte(uint8 cmd)
{
    uint8 val;
    NRF_CSN (0);                                    		// NRF_CSN_PIN置低,开始传输数据
    NRF_SPI_WR_Byte(cmd);                               	// 写入寄存器地址
    val = NRF_SPI_WR_Byte(0);                           	// 然后从该寄存器读数据
    NRF_CSN (1);                                    		// NRF_CSN_PIN拉高,结束数据传输
    return(val);                                        	// 返回寄存器数据
}

// 函数: NRF_SPI_W_Reg_Burst()
// 描述: 连续写入len个字节到addr指定的寄存器中,同时返回status状态字
// 参数: cmd    命令字
//       *p     待写入数据指针
//       len    待写入数据长度
//       return 返回status状态
//		内部使用用户无需调用
uint8 NRF_SPI_W_Reg_Burst(uint8 cmd, uint8 *p, uint8 len)
{
    uint8 status, i;
    NRF_CSN (0);                                    		// NRF_CSN_PIN置低,开始传输数据
    status = NRF_SPI_WR_Byte(cmd);                      	// 选择寄存器,同时返回状态字
    for(i=0; i<len; i++)
        NRF_SPI_WR_Byte(p[i]);                          	// 逐个字节写入nRF24L01+
    NRF_CSN (1);                                    		// NRF_CSN_PIN拉高,结束数据传输
    return(status);                                     	// 返回状态寄存器
}

// 函数: NRF_SPI_R_Reg_Burst()
// 描述: 连续读取len个字节到addr指定的寄存器中,同时返回status状态字
// 参数: cmd     命令字
//       *p     待读取数据指针
//       len    待读取数据长度
//       return 返回status状态
//		内部使用用户无需调用
uint8 NRF_SPI_R_Reg_Burst(uint8 cmd, uint8 *p, uint8 len)
{
    uint8 status, i;
    NRF_CSN (0);                                    		// NRF_CSN_PIN置低,开始传输数据
    status = NRF_SPI_WR_Byte(cmd);                      	// 选择寄存器,同时返回状态字
    for(i=0; i<len; i++)
        p[i] = NRF_SPI_WR_Byte(0);                      	// 逐个字节从nRF24L01+读出
    NRF_CSN (1);                                    		// NRF_CSN_PIN拉高,结束数据传输
    return(status);                                     	// 返回状态寄存器
}
/**************-----↑硬件SPI部分-------***************/


//-------------------------------------------------------------------------------------------------------------------
//  @brief      NRF检查函数
//  @param      NULL
//  @return     void
//  @since      v1.0
//  Sample usage:
//-------------------------------------------------------------------------------------------------------------------
uint8 NRF_Check(void)
{
    uint8 temp;
    NRF_SPI_W_Reg_Byte(NRF_CMD_W_REGISTER | NRF_REG_RX_PW_P0,   NRF_DATA_WIDTH);    // 设置RX通道0数据宽度
    temp = NRF_SPI_R_Reg_Byte(NRF_CMD_R_REGISTER | NRF_REG_RX_PW_P0);               // 读取RX通道0数据宽度

    return (NRF_DATA_WIDTH != temp);
    //如果程序卡死在while这里，则有以下几种情况
    //1.nrf未连接
    //2.nrf与单片机连接不可靠，大多数无法通信都是因为这个
    //3.nrf是坏的,如果NRF是新的，坏的可能性极低
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      NRF无线模块初始化
//  @param      NULL
//  @return     void
//  @since      v1.0
//  Sample usage:
//-------------------------------------------------------------------------------------------------------------------
void NRF_Dev_Init(void)
{
    HAL_Delay(100);  	//上电延时
    NRF_CE  (0);                                    		// 读/写nRF寄存器均需要将CE拉低
    NRF_CSN (1);                                    		// SPI禁止
    while(NRF_Check());

    NRF_SPI_W_Reg_Byte(NRF_CMD_W_REGISTER | NRF_REG_CONFIG,     NRF_RCD_TX_DS_DI        // 禁用发送完成中断
                       | NRF_RCD_MAX_RT_DI       // 禁用发送超时中断
                       | NRF_RCD_CRC_EN          // 启用CRC
                       | NRF_RCD_CRC16           // CRC16
                       | NRF_RCD_PWR_UP          // 上电模式
                       | NRF_RCD_PRIM_PRX);      // RX模式
    HAL_Delay(10);
    NRF_SPI_W_Reg_Byte(NRF_CMD_W_REGISTER | NRF_REG_EN_AA,      NRF_RCD_PIPE0_AA_EN);   // 使能通道0自动应答
    HAL_Delay(10);
    NRF_SPI_W_Reg_Byte(NRF_CMD_W_REGISTER | NRF_REG_EN_RXADDR,  NRF_RCD_PIPE0_RX_EN);   // 使能接收通道0
    HAL_Delay(10);
    NRF_SPI_W_Reg_Byte(NRF_CMD_W_REGISTER | NRF_REG_SETUP_AW,   NRF_RCD_AW_5byte);      // 设置RX地址宽度
    HAL_Delay(10);
    NRF_SPI_W_Reg_Byte(NRF_CMD_W_REGISTER | NRF_REG_SETUP_RETR, NRF_RCD_ARC_10|NRF_RCD_ARD_250us);        // 使能自动重发  间隔时间250US
    HAL_Delay(10);
    NRF_SPI_W_Reg_Byte(NRF_CMD_W_REGISTER | NRF_REG_RF_CH,      NRF_RF_CH);             // 设置射频通道
    HAL_Delay(10);
    NRF_SPI_W_Reg_Byte(NRF_CMD_W_REGISTER | NRF_REG_RF_SETUP,   NRF_RCD_RF_DR_2Mbps     // 设置传输速率为2Mbps
                       | NRF_RCD_RF_PWR_0dBm);   // 发射功率为0dBm
    HAL_Delay(10);
    NRF_SPI_W_Reg_Byte(NRF_CMD_W_REGISTER | NRF_REG_RX_PW_P0,   NRF_DATA_WIDTH);        // 设置RX通道0数据宽度
    HAL_Delay(10);

    NRF_SPI_W_Reg_Burst(NRF_CMD_W_REGISTER | NRF_REG_RX_ADDR_P0, NRF_RF_Addr, NRF_ADDR_WIDTH);   // 设置RX通道0地址
    HAL_Delay(10);
    NRF_SPI_W_Cmd_Byte(NRF_CMD_FLUSH_RX);                                               // 清空RXFIFO
    HAL_Delay(10);
    NRF_CE (1);                                                                     	// NRF_CE_PIN置高,进入运行模式
}


//-------------------------------------------------------------------------------------------------------------------
//  @brief      NRF无线模块数据发送函数
//  @param      *p			要发送的数据地址
//  @return     void
//  @since      v1.0
//  Sample usage:			初始化nRF24L01+,并将其转换为发送状态。发送数据缓冲区中的数据,发送完成后再转换为接收状态
//-------------------------------------------------------------------------------------------------------------------
void NRF_Send_Packet(uint8 *p)
{
    uint8 sta=0;

    NRF_CE (0);                                                                     	// NRF_CE_PIN置低,进入待机模式
    NRF_SPI_W_Reg_Byte(NRF_CMD_W_REGISTER | NRF_REG_CONFIG, NRF_RCD_RX_DR_DI            // 禁用发送完成中断
                       | NRF_RCD_MAX_RT_DI           // 禁用发送超时中断
                       | NRF_RCD_CRC_EN              // 启用CRC
                       | NRF_RCD_CRC16               // CRC16
                       | NRF_RCD_PWR_UP              // 上电模式
                       | NRF_RCD_PRIM_PTX);          // TX模式

    NRF_SPI_W_Reg_Burst(NRF_CMD_W_REGISTER | NRF_REG_TX_ADDR, NRF_RF_Addr, NRF_ADDR_WIDTH);  // 设置TX地址
    NRF_SPI_W_Reg_Burst(NRF_CMD_W_TX_PAYLOAD, p, NRF_DATA_WIDTH);                       // 写入发送数据
    NRF_CE (1);                                                                     	// NRF_CE_PIN置高,进入运行模式

    while(1)
    {
        sta = NRF_SPI_R_Reg_Byte(NRF_CMD_R_REGISTER | NRF_REG_STATUS);
        if(sta&0x10)//发送失败
        {
            NRF_SPI_W_Reg_Byte(NRF_CMD_W_REGISTER | NRF_REG_STATUS, 0x10); 				// 写回TX_DR,重置NRF_IRQ_PIN中断
            NRF_SPI_W_Reg_Byte(NRF_CMD_FLUSH_TX,0xff);
            break;
        }
        if(sta&0x20)//发送成功
        {
            NRF_SPI_W_Reg_Byte(NRF_CMD_W_REGISTER | NRF_REG_STATUS, NRF_RCD_TX_DR); 	// 写回TX_DR,重置NRF_IRQ_PIN中断
            break;
        }
    }
    NRF_CE (0);                                                                     	// NRF_CE_PIN置低,进入待机模式
    NRF_SPI_W_Reg_Byte(NRF_CMD_W_REGISTER | NRF_REG_CONFIG,     NRF_RCD_TX_DS_DI        // 禁用发送完成中断
                       | NRF_RCD_MAX_RT_DI       // 禁用发送超时中断
                       | NRF_RCD_CRC_EN          // 启用CRC
                       | NRF_RCD_CRC16           // CRC16
                       | NRF_RCD_PWR_UP          // 上电模式
                       | NRF_RCD_PRIM_PRX);      // RX模式
    NRF_CE (1);                                                                     	// NRF_CE_PIN置高,进入运行模式
}


//-------------------------------------------------------------------------------------------------------------------
//  @brief      NRF无线模块数据接收函数
//  @param      *p			准备接收的数据的数组地址
//  @return     uint8		接收成功标志,为1时表明数据接收成功
//  @since      v1.0
//  Sample usage:			检查nRF24L01+是否接收到数据,接收到数据时将数据读取到接收数据缓冲区中
//-------------------------------------------------------------------------------------------------------------------
uint8 NRF_Rece_Packet(uint8 *p,uint8 len)
{
    uint8 sta=0;                                                              	// 等待NRF_IRQ_PIN置低
    sta = NRF_SPI_R_Reg_Byte(NRF_CMD_R_REGISTER | NRF_REG_STATUS);                  // 读取STATUS寄存器
    if(sta & NRF_RCD_RX_DR)
    {   // RX_DS为1时表明接收到数据

        NRF_SPI_R_Reg_Burst(NRF_CMD_R_RX_PAYLOAD, p, len);            			    // 读取nRF24L01+中的接收数据
        NRF_SPI_W_Cmd_Byte(NRF_CMD_FLUSH_RX);																		// 清除RX FIFO(注意：这句话很必要)
        NRF_SPI_W_Reg_Byte(NRF_CMD_W_REGISTER | NRF_REG_STATUS, NRF_RCD_RX_DR);     // 写回RX_DS,重置NRF_IRQ_PIN中断
        return(1);                                                                  // 返回接收完成标志
    }
    return(0);                       // 返回接收检查失败标志
}


void Reciver_Buff(void)
{
    NRF_CE (0);
    NRF_Rece_Packet(NRF_RX_Buff,NRF_DATA_WIDTH);		//接收七位数据

    if(NRF_RX_Buff[1]== 0XFF && NRF_RX_Buff[2] == 0XFE)		//判断帧头
    {
        Remoto_data.L_Y = (int)NRF_RX_Buff[3] ;
        Remoto_data.L_X = (int)NRF_RX_Buff[4] ;
        Remoto_data.R_Y = (int)NRF_RX_Buff[5] ;
        Remoto_data.R_X = (int)NRF_RX_Buff[6] ;
        Remoto_data.control = NRF_RX_Buff[7];			//按键位
        Remoto_data.Flag = 1;											//接收完成标志位
        Remoto_data.Last_Time = uwTick;						//记录本次接收时间
        NRF_RX_Buff[1] = 0x00;									//标识本次接收结束,防止重复接收

//       Remoto_data.L_X = LIMIT(Remoto_data.L_X,0,64);			//限幅
//       Remoto_data.L_Y = LIMIT(Remoto_data.L_Y,15,64);
//       Remoto_data.R_X = LIMIT(Remoto_data.R_X,0,64);
//       Remoto_data.R_Y = LIMIT(Remoto_data.R_Y,0,64);

        HAL_GPIO_TogglePin(LED1_GPIO_Port,LED1_Pin);	//数据接收指示灯闪烁
    }
    NRF_CE (1);

}

//GPIO中断重定义
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
//    HAL_GPIO_TogglePin(LED1_GPIO_Port,LED1_Pin);

    Reciver_Buff();

}