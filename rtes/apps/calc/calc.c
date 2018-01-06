#include<stdio.h>
#include<string.h>
#include<stdlib.h>

#define syscall_number 376
#define MAX_CHAR_LENGTH 64
long calc_syscall(const char* param1, const char* param2, char operation, char* result)
{       
    return syscall(syscall_number,param1,param2,operation,result);
}
int main(int argc,char *argv[])
{
    char param1[MAX_CHAR_LENGTH] = "";
    char param2[MAX_CHAR_LENGTH] = "";
    char result[MAX_CHAR_LENGTH] = "";
    char op;
    
    strcpy(param1,argv[1]);
    strcpy(param2,argv[3]);
    op = *argv[2];

    calc_syscall(param1,param2,op,result);
    printf("Syscall calc returned %s\n",result);
    
    return 0;
}
