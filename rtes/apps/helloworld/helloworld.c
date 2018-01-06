#include<stdio.h>
#define syscall_number 376
long hello_syscall(void){
    return syscall(syscall_number);
}
int main(int argc,char *argv[]){
    long int a = hello_syscall();
    printf("Syscall returned %ld\n",a);
    return 0;
}
