/********************************** (C) COPYRIGHT *******************************
 * File Name          : main.c
 * Description        : CH32V307 震动报警器 + 网口/WiFi双模式TCP上传
 * Board              : YD-CH32V307VC
 * 硬件接线：
 *   蜂鸣器  : PA1
 *   ADXL345 : I2C2 (SCL=PB10, SDA=PB11), 地址0xA6
 *   OLED    : I2C2 (SCL=PB10, SDA=PB11), 地址0x78
 *   网口    : RMII, PHY地址1
 *   ESP-01S : UART4 (TX=PC10, RX=PC11), 3.3V供电, EN接3.3V
 *   串口1   : PA9 (调试输出, 115200)
 *
 * 网络模式（改代码中NET_MODE宏，重新编译）：
 *   NET_MODE_NONE = 无网络（只本地功能）
 *   NET_MODE_ETH  = 网口（WCHNet）
 *   NET_MODE_WIFI = WiFi（ESP-01S）
 ********************************** (C) COPYRIGHT *******************************/
#include "ch32v30x.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "u8g2.h"
#include "eth_driver.h"

// ===================== 通用网络配置 =====================
#define SERVER_IP0   192
#define SERVER_IP1   168
#define SERVER_IP2   137
#define SERVER_IP3   1
#define SERVER_PORT  8888

// 网络模式定义（改这里切换模式，重新编译即可）
#define NET_MODE_NONE  0
#define NET_MODE_ETH   1
#define NET_MODE_WIFI  2
//===================== 改连接方式 ===================================================================================================================================================



                    //这NONE
#define NET_MODE       NET_MODE_WIFI   // ← 改这里：NET_MODE_ETH=网口, NET_MODE_WIFI=WiFi, NET_MODE_NONE=无网络
                    //这个这个↑



// ===================== 全局变量 ===================================================================================================================================================
uint8_t net_mode = NET_MODE;  // 当前网络模式（由宏定义决定）
uint8_t net_connected = 0;    // TCP是否连接成功

// ===================== 函数原型声明（解决隐式声明警告）=====================
void BEEP_Init(void);
void BEEP_On(void);
void BEEP_Off(void);
void I2C2_Init(void);
uint8_t ADXL345_Init(void);
void ADXL345_Read(int16_t *x, int16_t *y, int16_t *z);
void OLED_Init_u8g2(void);
void OLED_DrawFace_u8g2(uint8_t level);
void Network_Init(void);
void Network_Task(void);
uint8_t Network_Send(char *data);

// ===================== 网口专属变量 =====================
#define KEEPALIVE_ENABLE  1
u8 MACAddr[6];
u8 IPAddr[4]    = {192, 168, 1, 100};
u8 GWIPAddr[4]  = {192, 168, 1, 1};
u8 IPMask[4]    = {255, 255, 255, 0};
u8 DESIP[4]     = {SERVER_IP0, SERVER_IP1, SERVER_IP2, SERVER_IP3};
u16 desport     = SERVER_PORT;
u16 srcport     = 1000;
u8 SocketId;
u8 socket[WCHNET_MAX_SOCKET_NUM];
u8 SocketRecvBuf[WCHNET_MAX_SOCKET_NUM][RECE_BUF_LEN];

// ===================== WiFi专属变量（ESP-01S）=======================================================================================================================================================================================================

//可以手机开热点然后电脑开热点
                                                     //这个这个这个这个这个这个这个这个
#define WIFI_SSID   "ESP-01S"     //WiFi名字         这个这个这个这个这个这个这个这个这个这个这个这个这个这个这个这个这个这个这个这个这个这个这个这个
#define WIFI_PASS   "00000000"    //WiFi密码         这个这个这个这个这个这个这个这个这个这个这个这个这个这个这个这个这个这个这个这个这个这个这个这个
#define ESP_BAUD    115200                           //这个这个这个这个这个这个这个这个 




// ===================== 硬件引脚定义 =================================================================================================================================================================================================================
#define BEEP_PORT    GPIOA
#define BEEP_PIN     GPIO_Pin_1
#define I2C_SCL_PIN  GPIO_Pin_10
#define I2C_SDA_PIN  GPIO_Pin_11
#define ADXL_ADDR    0xA6
#define OLED_ADDR    0x78

// ===================== 震动阈值 =====================
#define THRESHOLD_LIGHT   30
#define THRESHOLD_THEFT   100
#define THRESHOLD_QUAKE   500

// ===================== u8g2 OLED驱动 =====================
u8g2_t u8g2;

uint8_t u8x8_byte_ch32_i2c(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr)
{
    static uint8_t buffer[64];
    static uint8_t buf_idx;
    static uint8_t i2c_addr;
    uint8_t *data;
    uint8_t i;
    volatile uint32_t timeout;

    switch(msg)
    {
        case U8X8_MSG_BYTE_SEND:
            data = (uint8_t *)arg_ptr;
            while(arg_int > 0) { buffer[buf_idx++] = *data; data++; arg_int--; }
            break;
        case U8X8_MSG_BYTE_INIT:
            break;
        case U8X8_MSG_BYTE_SET_DC:
            break;
        case U8X8_MSG_BYTE_START_TRANSFER:
            buf_idx = 0;
            i2c_addr = u8x8_GetI2CAddress(u8x8);
            break;
        case U8X8_MSG_BYTE_END_TRANSFER:
            I2C_GenerateSTART(I2C2, ENABLE);
            timeout = 10000;
            while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT))
                if(--timeout == 0) { I2C_GenerateSTOP(I2C2, ENABLE); return 0; }
            I2C_SendData(I2C2, i2c_addr);
            timeout = 10000;
            while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
                if(--timeout == 0) { I2C_GenerateSTOP(I2C2, ENABLE); return 0; }
            for(i = 0; i < buf_idx; i++)
            {
                I2C_SendData(I2C2, buffer[i]);
                timeout = 10000;
                while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
                    if(--timeout == 0) { I2C_GenerateSTOP(I2C2, ENABLE); return 0; }
            }
            I2C_GenerateSTOP(I2C2, ENABLE);
            break;
        default:
            break;
    }
    return 1;
}

uint8_t u8x8_gpio_and_delay_ch32(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr)
{
    switch(msg)
    {
        case U8X8_MSG_DELAY_MILLI:  Delay_Ms(arg_int); break;
        case U8X8_MSG_DELAY_10MICRO: Delay_Ms(1); break;
        default: break;
    }
    return 1;
}

void OLED_Init_u8g2(void)
{
    u8g2_Setup_ssd1306_i2c_128x64_noname_f(&u8g2, U8G2_R0, u8x8_byte_ch32_i2c, u8x8_gpio_and_delay_ch32);
    u8g2_InitDisplay(&u8g2);
    u8g2_SetPowerSave(&u8g2, 0);
}

// 在表情上方显示当前网络模式
void OLED_DrawFace_u8g2(uint8_t level)
{
    u8g2_ClearBuffer(&u8g2);

    // 顶部显示网络模式
    u8g2_SetFont(&u8g2, u8g2_font_5x8_tf);
    if(net_mode == NET_MODE_NONE)      u8g2_DrawStr(&u8g2, 40, 8, "NONE");
    else if(net_mode == NET_MODE_ETH)  u8g2_DrawStr(&u8g2, 44, 8, "ETH");
    else                               u8g2_DrawStr(&u8g2, 42, 8, "WiFi");

    // 连接状态指示
    if(net_connected) u8g2_DrawStr(&u8g2, 100, 8, "*");

    if(level == 0)  // 平静
    {
        u8g2_DrawBox(&u8g2, 32, 18, 18, 22);
        u8g2_DrawBox(&u8g2, 78, 18, 18, 22);
        u8g2_DrawLine(&u8g2, 52, 52, 56, 56);
        u8g2_DrawLine(&u8g2, 56, 56, 60, 58);
        u8g2_DrawLine(&u8g2, 60, 58, 64, 59);
        u8g2_DrawLine(&u8g2, 64, 59, 68, 58);
        u8g2_DrawLine(&u8g2, 68, 58, 72, 56);
        u8g2_DrawLine(&u8g2, 72, 56, 76, 52);
    }
    else if(level == 1)  // 惊讶
    {
        u8g2_DrawBox(&u8g2, 30, 16, 22, 26);
        u8g2_DrawBox(&u8g2, 76, 16, 22, 26);
        u8g2_DrawFrame(&u8g2, 56, 50, 16, 12);
    }
    else  // 害怕
    {
        u8g2_DrawLine(&u8g2, 28, 18, 50, 42);
        u8g2_DrawLine(&u8g2, 50, 18, 28, 42);
        u8g2_DrawLine(&u8g2, 32, 18, 54, 42);
        u8g2_DrawLine(&u8g2, 54, 18, 32, 42);
        u8g2_DrawLine(&u8g2, 74, 18, 96, 42);
        u8g2_DrawLine(&u8g2, 96, 18, 74, 42);
        u8g2_DrawLine(&u8g2, 78, 18, 100, 42);
        u8g2_DrawLine(&u8g2, 100, 18, 78, 42);
        u8g2_DrawBox(&u8g2, 52, 50, 24, 12);
    }
    u8g2_SendBuffer(&u8g2);
}

// ===================== 蜂鸣器 PA1 =====================
void BEEP_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    GPIO_InitStruct.GPIO_Pin = BEEP_PIN;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(BEEP_PORT, &GPIO_InitStruct);
    GPIO_ResetBits(BEEP_PORT, BEEP_PIN);
}
void BEEP_On(void)  { GPIO_SetBits(BEEP_PORT, BEEP_PIN); }
void BEEP_Off(void) { GPIO_ResetBits(BEEP_PORT, BEEP_PIN); }

// ===================== I2C2 =====================
void I2C2_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    I2C_InitTypeDef I2C_InitStruct = {0};
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, ENABLE);
    GPIO_InitStruct.GPIO_Pin = I2C_SCL_PIN | I2C_SDA_PIN;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_OD;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStruct);
    I2C_InitStruct.I2C_ClockSpeed = 50000;
    I2C_InitStruct.I2C_Mode = I2C_Mode_I2C;
    I2C_InitStruct.I2C_DutyCycle = I2C_DutyCycle_2;
    I2C_InitStruct.I2C_OwnAddress1 = 0x00;
    I2C_InitStruct.I2C_Ack = I2C_Ack_Enable;
    I2C_InitStruct.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    I2C_Init(I2C2, &I2C_InitStruct);
    I2C_Cmd(I2C2, ENABLE);
}

uint8_t I2C_WaitEvent(uint32_t event, uint32_t timeout)
{
    while(timeout--) if(I2C_CheckEvent(I2C2, event) == 1) return 0;
    return 1;
}

uint8_t I2C_WriteReg(uint8_t devAddr, uint8_t regAddr, uint8_t data)
{
    I2C_GenerateSTART(I2C2, ENABLE);
    if(I2C_WaitEvent(I2C_EVENT_MASTER_MODE_SELECT, 10000)) goto fail;
    I2C_SendData(I2C2, devAddr);
    if(I2C_WaitEvent(I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED, 10000)) goto fail;
    I2C_SendData(I2C2, regAddr);
    if(I2C_WaitEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTED, 10000)) goto fail;
    I2C_SendData(I2C2, data);
    if(I2C_WaitEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTED, 10000)) goto fail;
    I2C_GenerateSTOP(I2C2, ENABLE);
    return 0;
fail:
    I2C_GenerateSTOP(I2C2, ENABLE);
    return 1;
}

uint8_t I2C_ReadReg(uint8_t devAddr, uint8_t regAddr, uint8_t *data)
{
    I2C_GenerateSTART(I2C2, ENABLE);
    if(I2C_WaitEvent(I2C_EVENT_MASTER_MODE_SELECT, 10000)) goto fail;
    I2C_SendData(I2C2, devAddr);
    if(I2C_WaitEvent(I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED, 10000)) goto fail;
    I2C_SendData(I2C2, regAddr);
    if(I2C_WaitEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTED, 10000)) goto fail;
    I2C_GenerateSTART(I2C2, ENABLE);
    if(I2C_WaitEvent(I2C_EVENT_MASTER_MODE_SELECT, 10000)) goto fail;
    I2C_SendData(I2C2, devAddr | 0x01);
    if(I2C_WaitEvent(I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED, 10000)) goto fail;
    I2C_AcknowledgeConfig(I2C2, DISABLE);
    if(I2C_WaitEvent(I2C_EVENT_MASTER_BYTE_RECEIVED, 10000)) goto fail;
    *data = I2C_ReceiveData(I2C2);
    I2C_GenerateSTOP(I2C2, ENABLE);
    I2C_AcknowledgeConfig(I2C2, ENABLE);
    return 0;
fail:
    I2C_GenerateSTOP(I2C2, ENABLE);
    I2C_AcknowledgeConfig(I2C2, ENABLE);
    return 1;
}

// ===================== ADXL345 =====================
uint8_t ADXL345_Init(void)
{
    uint8_t id;
    if(I2C_ReadReg(ADXL_ADDR, 0x00, &id)) return 1;
    printf("ADXL ID: 0x%02X (should be E5)\r\n", id);
    if(id != 0xE5) return 2;
    I2C_WriteReg(ADXL_ADDR, 0x2D, 0x08);
    I2C_WriteReg(ADXL_ADDR, 0x31, 0x08);
    return 0;
}

void ADXL345_Read(int16_t *x, int16_t *y, int16_t *z)
{
    uint8_t xl, xh, yl, yh, zl, zh;
    I2C_ReadReg(ADXL_ADDR, 0x32, &xl);
    I2C_ReadReg(ADXL_ADDR, 0x33, &xh);
    I2C_ReadReg(ADXL_ADDR, 0x34, &yl);
    I2C_ReadReg(ADXL_ADDR, 0x35, &yh);
    I2C_ReadReg(ADXL_ADDR, 0x36, &zl);
    I2C_ReadReg(ADXL_ADDR, 0x37, &zh);
    *x = (int16_t)((xh << 8) | xl);
    *y = (int16_t)((yh << 8) | yl);
    *z = (int16_t)((zh << 8) | zl);
}

// ===================== 网口实现 =====================
void mStopIfError(u8 iError)
{
    if (iError == WCHNET_ERR_SUCCESS) return;
    printf("Error: %02X\r\n", (u16)iError);
}

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

void WCHNET_HandleSockInt(u8 socketid, u8 intstat)
{
    u8 i;
    if(intstat & SINT_STAT_RECV)
    {
        u32 len;
        len = SocketInf[socketid].RecvRemLen;
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
        net_connected = 1;
        printf(">>> TCP Connect Success! socket id: %d\r\n", socketid);
    }
    if(intstat & SINT_STAT_DISCONNECT)
    {
        for(i=0; i<WCHNET_MAX_SOCKET_NUM; i++) {
            if(socket[i] == socketid) { socket[i] = 0xff; break; }
        }
        net_connected = 0;
        printf(">>> TCP Disconnect\r\n");
    }
    if(intstat & SINT_STAT_TIM_OUT)
    {
        for(i=0; i<WCHNET_MAX_SOCKET_NUM; i++) {
            if(socket[i] == socketid) { socket[i] = 0xff; break; }
        }
        net_connected = 0;
        printf(">>> TCP Timeout, reconnect...\r\n");
        WCHNET_CreateTcpSocket();
    }
}

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
        else printf(">>> PHY Link Down\r\n");
    }
    if(intstat & GINT_STAT_SOCKET)
    {
        for(i=0; i<WCHNET_MAX_SOCKET_NUM; i++) {
            socketint = WCHNET_GetSocketInt(i);
            if(socketint) WCHNET_HandleSockInt(i, socketint);
        }
    }
}

void ETH_Network_Init(void)
{
    printf("[3/3] Init ETH...\r\n");
    WCHNET_GetMacAddr(MACAddr);
    printf("MAC: %02x:%02x:%02x:%02x:%02x:%02x\r\n",
           MACAddr[0],MACAddr[1],MACAddr[2],MACAddr[3],MACAddr[4],MACAddr[5]);
    printf("Board IP: %d.%d.%d.%d, Connecting to %d.%d.%d.%d:%d...\r\n",
           IPAddr[0],IPAddr[1],IPAddr[2],IPAddr[3],
           SERVER_IP0,SERVER_IP1,SERVER_IP2,SERVER_IP3,SERVER_PORT);

    TIM2_Init();
    u8 i = ETH_LibInit(IPAddr, GWIPAddr, IPMask, MACAddr);
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
    WCHNET_CreateTcpSocket();
}

void ETH_Task(void)
{
    WCHNET_MainTask();
    if(WCHNET_QueryGlobalInt()) WCHNET_HandleGlobalInt();
}

uint8_t ETH_Send(char *data)
{
    if(!net_connected) return 0;
    u32 len = strlen(data);
    u8 ret = WCHNET_SocketSend(SocketId, (u8*)data, &len);
    return (ret == WCHNET_ERR_SUCCESS) ? 1 : 0;
}

// ===================== WiFi实现（ESP-01S via UART4，查询方式）=====================
void UART4_SendChar(char ch)
{
    while(USART_GetFlagStatus(UART4, USART_FLAG_TC) == RESET);
    USART_SendData(UART4, ch);
}

void UART4_SendString(char *str)
{
    while(*str) UART4_SendChar(*str++);
}

void UART4_Init(uint32_t baud)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    USART_InitTypeDef USART_InitStruct = {0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);

    // PC10 = UART4_TX（发），PC11 = UART4_RX（收）
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStruct);

    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOC, &GPIO_InitStruct);

    USART_InitStruct.USART_BaudRate = baud;
    USART_InitStruct.USART_WordLength = USART_WordLength_8b;
    USART_InitStruct.USART_StopBits = USART_StopBits_1;
    USART_InitStruct.USART_Parity = USART_Parity_No;
    USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStruct.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
    USART_Init(UART4, &USART_InitStruct);
    USART_Cmd(UART4, ENABLE);
}

// 发AT指令并等待回复，成功返回1，失败返回0
// cmd为空字符串时只等待回复，不发送
uint8_t ESP_SendAT(char *cmd, char *expect, uint32_t timeout)
{
    char buf[256];
    uint16_t idx = 0;
    uint32_t t = timeout;
    if(cmd[0] != 0)
    {
        printf("[ESP TX] %s\r\n", cmd);
        UART4_SendString(cmd);
        UART4_SendString("\r\n");
    }
    while(t--)
    {
        // 每1ms内检查100次RXNE（每10us一次），防止丢字节
        for(uint8_t i = 0; i < 100; i++) {
            if(USART_GetFlagStatus(UART4, USART_FLAG_RXNE) == SET)
            {
                char c = USART_ReceiveData(UART4);
                if(idx < 255) buf[idx++] = c;
                buf[idx] = 0;
                if(strstr(buf, expect) != 0) return 1;
            }
            Delay_Us(10);
        }
    }
    buf[idx] = 0;
    printf("[ESP RX] ");
    for(uint16_t i = 0; i < idx; i++) printf("%02X ", (uint8_t)buf[i]);
    printf("('%s')\r\n", buf);
    return 0;
}

void WiFi_Init(void)
{
    printf("[3/3] Init WiFi (ESP-01S)...\r\n");

    printf("[ESP] Reset...\r\n");
    UART4_Init(115200);
    Delay_Ms(3000);  // 等3秒让ESP完全启动

    // ===== 深度调试：发AT后持续收5秒，打印所有数据 =====
    printf("[DEBUG] send AT, listen 5s...\r\n");
    UART4_SendString("AT\r\n");
    char dbg[512];
    uint16_t dbg_idx = 0;
    for(uint32_t t = 0; t < 5000; t++) {
        for(uint8_t i = 0; i < 100; i++) {
            if(USART_GetFlagStatus(UART4, USART_FLAG_RXNE) == SET) {
                uint8_t c = USART_ReceiveData(UART4);
                if(dbg_idx < 511) dbg[dbg_idx++] = c;
            }
            Delay_Us(10);
        }
    }
    dbg[dbg_idx] = 0;
    printf("[DEBUG] got %d bytes: ", dbg_idx);
    for(uint16_t i = 0; i < dbg_idx; i++) printf("%02X ", (uint8_t)dbg[i]);
    printf("\r\n[DEBUG] as text: '%s'\r\n", dbg);
    if(dbg_idx == 0) {
        printf("[DEBUG] NO DATA from ESP - check TX/RX wiring!\r\n");
        return;
    }

    // 自动检测波特率
    uint32_t bauds[] = {115200, 9600, 19200, 38400, 57600, 74880, 230400};
    uint8_t found = 0;
    uint32_t correct_baud = 115200;
    for(int i = 0; i < 7; i++) {
        UART4_Init(bauds[i]);
        Delay_Ms(100);
        printf("[BAUD] try %lu...\r\n", bauds[i]);
        if(ESP_SendAT("AT", "OK", 1000)) {
            correct_baud = bauds[i];
            found = 1;
            printf("[BAUD] OK! ESP baud=%lu\r\n", bauds[i]);
            break;
        }
    }
    if(!found) { printf("[ESP] AT FAIL (all bauds)\r\n"); return; }
    UART4_Init(correct_baud);
    printf("[OK] ESP AT Ready\r\n");

    // 设置Station模式
    ESP_SendAT("AT+CWMODE=1", "OK", 2000);

    // 连接WiFi
    printf("[ESP] Connecting WiFi...\r\n");
    char cmd[128];
    sprintf(cmd, "AT+CWJAP=\"%s\",\"%s\"", WIFI_SSID, WIFI_PASS);
    if(!ESP_SendAT(cmd, "WIFI GOT IP", 15000))
    {
        printf("[WARN] WiFi Connect Fail\r\n");
        return;
    }
    printf("[OK] WiFi Connected\r\n");

    // 查询ESP的IP地址，确认和电脑同网段
    ESP_SendAT("AT+CIFSR", "STAIP", 3000);

    // 连接TCP服务器
    printf("[ESP] Connecting Server...\r\n");
    sprintf(cmd, "AT+CIPSTART=\"TCP\",\"%d.%d.%d.%d\",%d",
            SERVER_IP0,SERVER_IP1,SERVER_IP2,SERVER_IP3,SERVER_PORT);
    if(!ESP_SendAT(cmd, "CONNECT", 10000))
    {
        printf("[WARN] TCP Connect Fail\r\n");
        return;
    }
    net_connected = 1;
    printf(">>> TCP Connect Success!\r\n");
}

void WiFi_Task(void)
{
    // WiFi模式下可处理ESP异步事件
}

uint8_t WiFi_Send(char *data)
{
    if(!net_connected) return 0;
    char cmd[32];
    uint16_t len = strlen(data);
    sprintf(cmd, "AT+CIPSEND=%d", len);
    if(!ESP_SendAT(cmd, ">", 3000)) return 0;
    UART4_SendString(data);
    return ESP_SendAT("", "SEND OK", 3000);
}

// ===================== 网络抽象层（根据net_mode分发）=====================
void Network_Init(void)
{
    if(net_mode == NET_MODE_ETH)      ETH_Network_Init();
    else if(net_mode == NET_MODE_WIFI) WiFi_Init();
    else                               printf("[3/3] Network: None (local only)\r\n");
}

void Network_Task(void)
{
    if(net_mode == NET_MODE_ETH)      ETH_Task();
    else if(net_mode == NET_MODE_WIFI) WiFi_Task();
}

uint8_t Network_Send(char *data)
{
    if(net_mode == NET_MODE_ETH)      return ETH_Send(data);
    else if(net_mode == NET_MODE_WIFI) return WiFi_Send(data);
    return 0;
}

// ===================== 主函数 =====================
int main(void)
{
    int16_t ax, ay, az;
    int16_t last_ax=0, last_ay=0, last_az=0;
    uint8_t level = 0;
    uint8_t raw_level = 0;
    int diff;
    int diff_filtered;
    int diff_history[5] = {0,0,0,0,0};
    uint8_t hist_idx = 0;
    uint8_t stable_count = 0;
    uint8_t oled_tick = 0;
    uint8_t send_tick = 0;
    char txbuf[64];

    SystemCoreClockUpdate();
    Delay_Init();
    USART_Printf_Init(115200);

    printf("\r\n=== CH32V307 Vibration Monitor");
    if(net_mode == NET_MODE_ETH)      printf(" (ETH)");
    else if(net_mode == NET_MODE_WIFI) printf(" (WiFi)");
    else                               printf(" (NONE)");
    printf(" ===\r\n");
    printf("[CLK] SystemCoreClock = %u Hz\r\n", (unsigned int)SystemCoreClock);
    printf("[Mode] Change NET_MODE macro to switch network\r\n");

    // 初始化硬件
    BEEP_Init();
    I2C2_Init();
    printf("[1/3] Init ADXL...\r\n");
    if(ADXL345_Init() != 0) printf("[WARN] ADXL Fail\r\n");
    else printf("[OK] ADXL Ready\r\n");

    printf("[2/3] Init OLED...\r\n");
    OLED_Init_u8g2();
    OLED_DrawFace_u8g2(0);
    printf("[OK] OLED Ready\r\n");

    // 初始化网络
    Network_Init();

    // 先读一次传感器，避免上电误报警
    ADXL345_Read(&last_ax, &last_ay, &last_az);

    printf("=== Ready ===\r\n\r\n");

    while(1)
    {
        // ========== 网络协议栈任务 ==========
        Network_Task();

        // ========== 传感器读取 + 报警判断 ==========
        ADXL345_Read(&ax, &ay, &az);

        diff = abs(ax - last_ax) + abs(ay - last_ay) + abs(az - last_az);
        last_ax = ax; last_ay = ay; last_az = az;

        // 滑动平均滤波
        diff_history[hist_idx] = diff;
        hist_idx++;
        if(hist_idx >= 5) hist_idx = 0;
        diff_filtered = (diff_history[0] + diff_history[1] + diff_history[2]
                       + diff_history[3] + diff_history[4]) / 5;

        // 四级分级
        if(diff_filtered >= THRESHOLD_QUAKE)      raw_level = 3;
        else if(diff_filtered >= THRESHOLD_THEFT) raw_level = 2;
        else if(diff_filtered >= THRESHOLD_LIGHT) raw_level = 1;
        else                                       raw_level = 0;

        // 持续震动判断
        if(raw_level >= 1)
        {
            if(stable_count < 255) stable_count++;
            if(stable_count >= 3) level = raw_level;
        }
        else
        {
            if(stable_count > 0) stable_count--;
            if(stable_count == 0) level = 0;
        }

        // 蜂鸣器
        if(level >= 2) BEEP_On(); else BEEP_Off();

        // ========== OLED刷新（100ms）==========
        oled_tick++;
        if(oled_tick >= 2)
        {
            oled_tick = 0;
            OLED_DrawFace_u8g2(level);
        }

        // ========== 串口输出 + 网络发送（500ms）==========
        send_tick++;
        if(send_tick >= 10)
        {
            send_tick = 0;
            printf("X:%d Y:%d Z:%d Diff:%d ", ax, ay, az, diff_filtered);
            if(level == 0)      printf("[SAFE]\r\n");
            else if(level == 1) printf("[LIGHT]\r\n");
            else if(level == 2) printf("[THEFT]\r\n");
            else                printf("[QUAKE]\r\n");

            if(net_connected)
            {
                sprintf(txbuf, "%d,%d,%d,%d,%d\n", ax, ay, az, diff_filtered, level);
                Network_Send(txbuf);
            }
        }

        Delay_Ms(50);
    }
}
