#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>

// 指针基础练习
void swap(int* a, int* b)
{
    int t = *a;
    *a = *b;
    *b = t;
}

int main()
{
    int arr[5] = {10, 20, 30, 40, 50};
    int* p = arr;  // p指向数组第一个元素

    printf("用指针遍历数组：\n");
    for (int i = 0; i < 5; i++)
    {
        printf("arr[%d] = %d (地址: %p)\n", i, *(p + i), p + i);
    }

    int x = 100, y = 200;
    printf("\n交换前: x=%d, y=%d\n", x, y);
    swap(&x, &y);
    printf("交换后: x=%d, y=%d\n", x, y);

    return 0;
}
