#define _GNU_SOURCE
#include<sys/time.h>
#include<errno.h>
#include<asm/unistd.h>
#include<string.h>
#include<signal.h>
#include<stdio.h>
#include<stdlib.h>
#include<sched.h>

#define setaff __NR_sched_setaffinity
#define getaff __NR_sched_getaffinity
#define endjob __NR_end_job

volatile sig_atomic_t usr_interrupt = 1;
unsigned long C,T, cpuid;

#define CPU_SETSIZE 1024
#define __NCPUBITS  (8 * sizeof (unsigned long))

typedef struct
{
    unsigned long __bits[CPU_SETSIZE / __NCPUBITS];
} cpu_set_t;

#define CPU_SET(cpu, cpusetp) \
    ((cpusetp)->__bits[(cpu)/__NCPUBITS] |= (1UL << ((cpu) % __NCPUBITS)))
#define CPU_ZERO(cpusetp) memset((cpusetp), 0, sizeof(cpu_set_t))


void timer_handler (int signum)
{
    pid_t pid = getpid();
    kill(pid,SIGUSR1);
    usr_interrupt = 1;
}


void sigusr1_handler (int signum)
{
    struct itimerval timer;
    
    timer.it_value.tv_sec = C/1000;
    timer.it_value.tv_usec = (long) (C%1000)*1000;
    timer.it_interval.tv_sec = 0;
    timer.it_interval.tv_usec = 0;
    setitimer (ITIMER_VIRTUAL, &timer, NULL);

    /* Do busy work. */
   // printf ("\tPeriodic: Executing for C\n");
    while(usr_interrupt); 
}

void comp_handler (int signum)
{
    usr_interrupt = 0;
}


static inline int timespec_compare(const struct timespec *lhs, const struct timespec *rhs)
{
    if (lhs->tv_sec < rhs->tv_sec)
        return -1;
    if (lhs->tv_sec > rhs->tv_sec)
        return 1;
    return lhs->tv_nsec - rhs->tv_nsec;
}


int main(int argc, char **argv)
{
    struct sigaction sa,sb,sc;
    struct itimerval timer;
    struct timespec S1,E1,S2,d1,d2;
    unsigned long max1, max2, min1, min2, bug;
    sigset_t sigmask;
    cpu_set_t my_set;    
    long ret;
    int start_flag = 0;

    C = strtoul(argv[1],NULL,10);
    T = strtoul(argv[2],NULL,10);

    if(C>T){
        printf("\nC is greater than T");
        return -1;
    }
        
    cpuid = strtoul(argv[3],NULL,10);

    CPU_ZERO(&my_set);   
    CPU_SET(cpuid, &my_set);
                 
    int result = syscall(setaff,gettid(),&my_set);   
    if(result == -1)
        printf("\n Failed with error code:  %d", errno);   
 
//    printf("\nPID is: %ld\n",getpid());
//    printf("\nTID is: %ld\n",gettid());

    memset (&sa, 0, sizeof (sa));
    sa.sa_handler = &timer_handler;
    sigaction (SIGALRM, &sa, NULL);

    memset (&sb, 0, sizeof (sb));
    sb.sa_handler = &sigusr1_handler;
    sigaction (SIGUSR1, &sb, NULL);
    
    memset (&sc, 0, sizeof (sc));
    sc.sa_handler = &comp_handler;
    sigaction (SIGVTALRM, &sc, NULL);
    
    timer.it_value.tv_sec = T/1000;
    timer.it_value.tv_usec = (long) (T%1000)*1000;
    timer.it_interval.tv_sec = T/1000;
    timer.it_interval.tv_usec = (long) (T%1000)*1000;
 
    setitimer (ITIMER_REAL, &timer, NULL);
    sigfillset(&sigmask);
    sigdelset(&sigmask,SIGALRM);
    sigdelset(&sigmask,SIGVTALRM);
    sigdelset(&sigmask,SIGUSR1);
   
    S1.tv_sec = 0;
    S1.tv_nsec = 0;

    E1.tv_sec = 0;
    E1.tv_nsec = 0;

    S2.tv_sec = 0;
    S2.tv_nsec = 0;
    
    d1.tv_sec = 0;
    d1.tv_nsec = 0;

    d2.tv_sec = 0;
    d2.tv_nsec = 0;
            
    max1 = 10;
    min1 = 5;
    max2 = 20;
    min2 = 10;

    bug = 0;

    /* Do wait work. */
    while(1){
    //       printf("Periodic: Executing T-C\n");
        clock_gettime(CLOCK_MONOTONIC,&S1);
        
        if(timespec_compare(&S1,&S2) >= 0){ // || start_flag == 0){
            d1.tv_sec = rand()%(max1-min1)+min1;
            d2.tv_sec = rand()%(max2-min2)+min2;
            
            E1.tv_sec = S1.tv_sec + d1.tv_sec; 
            E1.tv_nsec = S1.tv_nsec; 
            
            S2.tv_sec = S1.tv_sec + d2.tv_sec; 
            S2.tv_nsec = S1.tv_nsec;
            
            start_flag = 1;

//            printf("S1: %ld %ld \n",S1.tv_sec,S1.tv_nsec);
            //printf("d1: %ld secs \nd2:  %ld secs\n",d1.tv_sec,d2.tv_sec);
//            printf("S2: %ld %ld \nE1:  %ld %ld\n",S2.tv_sec,S2.tv_nsec,E1.tv_sec,E1.tv_nsec);
        }

        if(timespec_compare(&S1,&E1) <= 0){
            bug = 1;
            //printf("bug is ON:\n");
            //printf("bug is ON:\nS1: %ld %ld \nE1:  %ld %ld\n",S1.tv_sec,S1.tv_nsec,E1.tv_sec,E1.tv_nsec);
        }
        else{
            bug = 0;
            //printf("\t\tbug is off\n");
            //printf("bug is off:\nS1: %ld %ld \nE1:  %ld %ld\n",S1.tv_sec,S1.tv_nsec,E1.tv_sec,E1.tv_nsec);
        }

        if(!bug)
            ret = syscall(endjob,gettid());
        else 
            ret = 0;

        if(ret == 0)
            sigsuspend(&sigmask);
    }
    return 0;
}
