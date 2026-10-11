/********************************** (C) COPYRIGHT *******************************
 * File Name          : main.c
 * Description        : CH32V307 TCP Client + ADXL345 震动监测
 * Board              : YD-CH32V307VC
 * ADXL345            : SCL=PB3, SDA=PB5
 * Buzzer             : PB1
 * TCP                : 板子IP=192.168.1.100, 连电脑192.168.1.10:8888
 ********************************** (C) COPYRIGHT *******************************/
#include "string.h"
#include "eth_driver.h"

#define KEEPALIVE_ENABLE         1

// ==================== 网络配置 ====================
u8 MACAddr[6];
u8 IPAddr[4] = {192, 168, 1, 100};        // 板子自己的IP
u8 GWIPAddr[4] = {192, 168, 1, 1};        // 网关
u8 IPMask[4] = {255, 255, 255, 0};        // 子网掩码
u8 DESIP[4] = {192, 168, 1, 10};          // 电脑IP（Python TCP服务器）
u16 desport = 8888;                        // 服务器端口
u16 srcport = 1000;

u8 SocketId;
u8 socket[WCHNET_MAX_SOCKET_NUM];
u8 SocketRecvBuf[WCHNET_MAX_SOCKET_NUM][RECE_BUF_LEN];
u8 MyBuf[RECE_BUF_LEN];
u8 tcp_connected = 0;                      // TCP是否连接成功

// ==================== ADXL345 软件I2C ====================
// SCL=PB3, SDA=PB5
#define SCL_H()   GPIO_WriteBit(GPIOB, GPIO_Pin_3, Bit_SET)
#define SCL_L()   GPIO_WriteBit(GPIOB, GPIO_Pin_3, Bit_RESET)
#define SDA_H()   GPIO_WriteBit(GPIOB, GPIO_Pin_5, Bit_SET)
#define SDA_L()   GPIO_WriteBit(GPIOB, GPIO_Pin_5, Bit_RESET)
#define SDA_READ() GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_5)

#define ADXL_ADDR_W  0xA6
#define ADXL_ADDR_R  0xA7

void SoftI2C_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_5;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_OD;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStruct);
    SCL_H(); SDA_H();
}

void SoftI2C_Start(void)
{
    SDA_H(); SCL_H(); Delay_Us(5);
    SDA_L(); Delay_Us(5);
    SCL_L(); Delay_Us(5);
}

void SoftI2C_Stop(void)
{
    SDA_L(); SCL_H(); Delay_Us(5);
    SDA_H(); Delay_Us(5);
}

void SoftI2C_SendByte(u8 dat)
{
    u8 i;
    for(i=0; i<8; i++)
    {
        if(dat & 0x80) SDA_H(); else SDA_L();
        dat <<= 1;
        Delay_Us(2);
        SCL_H(); Delay_Us(5);
        SCL_L(); Delay_Us(2);
    }
    SDA_H(); Delay_Us(2);
    SCL_H(); Delay_Us(5);
    SCL_L();
}

u8 SoftI2C_ReadByte(void)
{
    u8 i, dat=0;
    for(i=0; i<8; i++)
    {
        dat <<= 1;
        SCL_H(); Delay_Us(5);
        if(SDA_READ()) dat |= 0x01;
        SCL_L(); Delay_Us(2);
    }
    return dat;
}

void ADXL_WriteReg(u8 reg, u8 val)
{
    SoftI2C_Start();
    SoftI2C_SendByte(ADXL_ADDR_W);
    SoftI2C_SendByte(reg);
    SoftI2C_SendByte(val);
    SoftI2C_Stop();
}

u8 ADXL_ReadReg(u8 reg)
{
    u8 val;
    SoftI2C_Start();
    SoftI2C_SendByte(ADXL_ADDR_W);
    SoftI2C_SendByte(reg);
    SoftI2C_Start();
    SoftI2C_SendByte(ADXL_ADDR_R);
    val = SoftI2C_ReadByte();
    SoftI2C_Stop();
    return val;
}

void ADXL_ReadXYZ(s16 *ax, s16 *ay, s16 *az)
{
    u8 buf[6];
    SoftI2C_Start();
    SoftI2C_SendByte(ADXL_ADDR_W);
    SoftI2C_SendByte(0x32 | 0x40);
    SoftI2C_Start();
    SoftI2C_SendByte(ADXL_ADDR_R);
    buf[0] = SoftI2C_ReadByte();
    buf[1] = SoftI2C_ReadByte();
    buf[2] = SoftI2C_ReadByte();
    buf[3] = SoftI2C_ReadByte();
    buf[4] = SoftI2C_ReadByte();
    buf[5] = SoftI2C_ReadByte();
    SoftI2C_Stop();
    *ax = (s16)(buf[0] | (buf[1]<<8));
    *ay = (s16)(buf[2] | (buf[3]<<8));
    *az = (s16)(buf[4] | (buf[5]<<8));
}

void ADXL_Init(void)
{
    ADXL_WriteReg(0x2D, 0x08);
    ADXL_WriteReg(0x31, 0x00);
}

// ==================== 蜂鸣器 PB1 ====================
void Beep_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_1;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStruct);
    GPIO_WriteBit(GPIOB, GPIO_Pin_1, Bit_RESET);
}

void Beep_On(void)  { GPIO_WriteBit(GPIOB, GPIO_Pin_1, Bit_SET); }
void Beep_Off(void) { GPIO_WriteBit(GPIOB, GPIO_Pin_1, Bit_RESET); }

// ==================== 错误检查 ====================
void mStopIfError(u8 iError)
{
    if (iError == WCHNET_ERR_SUCCESS) return;
    printf("Error: %02X\r\n", (u16)iError);
}

// ==================== TIM2 网络定时器 ====================
void TIM2_Init(void)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure = {0};
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    TIM_TimeBaseStructure.TIM_Period = SystemCoreClock / 1000000;
    TIM_TimeBaseStructure.TIM_Prescaler = WCHNETTIMERPERIOD * 1000 - 1;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
    TIM_Cmd(TIM2, ENABLE);
    TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
    NVIC_EnableIRQ(TIM2_IRQn);
}

// ==================== 创建TCP Socket ====================
void WCHNET_CreateTcpSocket(void)
{
    u8 i;
    SOCK_INF TmpSocketInf;
    memset((void *)&TmpSocketInf, 0, sizeof(SOCK_INF));
    memcpy((void *)TmpSocketInf.IPAddr, DESIP, 4);
    TmpSocketInf.DesPort = desport;
    TmpSocketInf.SourPort = srcport++;
    TmpSocketInf.ProtoType = PROTO_TYPE_TCP;
    TmpSocketInf.RecvBufLen = RECE_BUF_LEN;
    i = WCHNET_SocketCreat(&SocketId, &TmpSocketInf);
    printf("SocketId %d\r\n", SocketId);
    mStopIfError(i);
    i = WCHNET_SocketConnect(SocketId);
    mStopIfError(i);
}

// ==================== Socket中断处理 ====================
void WCHNET_HandleSockInt(u8 socketid, u8 intstat)
{
    u8 i;
    if(intstat & SINT_STAT_RECV)
    {
        // 收到电脑数据，原样回发（演示用）
        u32 len;
        len = SocketInf[socketid].RecvRemLen;
        WCHNET_SocketSend(socketid, (u8*)SocketInf[socketid].RecvReadPoint, &len);
        WCHNET_SocketRecv(socketid, NULL, &len);
    }
    if(intstat & SINT_STAT_CONNECT)
    {
#if KEEPALIVE_ENABLE
        WCHNET_SocketSetKeepLive(socketid, ENABLE);
#endif
        WCHNET_ModifyRecvBuf(socketid, (u32)SocketRecvBuf[socketid], RECE_BUF_LEN);
        for(i=0; i<WCHNET_MAX_SOCKET_NUM; i++) {
            if(socket[i] == 0xff) { socket[i] = socketid; break; }
        }
        tcp_connected = 1;
        printf(">>> TCP Connect Success! socket id: %d\r\n", socketid);
    }
    if(intstat & SINT_STAT_DISCONNECT)
    {
        for(i=0; i<WCHNET_MAX_SOCKET_NUM; i++) {
            if(socket[i] == socketid) { socket[i] = 0xff; break; }
        }
        tcp_connected = 0;
        Beep_Off();
        printf(">>> TCP Disconnect\r\n");
    }
    if(intstat & SINT_STAT_TIM_OUT)
    {
        for(i=0; i<WCHNET_MAX_SOCKET_NUM; i++) {
            if(socket[i] == socketid) { socket[i] = 0xff; break; }
        }
        tcp_connected = 0;
        printf(">>> TCP Timeout, reconnect...\r\n");
        WCHNET_CreateTcpSocket();
    }
}

// ==================== 全局中断处理 ====================
void WCHNET_HandleGlobalInt(void)
{
    u8 intstat;
    u16 i;
    u8 socketint;
    intstat = WCHNET_GetGlobalInt();
    if(intstat & GINT_STAT_UNREACH)       printf("GINT_STAT_UNREACH\r\n");
    if(intstat & GINT_STAT_IP_CONFLI)     printf("GINT_STAT_IP_CONFLI\r\n");
    if(intstat & GINT_STAT_PHY_CHANGE)
    {
        i = WCHNET_GetPHYStatus();
        if(i & PHY_Linked_Status) printf(">>> PHY Link Success\r\n");
    }
    if(intstat & GINT_STAT_SOCKET)
    {
        for(i=0; i<WCHNET_MAX_SOCKET_NUM; i++) {
            socketint = WCHNET_GetSocketInt(i);
            if(socketint) WCHNET_HandleSockInt(i, socketint);
        }
    }
}

// ==================== 主函数 ====================
int main(void)
{
    u8 i;
    s16 ax, ay, az;
    s16 lastX=0, lastY=0, lastZ=0;
    u16 shake;
    u8 level;
    char txbuf[64];

    SystemCoreClockUpdate();
    Delay_Init();
    USART_Printf_Init(115200);
    printf("\r\n=== CH32V307 Vibration Monitor ===\r\n");

    // 初始化ADXL345和蜂鸣器
    SoftI2C_Init();
    ADXL_Init();
    Beep_Init();

    // 读一次DEVID验证ADXL345
    printf("ADXL345 DEVID = %d (should be 229)\r\n", ADXL_ReadReg(0x00));

    // 初始化以太网
    WCHNET_GetMacAddr(MACAddr);
    printf("MAC: %02x:%02x:%02x:%02x:%02x:%02x\r\n",
           MACAddr[0],MACAddr[1],MACAddr[2],MACAddr[3],MACAddr[4],MACAddr[5]);
    printf("Board IP: 192.168.1.100, Connecting to 192.168.1.10:8888...\r\n");

    TIM2_Init();
    i = ETH_LibInit(IPAddr, GWIPAddr, IPMask, MACAddr);
    mStopIfError(i);
    if(i == WCHNET_ERR_SUCCESS) printf(">>> ETH Init Success\r\n");

#if KEEPALIVE_ENABLE
    {
        struct _KEEP_CFG cfg;
        cfg.KLIdle = 20000;
        cfg.KLIntvl = 15000;
        cfg.KLCount = 9;
        WCHNET_ConfigKeepLive(&cfg);
    }
#endif
    memset(socket, 0xff, WCHNET_MAX_SOCKET_NUM);
    for(i=0; i<WCHNET_MAX_SOCKET_NUM; i++) WCHNET_CreateTcpSocket();

    while(1)
    {
        WCHNET_MainTask();
        if(WCHNET_QueryGlobalInt()) WCHNET_HandleGlobalInt();

        // 如果TCP已连接，每200ms读一次加速度并发送
        if(tcp_connected)
        {
            ADXL_ReadXYZ(&ax, &ay, &az);

            // 计算震动量：三轴变化量绝对值之和
            shake = (ax>lastX ? ax-lastX : lastX-ax)
                  + (ay>lastY ? ay-lastY : lastY-ay)
                  + (az>lastZ ? az-lastZ : lastZ-az);
            lastX = ax; lastY = ay; lastZ = az;

            // 震动等级：0=静止, 1=轻微, 2=强烈
            if(shake < 10)       level = 0;
            else if(shake < 50)  level = 1;
            else                 level = 2;

            // 蜂鸣器：强烈震动时响
            if(level == 2) Beep_On(); else Beep_Off();

            // 组包：X,Y,Z,shake,level\n
            sprintf(txbuf, "%d,%d,%d,%d,%d\n", ax, ay, az, shake, level);
            u32 len = strlen(txbuf);
            WCHNET_SocketSend(SocketId, (u8*)txbuf, &len);

            Delay_Ms(200);
        }
    }
}
