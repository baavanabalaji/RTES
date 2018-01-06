#define _GNU_SOURCE
#include<sys/time.h>
#include<errno.h>
#include<asm/unistd.h>
#include<string.h>
#include<signal.h>
#include<stdio.h>
#include<stdlib.h>
#include<sched.h>

#define setaff 241
#define getaff 242
#define cancel 380

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

int cancel_reserve(pid_t tid)                                                   
{                                                                               
    return syscall(cancel, tid);                                                
}


void timer_handler (int signum)
{
    pid_t pid = getpid();
    kill(pid,SIGUSR1);
    usr_interrupt = 1;
}


void budgetexcess_handler(int signum)
{
//    printf("received signal no: %d ::SIGEXCESS. Exceeded budget.\n",signum);                           
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
    //printf ("\tPeriodic: Executing for C\n");
    while(usr_interrupt); 
}

void comp_handler (int signum)
{
    usr_interrupt = 0;
}

void stop_handler (int signum)
{
   int mytid = gettid();
//   cancel_reserve(mytid);
   exit(0);
}

int main(int argc, char **argv)
{
    struct sigaction sa,sb,sc,sd,se;
    struct itimerval timer;
    sigset_t sigmask;
    cpu_set_t my_set;    
    
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
    
    memset(&sd, 0,sizeof(sd));
    sd.sa_handler = &budgetexcess_handler;
    sigaction (SIGEXCESS, &sd, NULL);

    memset (&se, 0, sizeof (se));
    se.sa_handler = &stop_handler;
    sigaction (SIGINT, &se, NULL);    

    timer.it_value.tv_sec = T/1000;
    timer.it_value.tv_usec = (long) (T%1000)*1000;
    timer.it_interval.tv_sec = T/1000;
    timer.it_interval.tv_usec = (long) (T%1000)*1000;
 
    setitimer (ITIMER_REAL, &timer, NULL);
    sigfillset(&sigmask);
    sigdelset(&sigmask,SIGALRM);
    sigdelset(&sigmask,SIGVTALRM);
    sigdelset(&sigmask,SIGUSR1);
    sigdelset(&sigmask,SIGEXCESS);


    /* Do wait work. */
    while(1){
      //  printf("Periodic: Executing T-C\n");
        sigsuspend(&sigmask);
    }
    return 0;
}
