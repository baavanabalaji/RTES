#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#define count_threads 377
#define list_threads 378
struct rt_threads_list
{
    pid_t tid;
    pid_t pid;
    unsigned int prio;
    char comm[16];
};

long sys_count_threads(void)
{
    return syscall(count_threads);
}

long sys_list_threads(struct rt_threads_list* rt_list,int numThreads)
{
    return syscall(list_threads, rt_list, numThreads);
}

void print_threads_info(struct rt_threads_list* rt_list,int numThreads)
{
    printf("\nTID \tPID \tPRIO \tNAME\n");
    int i = 0;
    while(i < numThreads){
        printf("%ld \t%ld \t%ld \t%s\n",rt_list[i].tid, rt_list[i].pid, 
                rt_list[i].prio, rt_list[i].comm);
        i++;
    } 
}

int main(int argc,char *argv[])
{
    int numThreads;
    int l,m, i,lines = 0; 
    struct rt_threads_list* rt_list;
    struct rt_threads_list temp;
    
    while(1){
        numThreads = sys_count_threads();
        if(numThreads > 0){
            rt_list = (struct rt_threads_list *) malloc(sizeof
                    (struct rt_threads_list)*numThreads);
            printf("Number of threads: %d",numThreads);
            sys_list_threads(rt_list, numThreads);
            
           // for(i = 0; i < lines; i++){
           //     printf("\033[F");
           // }
            
            lines = 1;

            for (l = 1; l < numThreads; l++){

                for (m = 0; m < numThreads-l; m++){
                
                    if (rt_list[m].prio < rt_list[m+1].prio){
                        temp = rt_list[m];
                        rt_list[m] = rt_list[m+1];
                        rt_list[m+1] = temp;
                    }
                   // lines++;

                }
            }
            
            print_threads_info(rt_list, numThreads);
            free(rt_list);
        }

        sleep(2);
    }
    return 0;
}
