#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>

// 循环版本斐波那契数列（效率高，推荐）
// 第1、2项都是1，从第3项开始每一项 = 前两项之和
int fib(int n)
{
    if (n <= 2)
        return 1;

    int a = 1, b = 1, c;
    for (int i = 3; i <= n; i++)
    {
        c = a + b;
        a = b;
        b = c;
    }
    return b;
}

int main()
{
    int n;
    printf("请输入求斐波那契数列的第N项: ");
    while (scanf("%d", &n) != EOF)
    {
        printf("第%d项 = %d\n", n, fib(n));
        printf("请继续输入（Ctrl+C退出）: ");
    }
    return 0;
}
