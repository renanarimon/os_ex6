#include <stdio.h>
#include <string.h>
#include <ctype.h>


int main(int argc, char const *argv[])
{
    printf("enter num of clients:");
    char num[5];
    memset(num, 0, sizeof(num));
    scanf("%s", num);
    
    return 0;
}
