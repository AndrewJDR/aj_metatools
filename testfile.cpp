#include <iostream>
#include <stdint.h>
int someFunc()
{
	return 0;
}

int main()
{
	int a = 7;
	int k = 10;
	int otherVar = 77;

	for(int ii = 0; ii < 5; ii++)
	{
		int32_t a = k + (5 / k * 5) - ii * k * someFunc();
	}
}
