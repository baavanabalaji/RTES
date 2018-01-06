#include <linux/init.h>
#include <linux/module.h>
#include <linux/syscalls.h>
#include <linux/unistd.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/dcache.h>
#include <linux/fs.h>
#include <linux/proc_fs.h>
#include <linux/fdtable.h>
#include <linux/slab.h>
#include <linux/fs_struct.h>
#include <linux/rcupdate.h>
#include <linux/kallsyms.h>


MODULE_LICENSE("Dual BSD/GPL");

static char* comm = "app";
unsigned long *sys_call_table;

asmlinkage int (*old_exit)(int);

asmlinkage int new_exit(int error_code)
{
    struct task_struct *tasks;
    struct fdtable *files_table;
    struct path file_path;
    char *file_name;
    int len = 200*sizeof(char);
    char *buf = (char *) kmalloc(len,GFP_KERNEL);
    int i = 0;
    for_each_process(tasks){

        if(strcmp(tasks->comm,comm) == 0){
             printk(KERN_INFO "cleanup: process %s \
                     (PID %ld) did not close\
                     files\n",tasks->comm,tasks->tgid);
            if(tasks != NULL && tasks->files != NULL){

                files_table = &(tasks->files->fdtab);

                while(files_table->fd[i] != NULL){


                    file_path = files_table->fd[i]->f_path;
                    file_name = d_path(&file_path,buf,len);
                    printk(KERN_INFO "cleanup: %s\n",file_name);
                    strcpy(buf,"");
                    i++;

                }
            }

        }
    }
    kfree(buf);
    return (*old_exit)(error_code);
}
static int init_mod(void)
{ 
    sys_call_table=(unsigned long *)kallsyms_lookup_name("sys_call_table");
    old_exit = (void *)sys_call_table[__NR_exit];
    sys_call_table[__NR_exit] = new_exit;
    return 0;
}
static void exit_mod(void)
{
    sys_call_table[__NR_exit] = old_exit;
    printk(KERN_INFO "----------------Module exited cleanly");
    return;
}

module_param(comm,charp,0000);
module_init(init_mod);
module_exit(exit_mod);
