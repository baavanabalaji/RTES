#include<stdio.h>
#include<unistd.h>                                                              
#include<stdlib.h>
#include<string.h>
#include<errno.h>
#include<time.h>

#define reserve 379
#define cancel 380
#define setaff 241
#define getaff 242


#define CPU_SETSIZE 1024
#define __NCPUBITS  (8 * sizeof (unsigned long))

typedef struct
{
    unsigned long __bits[CPU_SETSIZE / __NCPUBITS];
} cpu_set_t;

#define CPU_SET(cpu, cpusetp) \
    ((cpusetp)->__bits[(cpu)/__NCPUBITS] |= (1UL << ((cpu) % __NCPUBITS)))
#define CPU_ZERO(cpusetp) memset((cpusetp), 0, sizeof(cpu_set_t))




int set_reserve(pid_t tid, struct timespec *C, struct timespec *T, int cpuid)                                                    
{                                                                               
    return syscall(reserve, tid, C, T, cpuid);                                              
}                                                                               
                                                                                
int cancel_reserve(pid_t tid)           
{                                                                               
    return syscall(cancel, tid);                          
} 

int main(int argc, char *argv[])
{
    char *test = argv[1];
    pid_t tid;
    int cpuid,x,c,t,result;
    struct timespec C,T;
    cpu_set_t old_set,new_set;
    

    tid = atoi(argv[2]);
    
     //  printf("pid of the reserve task is: %d\n",gettid());


    if (tid == 0){    
        tid = gettid();
    }


    if (strcmp(test,"set") == 0) {
        c = atoi(argv[3]);
        t = atoi(argv[4]);
        cpuid = atoi(argv[5]);
        /*
        CPU_ZERO(&new_set);
        CPU_SET(cpuid, &new_set);

        result = syscall(getaff,gettid(),&old_set);
        if(result == -1)
            printf("\n Failed to get affinity with error code:  %d", errno);

        result = syscall(setaff,gettid(),&new_set);
        if(result == -1)
            printf("\n Failed to set affinity with error code:  %d", errno);
*/

        if(c > t){
            printf("invalid command");
            return 0;
        }
        if(cpuid > 3 || cpuid < -1){
            printf("invalid command");
            return 0;
        }
        if(tid < 0){
            printf("invalid command");
            return 0;
        }
       
        
        C.tv_nsec = (long)(c%1000)*1000000;
        C.tv_sec = c/1000;
        T.tv_nsec = (long)(t%1000)*1000000;
        T.tv_sec = t/1000;
       // printf("C:%lu\n",C.tv_nsec);
       // printf("T:%lu\n",T.tv_nsec);
       // printf("calling syscall set\n");
        x = set_reserve(tid,&C,&T,cpuid);
     
       /* result = syscall(setaff,gettid(),&old_set);
        if(result == -1)
            printf("\n Failed to set affinity with error code:  %d", errno);
*/

   
        if (x == -2){
            printf("caught a NULL");
            return 0;
        }
    
    }else if (strcmp(test,"cancel") == 0) {
        cancel_reserve(tid);
    }
    else{
        printf("invalid command");
    }

    return 0;

}
