#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>

int main()
{
    printf("Hello, VS Code!\n");
    printf("你的C语言环境配置成功了！\n");

    int arr[] = {1, 2, 3, 4, 5};
    for (int i = 0; i < 5; i++)
    {
        printf("arr[%d] = %d\n", i, arr[i]);
    }
    return 0;
}
