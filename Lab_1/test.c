#include <stdio.h>

int f(int a, int b)
{
    return a + b;
}

int main(void)
{
    printf(sizeof(long));
    int (*function)(int, int);
    function = f;
    printf("%d\n", function(1, 2));
    return 0;
}