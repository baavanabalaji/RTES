#include<linux/kernel.h>
#include<linux/slab.h>
#include<linux/uaccess.h>
#include<linux/string.h>
#include<linux/syscalls.h>
#include<asm/atomic.h>
#include<linux/sched.h>
#include<linux/spinlock_types.h>
#include<linux/semaphore.h>

struct rt_threads_list
{    
    pid_t tid;
    pid_t pid;
    unsigned int prio; 
    char comm[TASK_COMM_LEN];
};

atomic_t count = ATOMIC_INIT(0);
//static DEFINE_SPINLOCK(thread_lock);

asmlinkage long count_rt_threads()
{
    rcu_read_lock();
    atomic_set(&count,0);
    struct task_struct *task_list;
    for_each_process(task_list){    
        if ((task_list->pid != task_list->parent->pid)&&
            (task_list->rt_priority>=0)&&(task_list->rt_priority<=99)){

             atomic_inc(&count);
        }
    }
    rcu_read_unlock();
    return atomic_read(&count);
}

asmlinkage long list_rt_threads(struct rt_threads_list *rt_list, 
        int numThreads)
{
    unsigned long fl;
    struct rt_threads_list *rt_klist;
    rt_klist = kmalloc(sizeof(struct rt_threads_list)*numThreads, GFP_KERNEL);     

    //spin_lock_irqsave(&thread_lock,fl);
    rcu_read_lock();
    struct task_struct *task_list;
    int i = 0;
    for_each_process(task_list){
        if(i < numThreads){
            if((task_list->rt_priority>=0)&&(task_list->rt_priority<=99)){
                rt_klist[i].tid = task_list->pid;
                rt_klist[i].pid = task_list->parent->pid;
                rt_klist[i].prio = task_list->rt_priority;
                strcpy(rt_klist[i].comm, task_list->comm);
                i++;
            }
        }
    }
    numThreads=i;
    //spin_unlock_irqrestore(&thread_lock,fl);
    copy_to_user(rt_list,rt_klist,sizeof(struct rt_threads_list)*numThreads);
    kfree(rt_klist);
    rcu_read_unlock();
    return 0;
}
