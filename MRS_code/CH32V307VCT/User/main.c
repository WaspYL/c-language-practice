#include "ch32v30x.h"
#include <stdlib.h>
#include <stdint.h>
#include "u8g2.h"

// 函数声明（避免隐式声明警告）
void USART1_SendString(char *str);
void USART1_SendChar(char ch);
void USART1_SendNum(uint32_t num);
void I2C2_Init(void);
uint8_t ADXL345_Init(void);
void ADXL345_Read(int16_t *x, int16_t *y, int16_t *z);
void BEEP_Init(void);
void BEEP_On(void);
void BEEP_Off(void);
void OLED_Init_u8g2(void);
void OLED_DrawFace_u8g2(uint8_t level);
void Pixel(u8g2_t *u8g2, uint8_t x, uint8_t y);


// ===================== u8g2 OLED驱动 =====================
u8g2_t u8g2;  // u8g2全局对象

// u8g2回调函数：告诉u8g2怎么用I2C发数据（带超时保护）
uint8_t u8x8_byte_ch32_i2c(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr)
{
    static uint8_t buffer[64];
    static uint8_t buf_idx;
    static uint8_t i2c_addr;  // 保存I2C地址
    uint8_t *data;
    uint8_t i;
    volatile uint32_t timeout;

    switch(msg)
    {
        case U8X8_MSG_BYTE_SEND:
            data = (uint8_t *)arg_ptr;
            while(arg_int > 0)
            {
                buffer[buf_idx++] = *data;
                data++;
                arg_int--;
            }
            break;
        case U8X8_MSG_BYTE_INIT:
            break;
        case U8X8_MSG_BYTE_SET_DC:
            break;
        case U8X8_MSG_BYTE_START_TRANSFER:
            buf_idx = 0;
            i2c_addr = u8x8_GetI2CAddress(u8x8);  // 正确获取I2C地址的方式！
            break;
        case U8X8_MSG_BYTE_END_TRANSFER:
            // 发START
            I2C_GenerateSTART(I2C2, ENABLE);
            timeout = 10000;
            while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT))
            {
                if(--timeout == 0) { I2C_GenerateSTOP(I2C2, ENABLE); return 0; }
            }
            // 发地址（u8g2传的地址已经是8位写地址）
            I2C_SendData(I2C2, i2c_addr);
            timeout = 10000;
            while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
            {
                if(--timeout == 0) { I2C_GenerateSTOP(I2C2, ENABLE); return 0; }
            }
            // 发数据（buffer里全是数据，不包括地址）
            for(i = 0; i < buf_idx; i++)
            {
                I2C_SendData(I2C2, buffer[i]);
                timeout = 10000;
                while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
                {
                    if(--timeout == 0) { I2C_GenerateSTOP(I2C2, ENABLE); return 0; }
                }
            }
            I2C_GenerateSTOP(I2C2, ENABLE);
            break;
        default:
            break;
    }
    return 1;
}

// u8g2回调函数：告诉u8g2怎么延时
uint8_t u8x8_gpio_and_delay_ch32(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr)
{
    switch(msg)
    {
        case U8X8_MSG_DELAY_MILLI:
            Delay_Ms(arg_int);
            break;
        case U8X8_MSG_DELAY_10MICRO:
            Delay_Ms(1);
            break;
        default:
            break;
    }
    return 1;
}

// u8g2初始化
// 画像素点辅助函数（每个点2x2像素）
void Pixel(u8g2_t *u8g2, uint8_t x, uint8_t y)
{
    u8g2_DrawBox(u8g2, x*2, y*2, 2, 2);
}

void OLED_Init_u8g2(void)
{
    u8g2_Setup_ssd1306_i2c_128x64_noname_f(
        &u8g2,
        U8G2_R0,
        u8x8_byte_ch32_i2c,
        u8x8_gpio_and_delay_ch32
    );
    u8g2_InitDisplay(&u8g2);
    u8g2_SetPowerSave(&u8g2, 0);
}

// u8g2画表情：极简方块眼风格
void OLED_DrawFace_u8g2(uint8_t level)
{
    u8g2_ClearBuffer(&u8g2);

    if(level == 0)  // 平静：方块眼睛+微笑
    {
        // 左眼（纯白方块，无高光）
        u8g2_DrawBox(&u8g2, 32, 18, 18, 22);
        // 右眼（纯白方块，无高光）
        u8g2_DrawBox(&u8g2, 78, 18, 18, 22);
        // 微笑嘴（平滑弧线，6段近似圆弧）
        u8g2_DrawLine(&u8g2, 52, 52, 56, 56);
        u8g2_DrawLine(&u8g2, 56, 56, 60, 58);
        u8g2_DrawLine(&u8g2, 60, 58, 64, 59);
        u8g2_DrawLine(&u8g2, 64, 59, 68, 58);
        u8g2_DrawLine(&u8g2, 68, 58, 72, 56);
        u8g2_DrawLine(&u8g2, 72, 56, 76, 52);
    }
    else if(level == 1)  // 惊讶：稍大方块眼睛+O嘴
    {
        // 左眼（纯白方块）
        u8g2_DrawBox(&u8g2, 30, 16, 22, 26);
        // 右眼（纯白方块）
        u8g2_DrawBox(&u8g2, 76, 16, 22, 26);
        // O型嘴（放大一点）
        u8g2_DrawFrame(&u8g2, 56, 50, 16, 12);
    }
    else  // 害怕：X形眼睛+大张嘴（尖叫）
    {
        // 左眼（X形，表示紧张害怕）
        u8g2_DrawLine(&u8g2, 28, 18, 50, 42);
        u8g2_DrawLine(&u8g2, 50, 18, 28, 42);
        u8g2_DrawLine(&u8g2, 32, 18, 54, 42);
        u8g2_DrawLine(&u8g2, 54, 18, 32, 42);
        // 右眼（X形）
        u8g2_DrawLine(&u8g2, 74, 18, 96, 42);
        u8g2_DrawLine(&u8g2, 96, 18, 74, 42);
        u8g2_DrawLine(&u8g2, 78, 18, 100, 42);
        u8g2_DrawLine(&u8g2, 100, 18, 78, 42);
        // 大张嘴（尖叫，实心矩形）
        u8g2_DrawBox(&u8g2, 52, 50, 24, 12);
    }
    u8g2_SendBuffer(&u8g2);
}


// ===================== 硬件引脚定义 =====================
#define BEEP_PORT    GPIOA
#define BEEP_PIN     GPIO_Pin_1
// I2C2: ADXL345 + OLED 共用
#define I2C_SCL_PIN  GPIO_Pin_10
#define I2C_SDA_PIN  GPIO_Pin_11
#define ADXL_ADDR    0xA6
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
    while(*str) USART1_SendChar(*str++);
}

void USART1_SendNum(uint32_t num)
{
    char temp[10]; int i = 0;
    if(num == 0) { USART1_SendChar('0'); return; }
    while(num > 0) { temp[i++] = '0' + num%10; num /=10; }
    while(i>0) USART1_SendChar(temp[--i]);
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

// ===================== OLED 标准驱动 =====================
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

// 标准8x16 ASCII字库 (0x20-0x7E)
const uint8_t F8X16[] = {
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,// sp
    0x00,0x00,0x00,0xF8,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x33,0x30,0x00,0x00,0x00,//!
    0x00,0x10,0x0C,0x06,0x10,0x0C,0x06,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,//"
    0x40,0xC0,0x78,0x40,0xC0,0x78,0x40,0x00,0x04,0x3F,0x04,0x04,0x3F,0x04,0x04,0x00,//#
    0x00,0x70,0x88,0xFC,0x08,0x30,0x00,0x00,0x00,0x18,0x20,0xFF,0x21,0x1E,0x00,0x00,//$
    0xF0,0x08,0xF0,0x00,0xF0,0x08,0xF0,0x00,0x00,0x21,0x1C,0x03,0x1E,0x21,0x1E,0x00,//%
    0x00,0xF0,0x08,0x88,0x70,0x00,0x00,0x00,0x1E,0x21,0x23,0x24,0x19,0x27,0x21,0x10,//&
    0x10,0x16,0x0E,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,//'
    0x00,0x00,0x00,0xE0,0x18,0x04,0x02,0x00,0x00,0x00,0x00,0x07,0x18,0x20,0x40,0x00,//(
    0x00,0x02,0x04,0x18,0xE0,0x00,0x00,0x00,0x00,0x40,0x20,0x18,0x07,0x00,0x00,0x00,//)
    0x40,0x40,0x80,0xF0,0x80,0x40,0x40,0x00,0x02,0x02,0x01,0x0F,0x01,0x02,0x02,0x00,//*
    0x00,0x00,0x00,0xF0,0x00,0x00,0x00,0x00,0x01,0x01,0x01,0x1F,0x01,0x01,0x01,0x00,//+
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0xB0,0x70,0x00,0x00,0x00,0x00,0x00,//,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x01,0x01,0x01,0x01,0x01,0x01,//-
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x30,0x30,0x00,0x00,0x00,0x00,0x00,//.
    0x00,0x00,0x02,0x02,0x04,0x04,0x08,0x08,0x00,0x20,0x20,0x10,0x10,0x08,0x08,0x00,///
    0x00,0xE0,0x10,0x08,0x08,0x10,0xE0,0x00,0x00,0x0F,0x10,0x20,0x20,0x10,0x0F,0x00,//0
    0x00,0x10,0x10,0xF8,0x00,0x00,0x00,0x00,0x00,0x20,0x20,0x3F,0x20,0x20,0x00,0x00,//1
    0x00,0x70,0x08,0x08,0x08,0x88,0x70,0x00,0x00,0x30,0x28,0x24,0x22,0x21,0x30,0x00,//2
    0x00,0x30,0x08,0x88,0x88,0x48,0x30,0x00,0x00,0x18,0x20,0x20,0x20,0x11,0x0E,0x00,//3
    0x00,0x00,0xC0,0x20,0x10,0xF8,0x00,0x00,0x00,0x07,0x04,0x24,0x24,0x3F,0x24,0x00,//4
    0x00,0xF8,0x08,0x88,0x88,0x08,0x08,0x00,0x00,0x19,0x21,0x20,0x20,0x11,0x0E,0x00,//5
    0x00,0xE0,0x10,0x88,0x88,0x18,0x00,0x00,0x00,0x0F,0x11,0x20,0x20,0x11,0x0E,0x00,//6
    0x00,0x38,0x08,0x08,0xC8,0x38,0x08,0x00,0x00,0x00,0x00,0x3F,0x00,0x00,0x00,0x00,//7
    0x00,0x70,0x88,0x08,0x08,0x88,0x70,0x00,0x00,0x1C,0x22,0x21,0x21,0x22,0x1C,0x00,//8
    0x00,0xE0,0x10,0x08,0x08,0x10,0xE0,0x00,0x00,0x00,0x31,0x22,0x22,0x11,0x0F,0x00,//9
    0x00,0x00,0x00,0xC0,0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x30,0x30,0x00,0x00,0x00,//:
    0x00,0x00,0x00,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0x60,0x00,0x00,0x00,0x00,//;
    0x00,0x00,0x80,0x40,0x20,0x10,0x08,0x00,0x00,0x01,0x02,0x04,0x08,0x10,0x20,0x00,//<
    0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x00,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x00,//=
    0x00,0x08,0x10,0x20,0x40,0x80,0x00,0x00,0x00,0x20,0x10,0x08,0x04,0x02,0x01,0x00,//>
    0x00,0x70,0x48,0x08,0x08,0x08,0xF0,0x00,0x00,0x00,0x00,0x30,0x36,0x01,0x00,0x00,//?
    0xC0,0x30,0xC8,0x28,0xE8,0x10,0xE0,0x00,0x07,0x18,0x27,0x24,0x23,0x14,0x0B,0x00,//@
    0x00,0x00,0xC0,0x38,0xE0,0x00,0x00,0x00,0x20,0x3C,0x23,0x02,0x02,0x27,0x38,0x20,//A
    0x08,0xF8,0x88,0x88,0x88,0x70,0x00,0x00,0x20,0x3F,0x20,0x20,0x20,0x11,0x0E,0x00,//B
    0xC0,0x30,0x08,0x08,0x08,0x08,0x30,0x00,0x07,0x18,0x20,0x20,0x20,0x10,0x08,0x00,//C
    0x08,0xF8,0x08,0x08,0x08,0x10,0xE0,0x00,0x20,0x3F,0x20,0x20,0x20,0x10,0x0F,0x00,//D
    0x08,0xF8,0x88,0x88,0xE8,0x08,0x10,0x00,0x20,0x3F,0x20,0x20,0x23,0x20,0x18,0x00,//E
    0x08,0xF8,0x88,0x88,0xE8,0x08,0x10,0x00,0x20,0x3F,0x20,0x00,0x03,0x00,0x00,0x00,//F
    0xC0,0x30,0x08,0x08,0x08,0x38,0x00,0x00,0x07,0x18,0x20,0x20,0x22,0x1E,0x02,0x00,//G
    0x08,0xF8,0x08,0x00,0x00,0x08,0xF8,0x08,0x20,0x3F,0x21,0x01,0x01,0x21,0x3F,0x20,//H
    0x00,0x08,0x08,0xF8,0x08,0x08,0x00,0x00,0x00,0x20,0x20,0x3F,0x20,0x20,0x00,0x00,//I
    0x00,0x00,0x08,0x08,0xF8,0x08,0x08,0x00,0xC0,0x80,0x80,0x80,0x7F,0x00,0x00,0x00,//J
    0x08,0xF8,0x88,0xC0,0x28,0x18,0x08,0x00,0x20,0x3F,0x20,0x01,0x26,0x38,0x20,0x00,//K
    0x08,0xF8,0x08,0x00,0x00,0x00,0x00,0x00,0x20,0x3F,0x20,0x20,0x20,0x20,0x30,0x00,//L
    0x08,0xF8,0xF8,0x00,0xF8,0xF8,0x08,0x00,0x20,0x3F,0x00,0x3F,0x00,0x3F,0x20,0x00,//M
    0x08,0xF8,0x30,0xC0,0x00,0x08,0xF8,0x08,0x20,0x3F,0x20,0x00,0x07,0x18,0x3F,0x00,//N
    0xE0,0x10,0x08,0x08,0x08,0x10,0xE0,0x00,0x0F,0x10,0x20,0x20,0x20,0x10,0x0F,0x00,//O
    0x08,0xF8,0x08,0x08,0x08,0x08,0xF0,0x00,0x20,0x3F,0x21,0x01,0x01,0x01,0x00,0x00,//P
    0xE0,0x10,0x08,0x08,0x08,0x10,0xE0,0x00,0x0F,0x10,0x28,0x28,0x30,0x50,0x4F,0x00,//Q
    0x08,0xF8,0x88,0x88,0x88,0x88,0x70,0x00,0x20,0x3F,0x20,0x00,0x03,0x0C,0x30,0x20,//R
    0x00,0x70,0x88,0x08,0x08,0x08,0x38,0x00,0x00,0x38,0x20,0x21,0x21,0x22,0x1C,0x00,//S
    0x18,0x08,0x08,0xF8,0x08,0x08,0x18,0x00,0x00,0x00,0x20,0x3F,0x20,0x00,0x00,0x00,//T
    0x08,0xF8,0x08,0x00,0x00,0x08,0xF8,0x08,0x00,0x1F,0x20,0x20,0x20,0x20,0x1F,0x00,//U
    0x08,0x78,0x88,0x00,0x00,0xC8,0x38,0x08,0x00,0x00,0x07,0x38,0x0E,0x01,0x00,0x00,//V
    0xF8,0x08,0x00,0xF8,0x00,0x08,0xF8,0x00,0x03,0x3C,0x07,0x00,0x07,0x3C,0x03,0x00,//W
    0x08,0x18,0x68,0x80,0x80,0x68,0x18,0x08,0x20,0x30,0x2C,0x03,0x03,0x2C,0x30,0x20,//X
    0x08,0x18,0x08,0x80,0x80,0x08,0x18,0x08,0x00,0x00,0x20,0x3F,0x20,0x00,0x00,0x00,//Y
    0x00,0x08,0x08,0x08,0xC8,0x38,0x08,0x00,0x00,0x20,0x30,0x28,0x24,0x22,0x21,0x20,//Z
    0x00,0x00,0x00,0xFE,0x02,0x02,0x02,0x00,0x00,0x00,0x00,0x7F,0x40,0x40,0x40,0x00,//[
    0x00,0x0C,0x30,0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x06,0x38,0xC0,0x00,//\
    0x00,0x02,0x02,0x02,0xFE,0x00,0x00,0x00,0x00,0x40,0x40,0x40,0x7F,0x00,0x00,0x00,//]
    0x00,0x04,0x02,0x02,0x02,0x02,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,//^
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,//_
    0x00,0x02,0x02,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,//`
    0x00,0x00,0x80,0x80,0x80,0x80,0x00,0x00,0x00,0x19,0x24,0x22,0x22,0x22,0x3F,0x20,//a
    0x08,0xF8,0x00,0x80,0x80,0x00,0x00,0x00,0x00,0x3F,0x11,0x20,0x20,0x11,0x0E,0x00,//b
    0x00,0x00,0x00,0x80,0x80,0x80,0x00,0x00,0x00,0x0E,0x11,0x20,0x20,0x20,0x11,0x00,//c
    0x00,0x00,0x00,0x80,0x80,0x88,0xF8,0x00,0x00,0x0E,0x11,0x20,0x20,0x10,0x3F,0x20,//d
    0x00,0x00,0x80,0x80,0x80,0x80,0x00,0x00,0x00,0x1F,0x22,0x22,0x22,0x22,0x13,0x00,//e
    0x00,0x80,0x80,0xF0,0x88,0x88,0x88,0x18,0x00,0x20,0x20,0x3F,0x20,0x20,0x00,0x00,//f
    0x00,0x00,0x80,0x80,0x80,0x80,0x80,0x00,0x00,0x6B,0x94,0x94,0x94,0x93,0x60,0x00,//g
    0x08,0xF8,0x00,0x80,0x80,0x80,0x00,0x00,0x20,0x3F,0x21,0x01,0x01,0x21,0x1E,0x00,//h
    0x00,0x80,0x98,0x98,0x00,0x00,0x00,0x00,0x00,0x20,0x20,0x3F,0x20,0x20,0x00,0x00,//i
    0x00,0x00,0x00,0x80,0x98,0x98,0x00,0x00,0x00,0xC0,0x80,0x80,0x7F,0x00,0x00,0x00,//j
    0x08,0xF8,0x00,0x00,0x80,0x80,0x80,0x00,0x20,0x3F,0x24,0x02,0x2D,0x30,0x20,0x00,//k
    0x00,0x08,0x08,0xF8,0x00,0x00,0x00,0x00,0x00,0x20,0x20,0x3F,0x20,0x20,0x00,0x00,//l
    0x00,0x00,0x80,0x80,0x80,0x80,0x80,0x00,0x0F,0x30,0x0F,0x30,0x0F,0x30,0x0F,0x00,//m
    0x00,0x80,0x80,0x00,0x80,0x80,0x00,0x00,0x00,0x1F,0x21,0x01,0x01,0x21,0x1E,0x00,//n
    0x00,0x00,0x80,0x80,0x80,0x80,0x00,0x00,0x00,0x1F,0x20,0x20,0x20,0x11,0x0F,0x00,//o
    0x00,0x80,0x80,0x00,0x80,0x80,0x80,0x00,0x80,0xFF,0x91,0x20,0x20,0x11,0x0E,0x00,//p
    0x00,0x00,0x00,0x80,0x80,0x00,0x80,0x00,0x00,0x0E,0x11,0x20,0x20,0x91,0xFF,0x80,//q
    0x00,0x80,0x80,0x80,0x80,0x80,0x80,0x00,0x20,0x21,0x21,0x01,0x01,0x21,0x1E,0x00,//r
    0x00,0x00,0x80,0x80,0x80,0x80,0x80,0x00,0x00,0x33,0x24,0x24,0x24,0x24,0x19,0x00,//s
    0x00,0x80,0x80,0xE0,0x80,0x80,0x00,0x00,0x00,0x00,0x20,0x3F,0x20,0x00,0x00,0x00,//t
    0x00,0x80,0x80,0x00,0x00,0x80,0x80,0x00,0x00,0x1F,0x20,0x20,0x20,0x10,0x3F,0x20,//u
    0x00,0x80,0x80,0x80,0x00,0x80,0x80,0x00,0x00,0x01,0x0E,0x30,0x08,0x06,0x01,0x00,//v
    0x00,0x80,0x80,0x00,0x80,0x80,0x00,0x80,0x0F,0x30,0x0F,0x00,0x0F,0x30,0x0F,0x00,//w
    0x00,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x00,0x20,0x31,0x2E,0x0E,0x31,0x20,0x00,//x
    0x00,0x80,0x80,0x80,0x00,0x80,0x80,0x00,0x80,0x81,0x8E,0x70,0x18,0x06,0x01,0x00,//y
    0x00,0x00,0x80,0x00,0x80,0x80,0x80,0x00,0x00,0x21,0x30,0x2C,0x22,0x21,0x30,0x00,//z
    0x00,0x00,0x00,0x00,0x80,0x7C,0x02,0x02,0x00,0x00,0x00,0x00,0x00,0x3F,0x40,0x40,//{
    0x00,0x00,0x00,0x00,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0x00,0x00,0x00,//|
    0x00,0x02,0x02,0x7C,0x80,0x00,0x00,0x00,0x00,0x40,0x40,0x3F,0x00,0x00,0x00,0x00,//}
    0x00,0x06,0x01,0x01,0x02,0x02,0x04,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,//~
};

void OLED_SetPos(uint8_t x, uint8_t y)
{
    OLED_WriteCmd(0xb0 + y);
    OLED_WriteCmd(((x & 0xf0) >> 4) | 0x10);
    OLED_WriteCmd(x & 0x0f);
}

void OLED_ShowChar(uint8_t x, uint8_t y, uint8_t chr)
{
    uint8_t c = chr - ' ';
    uint8_t i;
    if(c > 95) c = 0;
    OLED_SetPos(x, y);
    for(i=0; i<8; i++) OLED_WriteData(F8X16[c*16 + i]);
    OLED_SetPos(x, y+1);
    for(i=0; i<8; i++) OLED_WriteData(F8X16[c*16 + i + 8]);
}

void OLED_ShowString(uint8_t x, uint8_t y, char *str)
{
    while(*str)
    {
        OLED_ShowChar(x, y, *str++);
        x += 8;
        if(x > 120) { x = 0; y += 2; }
    }
}

void OLED_ShowNum(uint8_t x, uint8_t y, int32_t num)
{
    char buf[12];
    int i = 0;
    if(num < 0) { OLED_ShowChar(x, y, '-'); x += 8; num = -num; }
    if(num == 0) { OLED_ShowChar(x, y, '0'); return; }
    while(num > 0) { buf[i++] = '0' + num%10; num /= 10; }
    while(i > 0) { i--; OLED_ShowChar(x, y, buf[i]); x += 8; }
}

void OLED_Clear(void)
{
    uint8_t x, y;
    for(y=0; y<8; y++)
    {
        OLED_SetPos(0, y);
        for(x=0; x<128; x++) OLED_WriteData(0x00);
    }
}

void OLED_ClearLine(uint8_t y)
{
    uint8_t x;
    OLED_SetPos(0, y);
    for(x=0; x<128; x++) OLED_WriteData(0x00);
    OLED_SetPos(0, y+1);
    for(x=0; x<128; x++) OLED_WriteData(0x00);
}

// 清指定区域
void OLED_ClearArea(uint8_t x0, uint8_t y0, uint8_t w, uint8_t h)
{
    uint8_t i, j;
    for(i=0; i<h; i++)
    {
        OLED_SetPos(x0, y0+i);
        for(j=0; j<w; j++) OLED_WriteData(0x00);
    }
}

void OLED_Init(void)
{
    Delay_Ms(100);
    OLED_WriteCmd(0xAE);
    OLED_WriteCmd(0xD5); OLED_WriteCmd(0x80);
    OLED_WriteCmd(0xA8); OLED_WriteCmd(0x3F);
    OLED_WriteCmd(0xD3); OLED_WriteCmd(0x00);
    OLED_WriteCmd(0x40);
    OLED_WriteCmd(0x8D); OLED_WriteCmd(0x14);
    OLED_WriteCmd(0x20); OLED_WriteCmd(0x02);
    OLED_WriteCmd(0xA1);
    OLED_WriteCmd(0xC8);
    OLED_WriteCmd(0xDA); OLED_WriteCmd(0x12);
    OLED_WriteCmd(0x81); OLED_WriteCmd(0xCF);
    OLED_WriteCmd(0xD9); OLED_WriteCmd(0xF1);
    OLED_WriteCmd(0xDB); OLED_WriteCmd(0x30);
    OLED_WriteCmd(0xA4);
    OLED_WriteCmd(0xA6);
    OLED_WriteCmd(0xAF);
}

// 画一个实心方块
void OLED_DrawBlock(uint8_t x, uint8_t y, uint8_t w, uint8_t h)
{
    uint8_t i, j;
    for(i=0; i<h; i++)
    {
        OLED_SetPos(x, y+i);
        for(j=0; j<w; j++) OLED_WriteData(0xFF);
    }
}

// 画一只带高光的圆眼睛
void OLED_DrawEye(uint8_t x, uint8_t y)
{
    // 圆形眼睛（宽20，高24，3页）
    // 第1页：顶部圆弧
    OLED_DrawBlock(x+2, y, 16, 1);
    // 第2页：最宽
    OLED_DrawBlock(x, y+1, 20, 1);
    // 第3页：底部圆弧
    OLED_DrawBlock(x+2, y+2, 16, 1);
    // 高光（右上角2x2白点，不画就是黑底上的白点）
    OLED_SetPos(x+14, y); OLED_WriteData(0x00);
    OLED_SetPos(x+15, y); OLED_WriteData(0x00);
    OLED_SetPos(x+14, y+1); OLED_WriteData(0x00);
    OLED_SetPos(x+15, y+1); OLED_WriteData(0x00);
}

// 画可爱机器人脸（生动版）
void OLED_DrawFace(uint8_t level)
{
    uint8_t page, i;
    // 先清大区域
    for(page=1; page<8; page++)
    {
        OLED_SetPos(16, page);
        for(i=0; i<96; i++) OLED_WriteData(0x00);
    }
    
    if(level == 0) // 平静：圆眼睛带高光 + 微笑
    {
        // 左眼
        OLED_DrawEye(24, 1);
        // 右眼
        OLED_DrawEye(84, 1);
        // 微笑嘴
        OLED_SetPos(44, 5); OLED_WriteData(0x00);
        OLED_SetPos(45, 5); OLED_WriteData(0x80);
        OLED_SetPos(46, 5); OLED_WriteData(0xC0);
        OLED_SetPos(47, 5); OLED_WriteData(0xE0);
        OLED_SetPos(48, 5); OLED_WriteData(0xF0);
        OLED_SetPos(49, 5); OLED_WriteData(0xF8);
        OLED_SetPos(50, 5); OLED_WriteData(0xFC);
        OLED_SetPos(51, 5); OLED_WriteData(0xFE);
        OLED_SetPos(52, 5); OLED_WriteData(0xFE);
        OLED_SetPos(53, 5); OLED_WriteData(0xFC);
        OLED_SetPos(54, 5); OLED_WriteData(0xF8);
        OLED_SetPos(55, 5); OLED_WriteData(0xF0);
        OLED_SetPos(56, 5); OLED_WriteData(0xE0);
        OLED_SetPos(57, 5); OLED_WriteData(0xC0);
        OLED_SetPos(58, 5); OLED_WriteData(0x80);
        OLED_SetPos(59, 5); OLED_WriteData(0x00);
        OLED_SetPos(44, 6); OLED_WriteData(0x00);
        OLED_SetPos(45, 6); OLED_WriteData(0x00);
        OLED_SetPos(46, 6); OLED_WriteData(0x01);
        OLED_SetPos(47, 6); OLED_WriteData(0x03);
        OLED_SetPos(48, 6); OLED_WriteData(0x07);
        OLED_SetPos(49, 6); OLED_WriteData(0x0F);
        OLED_SetPos(50, 6); OLED_WriteData(0x1F);
        OLED_SetPos(51, 6); OLED_WriteData(0x3F);
        OLED_SetPos(52, 6); OLED_WriteData(0x3F);
        OLED_SetPos(53, 6); OLED_WriteData(0x1F);
        OLED_SetPos(54, 6); OLED_WriteData(0x0F);
        OLED_SetPos(55, 6); OLED_WriteData(0x07);
        OLED_SetPos(56, 6); OLED_WriteData(0x03);
        OLED_SetPos(57, 6); OLED_WriteData(0x01);
        OLED_SetPos(58, 6); OLED_WriteData(0x00);
        OLED_SetPos(59, 6); OLED_WriteData(0x00);
    }
    else if(level == 1) // 惊讶：大圆眼睛 + O型嘴
    {
        // 左眼（更大的圆）
        OLED_DrawBlock(26, 1, 16, 1);
        OLED_DrawBlock(22, 2, 24, 2);
        OLED_DrawBlock(26, 4, 16, 1);
        // 高光
        OLED_SetPos(36, 1); OLED_WriteData(0x00);
        OLED_SetPos(37, 1); OLED_WriteData(0x00);
        OLED_SetPos(36, 2); OLED_WriteData(0x00);
        OLED_SetPos(37, 2); OLED_WriteData(0x00);
        // 右眼
        OLED_DrawBlock(86, 1, 16, 1);
        OLED_DrawBlock(82, 2, 24, 2);
        OLED_DrawBlock(86, 4, 16, 1);
        // 高光
        OLED_SetPos(96, 1); OLED_WriteData(0x00);
        OLED_SetPos(97, 1); OLED_WriteData(0x00);
        OLED_SetPos(96, 2); OLED_WriteData(0x00);
        OLED_SetPos(97, 2); OLED_WriteData(0x00);
        // O型嘴
        OLED_DrawBlock(50, 5, 8, 1);
        OLED_DrawBlock(46, 6, 16, 1);
        OLED_DrawBlock(50, 7, 8, 1);
    }
    else // 害怕：X眼睛 + 大嘴
    {
        // 左眼X
        OLED_SetPos(24, 1); OLED_WriteData(0x81);
        OLED_SetPos(25, 1); OLED_WriteData(0x42);
        OLED_SetPos(26, 1); OLED_WriteData(0x24);
        OLED_SetPos(27, 1); OLED_WriteData(0x18);
        OLED_SetPos(28, 1); OLED_WriteData(0x18);
        OLED_SetPos(29, 1); OLED_WriteData(0x24);
        OLED_SetPos(30, 1); OLED_WriteData(0x42);
        OLED_SetPos(31, 1); OLED_WriteData(0x81);
        OLED_SetPos(32, 1); OLED_WriteData(0x81);
        OLED_SetPos(33, 1); OLED_WriteData(0x42);
        OLED_SetPos(34, 1); OLED_WriteData(0x24);
        OLED_SetPos(35, 1); OLED_WriteData(0x18);
        OLED_SetPos(36, 1); OLED_WriteData(0x18);
        OLED_SetPos(37, 1); OLED_WriteData(0x24);
        OLED_SetPos(38, 1); OLED_WriteData(0x42);
        OLED_SetPos(39, 1); OLED_WriteData(0x81);
        OLED_SetPos(24, 2); OLED_WriteData(0x42);
        OLED_SetPos(25, 2); OLED_WriteData(0x81);
        OLED_SetPos(26, 2); OLED_WriteData(0x81);
        OLED_SetPos(27, 2); OLED_WriteData(0x42);
        OLED_SetPos(28, 2); OLED_WriteData(0x42);
        OLED_SetPos(29, 2); OLED_WriteData(0x81);
        OLED_SetPos(30, 2); OLED_WriteData(0x81);
        OLED_SetPos(31, 2); OLED_WriteData(0x42);
        OLED_SetPos(32, 2); OLED_WriteData(0x42);
        OLED_SetPos(33, 2); OLED_WriteData(0x81);
        OLED_SetPos(34, 2); OLED_WriteData(0x81);
        OLED_SetPos(35, 2); OLED_WriteData(0x42);
        OLED_SetPos(36, 2); OLED_WriteData(0x42);
        OLED_SetPos(37, 2); OLED_WriteData(0x81);
        OLED_SetPos(38, 2); OLED_WriteData(0x81);
        OLED_SetPos(39, 2); OLED_WriteData(0x42);
        OLED_SetPos(24, 3); OLED_WriteData(0x24);
        OLED_SetPos(25, 3); OLED_WriteData(0x42);
        OLED_SetPos(26, 3); OLED_WriteData(0x81);
        OLED_SetPos(27, 3); OLED_WriteData(0x81);
        OLED_SetPos(28, 3); OLED_WriteData(0x81);
        OLED_SetPos(29, 3); OLED_WriteData(0x81);
        OLED_SetPos(30, 3); OLED_WriteData(0x42);
        OLED_SetPos(31, 3); OLED_WriteData(0x24);
        OLED_SetPos(32, 3); OLED_WriteData(0x24);
        OLED_SetPos(33, 3); OLED_WriteData(0x42);
        OLED_SetPos(34, 3); OLED_WriteData(0x81);
        OLED_SetPos(35, 3); OLED_WriteData(0x81);
        OLED_SetPos(36, 3); OLED_WriteData(0x81);
        OLED_SetPos(37, 3); OLED_WriteData(0x81);
        OLED_SetPos(38, 3); OLED_WriteData(0x42);
        OLED_SetPos(39, 3); OLED_WriteData(0x24);
        // 右眼X
        OLED_SetPos(80, 1); OLED_WriteData(0x81);
        OLED_SetPos(81, 1); OLED_WriteData(0x42);
        OLED_SetPos(82, 1); OLED_WriteData(0x24);
        OLED_SetPos(83, 1); OLED_WriteData(0x18);
        OLED_SetPos(84, 1); OLED_WriteData(0x18);
        OLED_SetPos(85, 1); OLED_WriteData(0x24);
        OLED_SetPos(86, 1); OLED_WriteData(0x42);
        OLED_SetPos(87, 1); OLED_WriteData(0x81);
        OLED_SetPos(88, 1); OLED_WriteData(0x81);
        OLED_SetPos(89, 1); OLED_WriteData(0x42);
        OLED_SetPos(90, 1); OLED_WriteData(0x24);
        OLED_SetPos(91, 1); OLED_WriteData(0x18);
        OLED_SetPos(92, 1); OLED_WriteData(0x18);
        OLED_SetPos(93, 1); OLED_WriteData(0x24);
        OLED_SetPos(94, 1); OLED_WriteData(0x42);
        OLED_SetPos(95, 1); OLED_WriteData(0x81);
        OLED_SetPos(80, 2); OLED_WriteData(0x42);
        OLED_SetPos(81, 2); OLED_WriteData(0x81);
        OLED_SetPos(82, 2); OLED_WriteData(0x81);
        OLED_SetPos(83, 2); OLED_WriteData(0x42);
        OLED_SetPos(84, 2); OLED_WriteData(0x42);
        OLED_SetPos(85, 2); OLED_WriteData(0x81);
        OLED_SetPos(86, 2); OLED_WriteData(0x81);
        OLED_SetPos(87, 2); OLED_WriteData(0x42);
        OLED_SetPos(88, 2); OLED_WriteData(0x42);
        OLED_SetPos(89, 2); OLED_WriteData(0x81);
        OLED_SetPos(90, 2); OLED_WriteData(0x81);
        OLED_SetPos(91, 2); OLED_WriteData(0x42);
        OLED_SetPos(92, 2); OLED_WriteData(0x42);
        OLED_SetPos(93, 2); OLED_WriteData(0x81);
        OLED_SetPos(94, 2); OLED_WriteData(0x81);
        OLED_SetPos(95, 2); OLED_WriteData(0x42);
        OLED_SetPos(80, 3); OLED_WriteData(0x24);
        OLED_SetPos(81, 3); OLED_WriteData(0x42);
        OLED_SetPos(82, 3); OLED_WriteData(0x81);
        OLED_SetPos(83, 3); OLED_WriteData(0x81);
        OLED_SetPos(84, 3); OLED_WriteData(0x81);
        OLED_SetPos(85, 3); OLED_WriteData(0x81);
        OLED_SetPos(86, 3); OLED_WriteData(0x42);
        OLED_SetPos(87, 3); OLED_WriteData(0x24);
        OLED_SetPos(88, 3); OLED_WriteData(0x24);
        OLED_SetPos(89, 3); OLED_WriteData(0x42);
        OLED_SetPos(90, 3); OLED_WriteData(0x81);
        OLED_SetPos(91, 3); OLED_WriteData(0x81);
        OLED_SetPos(92, 3); OLED_WriteData(0x81);
        OLED_SetPos(93, 3); OLED_WriteData(0x81);
        OLED_SetPos(94, 3); OLED_WriteData(0x42);
        OLED_SetPos(95, 3); OLED_WriteData(0x24);
        // 大张嘴
        OLED_DrawBlock(46, 5, 12, 1);
        OLED_DrawBlock(42, 6, 20, 1);
        OLED_DrawBlock(46, 7, 12, 1);
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

// 震动等级阈值（可根据实际情况调整）
#define THRESHOLD_LIGHT   30    // 轻微震动
#define THRESHOLD_THEFT   100   // 防盗报警
#define THRESHOLD_QUAKE   500   // 地震预警

// ===================== main 函数 =====================
int main(void)
{
    int16_t ax, ay, az;
    int16_t last_ax=0, last_ay=0, last_az=0;
    static uint8_t blink = 0;
    uint8_t level = 0; // 0=安全 1=轻微 2=防盗 3=地震
    uint8_t raw_level = 0; // 原始检测等级（未经过持续判断）
    int diff, max_val;
    int diff_filtered; // 滤波后的震动强度
    // 滑动平均滤波
    int diff_history[5] = {0,0,0,0,0};
    uint8_t hist_idx = 0;
    // 持续震动判断计数器
    uint8_t stable_count = 0;
    uint8_t shake_duration = 0;  // 震动持续时间（单位：循环次数，5次=1秒）

    Delay_Init();
    USART1_Init();
    BEEP_Init();

    USART1_SendString("=== System Start ===\r\n");
    USART1_SendString("[1/4] Init I2C...\r\n");
    I2C2_Init();
    Delay_Ms(100);

    USART1_SendString("[2/4] Init ADXL...\r\n");
    if(ADXL345_Init() != 0) USART1_SendString("[WARN] ADXL Fail\r\n");
    else USART1_SendString("[OK] ADXL Ready\r\n");

    USART1_SendString("[3/4] Init OLED...\r\n");
    // 先测试I2C能不能通：读OLED的一个寄存器
    {
        uint8_t i2c_ok = 1;
        // 发一个空命令测试OLED是否响应
        I2C_GenerateSTART(I2C2, ENABLE);
        volatile uint32_t t = 10000;
        while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT)) { if(--t==0){i2c_ok=0;break;} }
        if(i2c_ok)
        {
            I2C_SendData(I2C2, 0x78);
            t = 10000;
            while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) { if(--t==0){i2c_ok=0;break;} }
        }
        I2C_GenerateSTOP(I2C2, ENABLE);
        if(i2c_ok) USART1_SendString("[DEBUG] OLED I2C ACK OK\r\n");
        else USART1_SendString("[DEBUG] OLED I2C ACK FAIL\r\n");
    }
    OLED_Init_u8g2();
    USART1_SendString("[OK] OLED Ready\r\n");
    // 初始化后清屏
    u8g2_ClearBuffer(&u8g2);
    u8g2_SendBuffer(&u8g2);
    USART1_SendString("[DEBUG] OLED cleared\r\n");

    USART1_SendString("[4/4] BEEP Test\r\n");
    BEEP_On(); Delay_Ms(1000); BEEP_Off();
    USART1_SendString("=== Ready ===\r\n\r\n");

    // OLED初始化清屏

    while(1)
    {
        ADXL345_Read(&ax, &ay, &az);
        blink++;

        // 计算震动强度
        diff = abs(ax - last_ax) + abs(ay - last_ay) + abs(az - last_az);
        max_val = abs(ax);
        if(abs(ay) > max_val) max_val = abs(ay);
        if(abs(az) > max_val) max_val = abs(az);

        // 滑动平均滤波（5次平均，让数据更稳定，减少误报）
        diff_history[hist_idx] = diff;
        hist_idx++;
        if(hist_idx >= 5) hist_idx = 0;
        diff_filtered = (diff_history[0] + diff_history[1] + diff_history[2] + diff_history[3] + diff_history[4]) / 5;

        // 用滤波后的数据判断原始等级
        if(diff_filtered >= THRESHOLD_QUAKE) raw_level = 3;
        else if(diff_filtered >= THRESHOLD_THEFT) raw_level = 2;
        else if(diff_filtered >= THRESHOLD_LIGHT) raw_level = 1;
        else raw_level = 0;

        // 震动持续时间判断
        if(raw_level >= 1) shake_duration++;  // 有震动，持续时间+1
        else shake_duration = 0;               // 没震动，清零

        // 持续震动判断：只有连续3次检测到防盗/地震才真正报警，减少误报
        if(raw_level >= 2)
        {
            if(stable_count < 5) stable_count++;
            if(stable_count >= 3)
            {
                // 持续时间判断：震动超过1秒（5次循环）才可能是地震
                if(shake_duration >= 5 && raw_level >= 2)
                {
                    level = 3;  // 长时间震动 + 强度够 = 地震
                }
                else
                {
                    level = 2;  // 短时间震动 = 防盗/人为触碰
                }
            }
            else
            {
                level = 1; // 偶尔震动，只显示轻微，不报警
            }
        }
        else
        {
            stable_count = 0; // 没有持续震动，直接清零，不让噪声累积
            level = raw_level; // 轻微或安全，直接显示
        }

        // 串口输出
        USART1_SendString("X:"); USART1_SendNum(ax);
        USART1_SendString(" Y:"); USART1_SendNum(ay);
        USART1_SendString(" Z:"); USART1_SendNum(az);
        USART1_SendString(" Diff:"); USART1_SendNum(diff_filtered);
        if(level == 3) USART1_SendString(" [QUAKE!]");
        else if(level == 2) USART1_SendString(" [THEFT!]");
        else if(level == 1) USART1_SendString(" [LIGHT]");
        else USART1_SendString(" [SAFE]");
        USART1_SendString("\r\n");

        // 蜂鸣器控制：不同等级不同模式
        if(level == 3)
        {
            // 地震：持续长响
            BEEP_On();
        }
        else if(level == 2)
        {
            // 防盗：间歇响（滴-滴-滴）
            if(blink % 2 == 0) BEEP_On();
            else BEEP_Off();
        }
        else
        {
            // 轻微和安全：不响
            BEEP_Off();
        }

        // OLED显示：u8g2大表情
        if(level >= 2 && blink % 2 != 0)
        {
            // 报警时闪烁：清屏
            u8g2_ClearBuffer(&u8g2);
            u8g2_SendBuffer(&u8g2);
        }
        else
        {
            OLED_DrawFace_u8g2(level);
        }

        last_ax = ax; last_ay = ay; last_az = az;
        Delay_Ms(200);
    }
}
