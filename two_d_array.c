#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>

// 二维数组练习：3x3矩阵打印
int main()
{
    int arr[3][3] = {
        {1, 2, 3},
        {4, 5, 6},
        {7, 8, 9}
    };

    printf("3x3矩阵：\n");
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            printf("%d ", arr[i][j]);
        }
        printf("\n");
    }

    printf("\n对角线元素：\n");
    for (int i = 0; i < 3; i++)
    {
        printf("%d ", arr[i][i]);
    }
    printf("\n");
    return 0;
}
