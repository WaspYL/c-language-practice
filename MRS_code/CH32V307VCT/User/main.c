#include "ch32v30x.h"
#include <stdlib.h>
#include <stdint.h>

// ===================== 硬件引脚定义 =====================
// 蜂鸣器
#define BEEP_PORT    GPIOA
#define BEEP_PIN     GPIO_Pin_1
// I2C2 (ADXL345 + OLED 共用这条总线)
#define I2C_SCL_PORT GPIOB
#define I2C_SCL_PIN  GPIO_Pin_10
#define I2C_SDA_PORT GPIOB
#define I2C_SDA_PIN  GPIO_Pin_11
// ADXL345 I2C地址
#define ADXL_ADDR    0xA6
// OLED I2C地址
#define OLED_ADDR    0x78

// ===================== 基础函数 =====================
void Delay_Init(void);
void Delay_Ms(uint32_t ms);

// ===================== 串口函数 =====================
void USART1_SendChar(char ch)
{
    while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
    USART_SendData(USART1, ch);
}

void USART1_SendString(char *str)
{
    while(*str != '\0')
    {
        USART1_SendChar(*str);
        str++;
    }
}

void USART1_SendNum(uint32_t num)
{
    char temp[10];
    int i = 0;
    if(num == 0) { USART1_SendChar('0'); return; }
    while(num > 0) { temp[i++] = '0' + (num % 10); num /= 10; }
    while(i > 0) { i--; USART1_SendChar(temp[i]); }
}

void USART1_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    USART_InitTypeDef USART_InitStruct = {0};
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStruct);
    USART_InitStruct.USART_BaudRate = 115200;
    USART_InitStruct.USART_WordLength = USART_WordLength_8b;
    USART_InitStruct.USART_StopBits = USART_StopBits_1;
    USART_InitStruct.USART_Parity = USART_Parity_No;
    USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStruct.USART_Mode = USART_Mode_Tx;
    USART_Init(USART1, &USART_InitStruct);
    USART_Cmd(USART1, ENABLE);
}

// ===================== 蜂鸣器函数 =====================
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

// ===================== I2C2 函数 (带超时保护) =====================
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

    I2C_InitStruct.I2C_ClockSpeed = 100000;
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
    while(timeout--)
    {
        if(I2C_CheckEvent(I2C2, event) == 1) return 0;
    }
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

// ===================== OLED 函数 (复用I2C2) =====================
void OLED_WriteCmd(uint8_t cmd)
{
    I2C_GenerateSTART(I2C2, ENABLE);
    if(I2C_WaitEvent(I2C_EVENT_MASTER_MODE_SELECT, 10000)) { I2C_GenerateSTOP(I2C2, ENABLE); return; }
    I2C_SendData(I2C2, OLED_ADDR);
    if(I2C_WaitEvent(I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED, 10000)) { I2C_GenerateSTOP(I2C2, ENABLE); return; }
    I2C_SendData(I2C2, 0x00);
    if(I2C_WaitEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTED, 10000)) { I2C_GenerateSTOP(I2C2, ENABLE); return; }
    I2C_SendData(I2C2, cmd);
    if(I2C_WaitEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTED, 10000)) { I2C_GenerateSTOP(I2C2, ENABLE); return; }
    I2C_GenerateSTOP(I2C2, ENABLE);
}

void OLED_WriteData(uint8_t dat)
{
    I2C_GenerateSTART(I2C2, ENABLE);
    if(I2C_WaitEvent(I2C_EVENT_MASTER_MODE_SELECT, 10000)) { I2C_GenerateSTOP(I2C2, ENABLE); return; }
    I2C_SendData(I2C2, OLED_ADDR);
    if(I2C_WaitEvent(I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED, 10000)) { I2C_GenerateSTOP(I2C2, ENABLE); return; }
    I2C_SendData(I2C2, 0x40);
    if(I2C_WaitEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTED, 10000)) { I2C_GenerateSTOP(I2C2, ENABLE); return; }
    I2C_SendData(I2C2, dat);
    if(I2C_WaitEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTED, 10000)) { I2C_GenerateSTOP(I2C2, ENABLE); return; }
    I2C_GenerateSTOP(I2C2, ENABLE);
}

void OLED_Init(void)
{
    Delay_Ms(100);
    OLED_WriteCmd(0xAE);
    OLED_WriteCmd(0x20); OLED_WriteCmd(0x10);
    OLED_WriteCmd(0xB0);
    OLED_WriteCmd(0xC8);
    OLED_WriteCmd(0x00); OLED_WriteCmd(0x10);
    OLED_WriteCmd(0x40);
    OLED_WriteCmd(0x81); OLED_WriteCmd(0xFF);
    OLED_WriteCmd(0xA1);
    OLED_WriteCmd(0xA6);
    OLED_WriteCmd(0xA8); OLED_WriteCmd(0x3F);
    OLED_WriteCmd(0xA4);
    OLED_WriteCmd(0xD3); OLED_WriteCmd(0x00);
    OLED_WriteCmd(0xD5); OLED_WriteCmd(0xF0);
    OLED_WriteCmd(0xD9); OLED_WriteCmd(0x22);
    OLED_WriteCmd(0xDA); OLED_WriteCmd(0x12);
    OLED_WriteCmd(0xDB); OLED_WriteCmd(0x20);
    OLED_WriteCmd(0x8D); OLED_WriteCmd(0x14);
    OLED_WriteCmd(0xAF);
    // 全屏全白测试
    for(uint8_t y=0; y<8; y++)
    {
        OLED_WriteCmd(0xb0 + y);
        OLED_WriteCmd(0x00); OLED_WriteCmd(0x10);
        for(uint8_t x=0; x<128; x++) OLED_WriteData(0xFF);
    }
}

// ===================== ADXL345 函数 =====================
uint8_t ADXL345_Init(void)
{
    uint8_t id;
    if(I2C_ReadReg(ADXL_ADDR, 0x00, &id)) return 1;
    USART1_SendString("ADXL ID: 0x");
    USART1_SendNum(id);
    USART1_SendString("\r\n");
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

// ===================== main 函数 =====================
int main(void)
{
    Delay_Init();
    USART1_Init();
    BEEP_Init();

    USART1_SendString("=== 系统启动 ===\r\n");

    USART1_SendString("[1/4] 初始化I2C2...\r\n");
    I2C2_Init();
    Delay_Ms(100);

    USART1_SendString("[2/4] 初始化ADXL345...\r\n");
    if(ADXL345_Init() != 0)
    {
        USART1_SendString("[警告] ADXL345初始化失败\r\n");
    }
    else
    {
        USART1_SendString("[OK] ADXL345正常\r\n");
    }

    USART1_SendString("[3/3] 蜂鸣器自检\r\n");
    BEEP_On(); Delay_Ms(1000); BEEP_Off();

    USART1_SendString("=== 初始化完成 ===\r\n\r\n");

    int16_t ax, ay, az;
    int16_t last_ax = 0, last_ay = 0, last_az = 0;

    while(1)
    {
        ADXL345_Read(&ax, &ay, &az);
        
        USART1_SendString("X:"); USART1_SendNum(ax);
        USART1_SendString(" Y:"); USART1_SendNum(ay);
        USART1_SendString(" Z:"); USART1_SendNum(az);
        
        int diff = abs(ax - last_ax) + abs(ay - last_ay) + abs(az - last_az);
        if(diff > 50)
        {
            BEEP_On();
            USART1_SendString("  <-- 震动报警!");
        }
        else
        {
            BEEP_Off();
        }
        last_ax = ax; last_ay = ay; last_az = az;

        USART1_SendString("\r\n");
        Delay_Ms(200);
    }
}
