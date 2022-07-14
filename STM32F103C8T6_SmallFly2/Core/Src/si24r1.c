#include "si24r1.h"
uint8 NRF_RF_Addr[NRF_ADDR_WIDTH]= {0x00,0xFF,0x00,0xFF,0x00};  // TX,RX��ַ,�����и���

uint8 NRF_RX_Buff[NRF_DATA_WIDTH];                      	// �������ݻ�����
uint8 NRF_TX_Buff[NRF_DATA_WIDTH];                      	// �������ݻ�����
struct remoto_data Remoto_data;														//	ң������������


/**************-----��Ӳ��SPI����-------***************/
// ����: NRF_SPI_WR_Byte()
// ����: ͨ��SPIдһ��byte��nRF24L01+,ͬʱ��nRF24L01+��ȡһ��byte
// ����: byte   ��д���ֽ�����
//       return ��ȡ�����ֽ�����
//		�ڲ�ʹ���û��������
uint8 NRF_SPI_WR_Byte(uint8 byte)
{
    HAL_SPI_TransmitReceive(&hspi1,&byte,&byte,1,100);
    return(byte);                                      		// ���ض�����һ�ֽ�
}

// ����: NRF_SPI_W_Cmd_Byte()
// ����: ��������д��nRF24L01+,ͬʱ����status�ֽ�
// ����: cmd    ������
//       return ����status״̬
//		�ڲ�ʹ���û��������
uint8 NRF_SPI_W_Cmd_Byte(uint8 cmd)
{
    uint8 status;
    NRF_CSN (0);                                    		// NRF_CSN_PIN�õ�,��ʼ��������
    status = NRF_SPI_WR_Byte(cmd);                      	// д��Ĵ�����ַ,ͬʱ����״̬��
    NRF_CSN (1);                                    		// NRF_CSN_PIN����,�������ݴ���
    return(status);                                     	// ����״̬�Ĵ���
}

// ����: NRF_SPI_W_Reg_Byte()
// ����: ��valд��addr��Ӧ�ļĴ�����ַ,ͬʱ����status�ֽ�
// ����: cmd    ������
//       val    ��д��Ĵ�������ֵ
//       return ����status״̬
//		�ڲ�ʹ���û��������
uint8 NRF_SPI_W_Reg_Byte(uint8 cmd, uint8 val)
{
    uint8 status;
    NRF_CSN (0);                                    		// NRF_CSN_PIN�õ�,��ʼ��������
    status = NRF_SPI_WR_Byte(cmd);                      	// д��Ĵ�����ַ,ͬʱ����״̬��
    NRF_SPI_WR_Byte(val);                               	// Ȼ��д���ݵ��üĴ���
    NRF_CSN (1);                                    		// NRF_CSN_PIN����,�������ݴ���
    return(status);                                     	// ����״̬�Ĵ���
}

// ����: NRF_SPI_W_Reg_Byte()
// ����: ��ȡaddr����Ӧ�ļĴ�����ַ
// ����: cmd    ������
//       return ���ؼĴ����ڵ���ֵ
//		�ڲ�ʹ���û��������
uint8 NRF_SPI_R_Reg_Byte(uint8 cmd)
{
    uint8 val;
    NRF_CSN (0);                                    		// NRF_CSN_PIN�õ�,��ʼ��������
    NRF_SPI_WR_Byte(cmd);                               	// д��Ĵ�����ַ
    val = NRF_SPI_WR_Byte(0);                           	// Ȼ��ӸüĴ���������
    NRF_CSN (1);                                    		// NRF_CSN_PIN����,�������ݴ���
    return(val);                                        	// ���ؼĴ�������
}

// ����: NRF_SPI_W_Reg_Burst()
// ����: ����д��len���ֽڵ�addrָ���ļĴ�����,ͬʱ����status״̬��
// ����: cmd    ������
//       *p     ��д������ָ��
//       len    ��д�����ݳ���
//       return ����status״̬
//		�ڲ�ʹ���û��������
uint8 NRF_SPI_W_Reg_Burst(uint8 cmd, uint8 *p, uint8 len)
{
    uint8 status, i;
    NRF_CSN (0);                                    		// NRF_CSN_PIN�õ�,��ʼ��������
    status = NRF_SPI_WR_Byte(cmd);                      	// ѡ��Ĵ���,ͬʱ����״̬��
    for(i=0; i<len; i++)
        NRF_SPI_WR_Byte(p[i]);                          	// ����ֽ�д��nRF24L01+
    NRF_CSN (1);                                    		// NRF_CSN_PIN����,�������ݴ���
    return(status);                                     	// ����״̬�Ĵ���
}

// ����: NRF_SPI_R_Reg_Burst()
// ����: ������ȡlen���ֽڵ�addrָ���ļĴ�����,ͬʱ����status״̬��
// ����: cmd     ������
//       *p     ����ȡ����ָ��
//       len    ����ȡ���ݳ���
//       return ����status״̬
//		�ڲ�ʹ���û��������
uint8 NRF_SPI_R_Reg_Burst(uint8 cmd, uint8 *p, uint8 len)
{
    uint8 status, i;
    NRF_CSN (0);                                    		// NRF_CSN_PIN�õ�,��ʼ��������
    status = NRF_SPI_WR_Byte(cmd);                      	// ѡ��Ĵ���,ͬʱ����״̬��
    for(i=0; i<len; i++)
        p[i] = NRF_SPI_WR_Byte(0);                      	// ����ֽڴ�nRF24L01+����
    NRF_CSN (1);                                    		// NRF_CSN_PIN����,�������ݴ���
    return(status);                                     	// ����״̬�Ĵ���
}
/**************-----��Ӳ��SPI����-------***************/


//-------------------------------------------------------------------------------------------------------------------
//  @brief      NRF��麯��
//  @param      NULL
//  @return     void
//  @since      v1.0
//  Sample usage:
//-------------------------------------------------------------------------------------------------------------------
uint8 NRF_Check(void)
{
    uint8 temp;
    NRF_SPI_W_Reg_Byte(NRF_CMD_W_REGISTER | NRF_REG_RX_PW_P0,   NRF_DATA_WIDTH);    // ����RXͨ��0���ݿ��
    temp = NRF_SPI_R_Reg_Byte(NRF_CMD_R_REGISTER | NRF_REG_RX_PW_P0);               // ��ȡRXͨ��0���ݿ��

    return (NRF_DATA_WIDTH != temp);
    //�����������while����������¼������
    //1.nrfδ����
    //2.nrf�뵥Ƭ�����Ӳ��ɿ���������޷�ͨ�Ŷ�����Ϊ���
    //3.nrf�ǻ���,���NRF���µģ����Ŀ����Լ���
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      NRF����ģ���ʼ��
//  @param      NULL
//  @return     void
//  @since      v1.0
//  Sample usage:
//-------------------------------------------------------------------------------------------------------------------
void NRF_Dev_Init(void)
{
    HAL_Delay(100);  	//�ϵ���ʱ
    NRF_CE  (0);                                    		// ��/дnRF�Ĵ�������Ҫ��CE����
    NRF_CSN (1);                                    		// SPI��ֹ
    while(NRF_Check());

    NRF_SPI_W_Reg_Byte(NRF_CMD_W_REGISTER | NRF_REG_CONFIG,     NRF_RCD_TX_DS_DI        // ���÷�������ж�
                       | NRF_RCD_MAX_RT_DI       // ���÷��ͳ�ʱ�ж�
                       | NRF_RCD_CRC_EN          // ����CRC
                       | NRF_RCD_CRC16           // CRC16
                       | NRF_RCD_PWR_UP          // �ϵ�ģʽ
                       | NRF_RCD_PRIM_PRX);      // RXģʽ
    HAL_Delay(10);
    NRF_SPI_W_Reg_Byte(NRF_CMD_W_REGISTER | NRF_REG_EN_AA,      NRF_RCD_PIPE0_AA_EN);   // ʹ��ͨ��0�Զ�Ӧ��
    HAL_Delay(10);
    NRF_SPI_W_Reg_Byte(NRF_CMD_W_REGISTER | NRF_REG_EN_RXADDR,  NRF_RCD_PIPE0_RX_EN);   // ʹ�ܽ���ͨ��0
    HAL_Delay(10);
    NRF_SPI_W_Reg_Byte(NRF_CMD_W_REGISTER | NRF_REG_SETUP_AW,   NRF_RCD_AW_5byte);      // ����RX��ַ���
    HAL_Delay(10);
    NRF_SPI_W_Reg_Byte(NRF_CMD_W_REGISTER | NRF_REG_SETUP_RETR, NRF_RCD_ARC_10|NRF_RCD_ARD_250us);        // ʹ���Զ��ط�  ���ʱ��250US
    HAL_Delay(10);
    NRF_SPI_W_Reg_Byte(NRF_CMD_W_REGISTER | NRF_REG_RF_CH,      NRF_RF_CH);             // ������Ƶͨ��
    HAL_Delay(10);
    NRF_SPI_W_Reg_Byte(NRF_CMD_W_REGISTER | NRF_REG_RF_SETUP,   NRF_RCD_RF_DR_2Mbps     // ���ô�������Ϊ2Mbps
                       | NRF_RCD_RF_PWR_0dBm);   // ���书��Ϊ0dBm
    HAL_Delay(10);
    NRF_SPI_W_Reg_Byte(NRF_CMD_W_REGISTER | NRF_REG_RX_PW_P0,   NRF_DATA_WIDTH);        // ����RXͨ��0���ݿ��
    HAL_Delay(10);

    NRF_SPI_W_Reg_Burst(NRF_CMD_W_REGISTER | NRF_REG_RX_ADDR_P0, NRF_RF_Addr, NRF_ADDR_WIDTH);   // ����RXͨ��0��ַ
    HAL_Delay(10);
    NRF_SPI_W_Cmd_Byte(NRF_CMD_FLUSH_RX);                                               // ���RXFIFO
    HAL_Delay(10);
    NRF_CE (1);                                                                     	// NRF_CE_PIN�ø�,��������ģʽ
}


//-------------------------------------------------------------------------------------------------------------------
//  @brief      NRF����ģ�����ݷ��ͺ���
//  @param      *p			Ҫ���͵����ݵ�ַ
//  @return     void
//  @since      v1.0
//  Sample usage:			��ʼ��nRF24L01+,������ת��Ϊ����״̬���������ݻ������е�����,������ɺ���ת��Ϊ����״̬
//-------------------------------------------------------------------------------------------------------------------
void NRF_Send_Packet(uint8 *p)
{
    uint8 sta=0;

    NRF_CE (0);                                                                     	// NRF_CE_PIN�õ�,�������ģʽ
    NRF_SPI_W_Reg_Byte(NRF_CMD_W_REGISTER | NRF_REG_CONFIG, NRF_RCD_RX_DR_DI            // ���÷�������ж�
                       | NRF_RCD_MAX_RT_DI           // ���÷��ͳ�ʱ�ж�
                       | NRF_RCD_CRC_EN              // ����CRC
                       | NRF_RCD_CRC16               // CRC16
                       | NRF_RCD_PWR_UP              // �ϵ�ģʽ
                       | NRF_RCD_PRIM_PTX);          // TXģʽ

    NRF_SPI_W_Reg_Burst(NRF_CMD_W_REGISTER | NRF_REG_TX_ADDR, NRF_RF_Addr, NRF_ADDR_WIDTH);  // ����TX��ַ
    NRF_SPI_W_Reg_Burst(NRF_CMD_W_TX_PAYLOAD, p, NRF_DATA_WIDTH);                       // д�뷢������
    NRF_CE (1);                                                                     	// NRF_CE_PIN�ø�,��������ģʽ

    while(1)
    {
        sta = NRF_SPI_R_Reg_Byte(NRF_CMD_R_REGISTER | NRF_REG_STATUS);
        if(sta&0x10)//����ʧ��
        {
            NRF_SPI_W_Reg_Byte(NRF_CMD_W_REGISTER | NRF_REG_STATUS, 0x10); 				// д��TX_DR,����NRF_IRQ_PIN�ж�
            NRF_SPI_W_Reg_Byte(NRF_CMD_FLUSH_TX,0xff);
            break;
        }
        if(sta&0x20)//���ͳɹ�
        {
            NRF_SPI_W_Reg_Byte(NRF_CMD_W_REGISTER | NRF_REG_STATUS, NRF_RCD_TX_DR); 	// д��TX_DR,����NRF_IRQ_PIN�ж�
            break;
        }
    }
    NRF_CE (0);                                                                     	// NRF_CE_PIN�õ�,�������ģʽ
    NRF_SPI_W_Reg_Byte(NRF_CMD_W_REGISTER | NRF_REG_CONFIG,     NRF_RCD_TX_DS_DI        // ���÷�������ж�
                       | NRF_RCD_MAX_RT_DI       // ���÷��ͳ�ʱ�ж�
                       | NRF_RCD_CRC_EN          // ����CRC
                       | NRF_RCD_CRC16           // CRC16
                       | NRF_RCD_PWR_UP          // �ϵ�ģʽ
                       | NRF_RCD_PRIM_PRX);      // RXģʽ
    NRF_CE (1);                                                                     	// NRF_CE_PIN�ø�,��������ģʽ
}


//-------------------------------------------------------------------------------------------------------------------
//  @brief      NRF����ģ�����ݽ��պ���
//  @param      *p			׼�����յ����ݵ������ַ
//  @return     uint8		���ճɹ���־,Ϊ1ʱ�������ݽ��ճɹ�
//  @since      v1.0
//  Sample usage:			���nRF24L01+�Ƿ���յ�����,���յ�����ʱ�����ݶ�ȡ���������ݻ�������
//-------------------------------------------------------------------------------------------------------------------
uint8 NRF_Rece_Packet(uint8 *p,uint8 len)
{
    uint8 sta=0;                                                              	// �ȴ�NRF_IRQ_PIN�õ�
    sta = NRF_SPI_R_Reg_Byte(NRF_CMD_R_REGISTER | NRF_REG_STATUS);                  // ��ȡSTATUS�Ĵ���
    if(sta & NRF_RCD_RX_DR)
    {   // RX_DSΪ1ʱ�������յ�����

        NRF_SPI_R_Reg_Burst(NRF_CMD_R_RX_PAYLOAD, p, len);            			    // ��ȡnRF24L01+�еĽ�������
        NRF_SPI_W_Cmd_Byte(NRF_CMD_FLUSH_RX);																		// ���RX FIFO(ע�⣺��仰�ܱ�Ҫ)
        NRF_SPI_W_Reg_Byte(NRF_CMD_W_REGISTER | NRF_REG_STATUS, NRF_RCD_RX_DR);     // д��RX_DS,����NRF_IRQ_PIN�ж�
        return(1);                                                                  // ���ؽ�����ɱ�־
    }
    return(0);                       // ���ؽ��ռ��ʧ�ܱ�־
}


void Reciver_Buff(void)
{
    NRF_CE (0);
    NRF_Rece_Packet(NRF_RX_Buff,NRF_DATA_WIDTH);		//������λ����

    if(NRF_RX_Buff[1]== 0XFF && NRF_RX_Buff[2] == 0XFE)		//�ж�֡ͷ
    {
        Remoto_data.L_Y = (int)NRF_RX_Buff[3] ;
        Remoto_data.L_X = (int)NRF_RX_Buff[4] ;
        Remoto_data.R_Y = (int)NRF_RX_Buff[5] ;
        Remoto_data.R_X = (int)NRF_RX_Buff[6] ;
        Remoto_data.control = NRF_RX_Buff[7];			//����λ
        Remoto_data.Flag = 1;											//������ɱ�־λ
        Remoto_data.Last_Time = uwTick;						//��¼���ν���ʱ��
        NRF_RX_Buff[1] = 0x00;									//��ʶ���ν��ս���,��ֹ�ظ�����

//       Remoto_data.L_X = LIMIT(Remoto_data.L_X,0,64);			//�޷�
//       Remoto_data.L_Y = LIMIT(Remoto_data.L_Y,15,64);
//       Remoto_data.R_X = LIMIT(Remoto_data.R_X,0,64);
//       Remoto_data.R_Y = LIMIT(Remoto_data.R_Y,0,64);

        HAL_GPIO_TogglePin(LED1_GPIO_Port,LED1_Pin);	//���ݽ���ָʾ����˸
    }
    NRF_CE (1);

}

//GPIO�ж��ض���
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
//    HAL_GPIO_TogglePin(LED1_GPIO_Port,LED1_Pin);

    Reciver_Buff();

}