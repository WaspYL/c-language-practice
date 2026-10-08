#include "ch32v30x.h"

// ===== 串口1发送一个字符 =====
void USART1_SendChar(char ch)
{
    while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);  // 等上一个发完
    USART_SendData(USART1, ch);                                   // 发这个字符
}

// ===== 串口1发送一串文字 =====
void USART1_SendString(char *str)
{
    while(*str != '\0')   // 遇到字符串结束符就停
    {
        USART1_SendChar(*str);  // 一个字一个字发
        str++;                  // 指向下一个字
    }
}

// ===== 串口1发送数字 =====
void USART1_SendNum(uint32_t num)
{
    char temp[10];
    int i = 0;

    if(num == 0)
    {
        USART1_SendChar('0');
        return;
    }

    // 把数字拆成一位一位，倒着存
    while(num > 0)
    {
        temp[i] = '0' + (num % 10);
        num = num / 10;
        i++;
    }

    // 倒着发出来，就是正序的数字
    while(i > 0)
    {
        i--;
        USART1_SendChar(temp[i]);
    }
}

// ===== 初始化USART1串口 =====
void USART1_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    USART_InitTypeDef USART_InitStruct = {0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

    // PA9 = TX（发送），复用推挽输出
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    // 串口参数：115200波特率，8位数据，1停止，无校验
    USART_InitStruct.USART_BaudRate = 115200;
    USART_InitStruct.USART_WordLength = USART_WordLength_8b;
    USART_InitStruct.USART_StopBits = USART_StopBits_1;
    USART_InitStruct.USART_Parity = USART_Parity_No;
    USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStruct.USART_Mode = USART_Mode_Tx;
    USART_Init(USART1, &USART_InitStruct);

    USART_Cmd(USART1, ENABLE);
}

int main(void)
{
    Delay_Init();
    USART1_Init();

    USART1_SendString("====================\r\n");
    USART1_SendString("  Hello CH32V307!\r\n");
    USART1_SendString("  串口启动成功！\r\n");
    USART1_SendString("====================\r\n");

    uint32_t count = 0;
    while(1)
    {
        USART1_SendString("count = ");
        USART1_SendNum(count);
        USART1_SendString("\r\n");
        count++;
        Delay_Ms(1000);
    }
}
