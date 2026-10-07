#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
// //指针+-整
// void print_arr(int* p, int len)
// {
// 	for (int i = 0;i < len;i++)
// 	{
// 		printf("%d ", *(p + i));
// 	}
// }
// int main()
// {
// 	int arr[] = { 1,2,3,4,5 };
// 	int len = sizeof(arr) / sizeof(arr[0]);
// 	print_arr(arr, len);
// 	return 0;
// }

////循环求菲波那切数列的第N项
//int pib(int n)
//{;
//	int f1 = 1;
//	int f2 = 1;
//	int f3=1;
//	for (int i = 3;i <= n;i++)
//	{
//		f3 = f1 + f2;
//		f1 = f2;
//		f2 = f3;
//	}
//	return f3;
//}
//int main()
//{
//	int n;
//	printf("请输入循环求菲波那切数列的第N项\n");
//	while (scanf("%d", &n) != EOF)
//	{
//		printf("循环求菲波那切数列的第%d项为:%d\n", n, pib(n));
//	}
//	printf("\n");
//	return 0;
//}

// void prt(int arr[],int n)
// {
// 	printf("顺序打印:\n");
// 	for (int i = 0;i < n;i++)
// 	{
// 		printf("%d ", arr[i]);
// 	}
	
	
// }
// int main()
// {
// 	int n;
// 	printf("请输入数组大小:\n");
// 	while (scanf("%d", &n) != EOF)
// 	{
// 		int arr[100];
// 		printf("请输入数组内容:\n");
// 		for (int i = 0;i < n;i++)
// 		{
// 			scanf("%d", &arr[i]);
// 		}
// 		prt(arr,n);
// 	}

// 	printf("\n");
// 	return 0;
// }





// 递归求菲波那切数列的第N项
//int fib(int n)
//{
//	if (n <= 2)
//	{
//		return 1;
//	}
//	else
//	{
//		return fib(n - 1) + fib(n - 2);
//	}
//}
//int main()
//{
//	int n;
//	printf("请输递归求菲波那切数列的第N项:\n");
//	while (scanf("%d", &n) != EOF)
//	{
//		printf(" 递归求菲波那切数列的第%d项为:%d\n", n, fib(n));
//
//	}
//	printf("\n");
//	return 0;
//}


////顺序打印⼀个整数的每⼀位
//void fac(int n)
//{
//	if (n == 0)
//		return;
//		fac(n/ 10);
//		printf("%d ",n % 10);
//}
//int main()
//{
//	int n;
//	printf("请输入你要单打印的数值:\n");
//	while (scanf("%d", &n) != EOF)
//	{
//		fac(n);
//	}
//	printf("\n");
//	return 0;
//}





//递归求1+2+3+4+....+N的和
//int fac(int n)
//{
//	if (n <= 1)
//	{
//		return 1;
//	}
//	else
//	{
//		return n + fac(n - 1);
//	}
//}
//int main()
//{
//	int n;
//	printf("请输入求的递归求和初始值:\n");
//	while (scanf("%d", &n) != EOF)
//	{
//		int ret = fac(n);
//		printf("%d的递归求和为:%d\n", n, ret);
//	}
//	printf("\n");
//	return 0;
//}



//递归
//int fac(int n)
//{
//	if (n == 0)
//	{
//		return 1;
//	}
//	else
//	{
//		return n * fac(n - 1);
//	}
//}
//int main()
//{
//	int n;
//	printf("请输入求阶乘值\n");
//	while (scanf("%d", &n) != EOF)
//	{
//		int ret = fac(n);
//		printf("%d的阶乘:%d\n", n, ret);
//	}
//	printf("\n");
//	return 0;
//}






//int main()
//{
//	for (int a = 0;a < 10;a++)
//	{
//		if (a == 5)
//		{
//			printf("hhh\n");
//		}
//		else if (a == 7)
//		{
//			printf("111\n");
//		}
//	}
//	return 0;
//}

////static 修饰局部变量
//void test()
//{
//	//static修饰局部变量
//
//	static int i=0;
//	i++;
//	printf("%d ", i);
//}
//int main()
//{
//	int i = 0;
//	for (i = 0; i < 5; i++)
//	{
//		test();
//	}
//	return 0;
//}






////数组做函数参数
//void set_arr(int arr[], int n)
//{
//	for (int i = 0;i < n;i++)
//	{
//		arr[i] = -1;
//	}
//}
//void print_arr(int arr[], int n)
//{
//	for (int i = 0;i < n;i++)
//	{
//		printf("%d ", arr[i]);
//	}
//}
//int main()
//{
//	int arr[] = { 1,2,3,4,5,6,7,8,9,10 };
//	int n = sizeof(arr) / sizeof(arr[0]);
//	printf("原始数组:\n");
//	print_arr(arr, n);
//	printf("\n");
//	printf("改为-1 \n");
//	set_arr(arr,n);
//	print_arr(arr, n);
//	return 0;
//}


//链式访问
//int fac(int n)
//{
//	int ret = 1;
//	for (int i = 1;i <= n;i++)
//	{
//		ret *= i;
//	}
//	return ret;
//}
//int pow(int n)
//{
//	int ret;
//	ret = n * n;
//	return ret;
//}
//int main()
//{
//	int n;
//	printf("请输入求的阶乘，进行求其平方:\n");
//	while (scanf("%d", &n) != EOF)
//	{
//		printf("%d的结果是%d\n", n, pow(fac(n)));
//	}
//	printf("\n");
//	return 0;
//}


//阶乘求和//嵌套调⽤
//int fac(int n)//阶乘
//{
//	int ret = 1;
//	for (int i = 1;i <= n;i++)
//	{
//		ret *= i;
//	}
//	return ret;
//}
//int add(int n)//求和
//{
//	int ret = 0;
//	for (int i = 1;i <= n;i++)
//	{
//		ret += fac(i);
//	}
//	return ret;
//}
//int main()
//{
//	int n;
//	while (scanf("%d",&n) != EOF)
//	{
//		printf("%d的阶乘求和=%d\n", n, add(n));
//	}
//	printf("\n");
//	return 0;
//}


//void spew(int *x, int *y)//交换，带指针
//{
//	int t = *x;
//	*x = *y;
//	*y = t;
//}
//int main()
//{
//	int x, y;
//	printf("原来的:\n");
//	while (scanf("%d%d",& x, &y) != EOF)
//	{
//		printf("x=%d y=%d\n", x, y);
//		printf("颠倒后:\n");
//		spew(&x, &y);
//		printf("x=%d y=%d\n", x, y);
//	}
//	return 0;
//
//}


//int fac(int n)//阶乘
//{
//	int ret = 1;
//	for (int i=1;i <= n;i++)
//	{
//		ret *= i;
//	}
//	return ret;
//}
//int main()
//	{
//		long long int n;
//		printf("请输入数字:\n");
//		while (scanf("%lld", &n) != EOF)
//		{
//			printf("%lld\n", fac(n));
//			printf("完成\n");
//		}
//		return 0;
//	}



//long long int 和(long long int a, long long int b)
//	{
//		return a + b;
//	}
//	int main()
//	{
//		long long int a,b;
//		printf("请输入数字:\n");
//		while (scanf("%lld%lld", &a,&b) != EOF)
//		{
//			printf("%lld\n", 和(a, b));
//			printf("完成\n");
//		}
//		return 0;
//	}


//int main()
//{
//	double a = sqrt(1024);
//	printf("%f ", a);
//	return 0;
//}


////输入数组大小和值
//int main()
//{
//	int n = 0;
//	scanf("%d", &n);//根据输入数值确定数组的大小int arr[n];
//	int arr[n];
//	int i = 0;
//	for (i = 0; i < n; i++)
//	{
//		scanf("%d", &arr[i]);
//	}
//	for (i = 0; i < n; i++)
//	{
//		printf("%d ", arr[i]);
//	}
//	return 0;
//}



//int main()
//{
//	int a[3][3] = { {1,2,3 },{4,5,6},{7,8,9} };
//	printf("初始数组:\n");
//	for (int i = 0;i < 3;i++)
//	{
//		for (int j = 0;j < 3;j++)
//		{
//			printf("%d ", a[i][j]);
//		}
//		printf("\n");
//	}
//		printf("要求数组:\n");
//		for (int i = 0;i < 3;i++)
//		{
//			for (int j = i + 1;j < 3;j++)
//			{
//				int b = a[i][j];
//				a[i][j] = a[j][i];
//				a[j][i] = b;
//			}
//		}
//		for (int i = 0;i < 3;i++)
//		{
//			for (int j = 0;j < 3;j++)
//			{
//				printf("%d ", a[i][j]);
//			}
//			printf("\n");
//		}
//		return 0;
//}


//int main()
//{
//	int a[2][3];
//	for (int i = 0;i < 2;i++)
//	{
//		for (int j = 0;j < 3;j++)
//		{
//			scanf("%d", &a[i][j]);
//		}
//	}
//	for (int i = 0;i < 2;i++)
//	{
//		for (int j = 0;j < 3;j++)
//		{
//			printf("%d ", a[i][j]);
//		}
//		printf("\n");
//	}
//	return 0;
//}



//int main()
//{
//	int a[2][3] = { 1,2,3,4,5,6 };
//	for (int i = 0;i<2;i++)
//	{
//		for (int j = 0;j < 3;j++)
//		{
//			printf("%d \n", a[i][j]);
//		}
//	}
//	return 0;
//}



//int main()
//{
//    int a[] = { 1,2,3,4,5,6,7,8,9,10 };
//    int key;
//    while (1)
//    {
//        printf("请输入查询数：");
//        int ret = scanf("%d", &key);
//        if (ret != 1)
//        {
//            // 清除缓冲区垃圾字符
//            while (getchar() != '\n');
//            printf("输入不是数字！重新输入\n");
//            continue;
//        }
//        if (key == -1)
//        {
//            printf("程序结束\n");
//            break;
//        }
//
//        int left = 0;
//        int right = sizeof(a) / sizeof(a[0]) - 1;
//        int mid = 0;
//        int find = 0;
//
//        while (left <= right)
//        {
//            mid = (left + right) / 2;
//            if (a[mid] > key)
//            {
//                right = mid - 1;
//            }
//            else if (a[mid] < key)
//            {
//                left = mid + 1;
//            }
//            else
//            {
//                find = 1;
//                break;
//            }
//        }
//
//        if (1 == find)
//        {
//            printf("找到了,下标是%d\n", mid);
//        }
//        else
//        {
//            printf("没有\n");
//        }
//        printf("\n");
//    }
//    return 0;
//}


//int main()
//{
//	int a[] = { 1,2,3,4,5,6,7,8,9,10 };
//	int key;
//	printf("请输入查询数:\n");
//	while (scanf("%d", &key) != EOF)
//	{
//		int left = 0;
//		int right = sizeof(a) / sizeof(a[0]) - 1;
//		int mid = 0;
//		int find = 0;
//		while (left <= right)
//		{
//			mid = (left + right) / 2;
//			if (a[mid] > key)
//			{
//				right = mid - 1;
//			}
//			else if (a[mid] < key)
//			{
//				left = mid + 1;
//			}
//			else
//			{
//				find = 1;
//				break;
//			}
//		}
//		if (1 == find)
//		{
//			printf("找到了,下标是%d\n", mid);
//		}
//		else
//		{
//			printf("没有\n");
//		}
//	}
//
//	printf("\n");
//	return 0;
//}



//int main()
//{
//	int a[] = { 1,2,3,4,5,6,7,8,9,10 };
//	int b = sizeof(a) / sizeof(a[0]);
//	int i = 0;
//	for (i;i < b;i++)
//	{
//		a[i] = a[i] * 2;
//		printf("%d ", a[i]);
//	}
//	printf("\n");
//	return 0;
// }






//int main()
//{
//	int a[] = { 1,2,3,4,5,6,7,8,9,10 };
//	int b = sizeof(a) / sizeof(a[0]);
//	int i = 0;
//	int j = b - 1;
//	while (i < j)
//	{
//		int c = a[i];
//		a[i] = a[j];
//		a[j] = c;
//		i++;
//		j--;
//	}
//	for (int i = 0;i < b;i++)
//	{
//		printf("%d ", a[i]);
//	}
//	printf("\n");
//	return 0;
//}













//int main()
//{
//	int a[] = { 1,2,3,4,5,6,7,8,9,10 };
//	int b = sizeof(a) / sizeof(a[0]);
//	int i = 0;
//	int j = b - 1;
//	while (i < j)
//	{
//		int c = a[i];
//		a[i] = a[j];
//		a[j] = c;
//		i++;
//		j--;
//	}
//	for (int i = 0;i < b;i++)
//	{
//		printf("%d ",a[i]);
//	}
//	printf("\n");
//	return 0;
//}









//int main()
//{
//	int a[] = { 1,2,3,4,5 };
//	printf("%d\n", sizeof(a)/sizeof(a[1]));
//	return 0;
//}


//int main()
//{
//	long long int a[] = { 1,2,3,4,5 };
//	for (int i = 0;i < 6;i++)
//	{
//		printf("%d是%d\n", i, &a[i]);
//	}
//	return 0;
//}
















//int main()
//{
//	int a[] = { 1,2,3,4,5 };
//	for (int i = 0;i < 5;i++)
//	{
//		
//		printf("%d\n", a[i]);
//	}
//	return 0;
//}


			


























//int main()
//{
//	int a[] = { 1,2,3,4,5,6,7,8,9,10 };
//	printf("%d\n", a[1]);
//	printf("%d\n", a[2]);
//	printf("%d\n", a[3]);
//	printf("%d\n", a[4]);
//	printf("%d\n", a[5]);
//	printf("%d\n", a[6]);
//	printf("%d\n", a[7]);
//	return 0;
//}







//int main()
//{
//	int a[10] = { 1,2,3,4,5,6,7,8,9,10 };
//	int b = a[2];
//	printf("%d\n", b);
//	return 0;
//}














//int  main()
//{
//	for (int a = 1;a <= 100;a++)
//	{
//		if (a % 3 != 0)
//		{
//			continue;
//		}
//		printf("%d\n", a);
//	}
//	return 0;
//}








//int main()
//{
//	for (int a = 1;a <= 10;a ++ )
//	{
//		if (a == 5)
//		{
//			continue;
//		}
//		printf("%d  ", a);
//	}
//	return 0;
//}




























//int main()
//{
//	int a= 1;
//	while (a <= 100)
//	{
//		if (a % 3 == 0)
//		{
//			printf("%d", a);
//			break;
//		}
//		
//		a++;
//	}
//	return 0;
//}


























//int main()
//{
//	int a = 1;
//	while (a < 10)
//	{
//		printf("%d\n", a);
//		a++;
//		if (a >= 5)
//		{
//			break;
//		}
//		
//	}
//	return 0;
//}
















//int main()
//{
//	int i,j;
//	printf("九九乘法表\n");
//	for (i=1;i <=9;i++)
//	{
//		for (j=1;j <= i;j++)
//		{
//			printf("%d * %d = %2d   ", j, i, i*j  );
//		}
//		printf("\n");
//	}
//	return 0;
//}




















//int main()
//{
//	int a = 0;
//	do
//	{
//		printf("%d\n", a);
//		a++;
//	} while (a <= 10);
//	return 0;
//}
























//int main()
//{
//    int a;
//    while (scanf("%d", &a) != EOF)
//    {
//        int b = 0;
//        int i;
//        for (i = a; i <= 100; i++)
//        {
//            if (i % 2 == 0)
//            {
//                b += i;
//            }
//        }
//        printf("%d\n", b);
//    }
//    return 0;
//}


































//int main()
//{
//	int a;
//	while (scanf("%d", &a) != EOF)
//	{
//		for (a;a <= 10;a++)
//		{
//			printf("%d\n",a);
//		}
//	}
//	return 0;
//}































//int main()
//{
//	int a;
//	while (scanf("%d", &a) != EOF)
//	{
//		long long int b = 1;
//		while (a> 0)
//		{
//			b = a * b;
//			a -= 1;
//		}
//		printf("%lld\n", b);
//	}
//	return 0;
//}















//int main()
//{
//	int a;
//	while (scanf("%d", &a) != EOF)
//	{
//		while (a)
//		{
//			printf("%d", a % 10);
//			a /= 10;
//}
//	}
//	return 0;
//}
































//int main()
//{
//	int a = 1;
//	while (a <= 10)
//	{
//		printf("%d\n", a);
//		a ++;
//	}
//	return 0;
//}






























//int main()
//{
//	int a;
//	while (scanf("%d", &a) != EOF)
//	{
//		switch(a)
//		{
//			case 1:printf("工作日\n",a);
//				break;
//			case 2:printf("工作日\n",a);
//				break;
//			case 3:printf("工作日\n",a);
//				break;
//			case 4:printf("工作日\n", a);
//				break;
//			case 5:printf("工作日\n", a);
//				break;
//			case 6:printf("休息日\n", a);
//				break;
//			case 7:printf("休息日\n", a);
//				break;
//			default:printf("无\n");
//
//		}
//	}
//	return 0;
//}
























//int main()
//{
//	int year;
//	while (scanf("%d", &year) != EOF)
//	{
//		if (year % 4 == 0 && year % 100 != 0||year%400==0)
//		{
//			printf("闰年");
//		}
//		else
//		{
//			printf("no");
//		}
//	}
//	return 0;
//}












//int main()
//{
//	char a, b;
//	a = 14;
//	b = 15;
//	printf("%c%c", a, b);
//	return 0;123
//}


 













//int main()
//{double x,g=10,t=3,h;
//	printf("输入高度x:");
//	while (scanf("%lf", &x) != EOF)
//	{
//		h = x - 0.5 * g * t * t;
//		if (h >=0)
//		{
//			printf("下落高度height=%.2lf m\n", h);
//		}
//		else if(h <0)
//		{
//			printf("没了\n");
//		}
//	}
//
//	return 0;
//
//}



//
//h=s-1/2gt^2











































//int main()
//
//{
//    
//    int a = 10;
//
//    int b = 20;
//
//    printf("你好a = %d b = %d\n", a, b);
//
//    a = a ^ b;
//
//    b = a ^ b;
//
//    a = a ^ b;
//
//    printf("ʲô������a = %d b = %d\n", a, b);
//
//    return 0;
//
//}













//int main()
//{
//	int a;
//	while (scanf("%d", &a) != EOF)
//	{
//		if (a >= 10)
//		{
//			printf("a>=10\n");
//		}
//		else if (a >= 5)
//		{
//			printf("a>=5\n");
//		}
//		else
//		{
//			printf("a<5\n");
//		}
//	}
//	return 0;
//}


//int main()
//{
//	int a = 1;
//	int b = 2;
//	int c = (a > b, a = b + 10, b = a + 1);
//	printf("%d\n", c);
//	return 0;
//}














//int main()
//{
//	int a = 11;
//	int b = 12;
//	int ret = a > b ? a : b;
//	printf("%d/n", ret);
//	return 0;
//}







//int main()
//{
//	char a = 10;
//	printf("%d", ~2);
//	return 0;
//}











//int main1()
//{
//
//
//
//
//	
//	  
//	int a = 10;
//	printf("%d\n", a);
//	int b = ++a;
//	printf("%d\n,%d\n",b,a);
//
//	
//	
//	
//	
//	
//	
//	//char ch = 'a';
//	//printf("%c\n", ch);
//	return 0;
//
//
//
//
//
//
//}
