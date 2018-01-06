#include<linux/reservation.h>
#include<linux/spinlock_types.h>
#include<linux/semaphore.h>
#include<linux/calc.h>

static DEFINE_SPINLOCK(sysfs_thread_lock);

static char part_mesg[64];

char mes1[]="FF";
char mes2[]="NF";
char mes3[]="BF";
char mes4[]="WF";

int policy_bin_flag;

msgstore *head = NULL;

msgstore *find_pid_here(pid_t pid){
    msgstore *iterate;
    
    if (head == NULL)
        return NULL;
    iterate = head;
    while(iterate != NULL){
        if (iterate -> pid == pid)
            return iterate;
        
        iterate = iterate->next;
    }
    return NULL;
}


static ssize_t c_show(struct kobject *kobj, struct kobj_attribute *attr,
              char *buf)
{
   // rcu_read_lock();
    unsigned long sysfs_fl;
    spin_lock_irqsave(&sysfs_thread_lock,sysfs_fl);

    int error_st;
    msgstore *instr;
    pid_t pid;
    char s[10];
    strcpy(s,attr->attr.name);

    error_st = kstrtoint(s,10,&pid);
    if (error_st < 0)
        return error_st;

    instr = find_pid_here(pid);
    int len = 0;
    len = sprintf(buf,"%s",instr -> mesg);
    strcpy(instr -> mesg,"");

    //rcu_read_unlock();
    spin_unlock_irqrestore(&sysfs_thread_lock,sysfs_fl);

    return len;
}

static ssize_t c_store(struct kobject *kobj, struct kobj_attribute *attr,
               const char *buf, size_t count)
{ 
    return -ENOTSUPP;   
}

msgstore *create_new_pidnode(pid_t pid){

    char s[10];
    msgstore *new_node_m = (msgstore *)kmalloc(sizeof(msgstore),GFP_NOWAIT);
    new_node_m -> pid = pid;
    strcpy(new_node_m -> mesg, "");
    strcpy(new_node_m -> timestamp, "0");

    sprintf(s,"%ld",pid);
    
    new_node_m -> pid_attrb.attr.name=(char *)kmalloc(sizeof(char)*10,GFP_NOWAIT);
    strcpy(new_node_m -> pid_attrb.attr.name,s);
    new_node_m -> pid_attrb.attr.mode= 0666;
    new_node_m -> pid_attrb.show = c_show;
    new_node_m -> pid_attrb.store = c_store;

    new_node_m->next = NULL;
    return new_node_m;

}


msgstore *insert_new_pid(pid_t pid){
    msgstore *newone;
    int ret;
    newone = create_new_pidnode(pid);
    if (head == NULL)
    {
        head = newone;
        newone -> next = NULL;
    }
    else{
        newone -> next = head;
        head = newone;
    }
    
    ret = sysfs_create_file(utilobj, &newone->pid_attrb.attr);
    if (ret){
        printk("File not created\n");
        return NULL;
    }
    
    //printk("File %d created\n",pid);
    return newone;
}

static ssize_t enabled_show(struct kobject *kobj, struct kobj_attribute *attr,
            char *buf)
{
    return -ENOTSUPP;
    //return sprintf(buf,"%d",enable);
}

static ssize_t enabled_store(struct kobject *kobj, struct kobj_attribute *attr,
             const char *buf, size_t count)
{
    
    int ret;
    int error_st;
    unsigned long sysfs_fl;
    //rcu_read_lock();
    
    spin_lock_irqsave(&sysfs_thread_lock,sysfs_fl);

    sscanf(buf,"%du",&enable);
    error_st = kstrtoint(buf,10,&ret);
    if (error_st < 0)
        return error_st;

    if (ret == 1)
    {
        monitoring_flag =1;
    }

    if (ret == 0)
        monitoring_flag =0;
   // rcu_read_unlock();
    spin_unlock_irqrestore(&sysfs_thread_lock,sysfs_fl);

    return count; 

}

int get_policy_bin_flag(){
    return policy_bin_flag;
}

static ssize_t partition_show(struct kobject *kobj, struct kobj_attribute *attr,
                      char *buf)                                                
{
    int len;
    //printk("partition_policy::inside cat file\n");
    //printk("partition_policy:: cat %s\n",part_mesg);
    len = sprintf(buf,"%s",part_mesg);
    return len;
    //strcpy(buf,part_mesg);
                                                                                
}                                                                               
                                                                                
static ssize_t partition_store(struct kobject *kobj, struct kobj_attribute *attr,
                      char *buf, size_t count)                                  
{                                                                               
    sscanf(buf,"%s",part_mesg);                                                           
    if (strcmp(part_mesg,mes1)==0)                                                   
        policy_bin_flag = 1;                                                        
    else if (strcmp(part_mesg,mes2)==0)                                              
        policy_bin_flag = 2;                                                        
    else if (strcmp(part_mesg,mes3)==0)                                              
        policy_bin_flag = 3;                                                        
    else if (strcmp(part_mesg,mes4)==0)                                              
        policy_bin_flag = 4;                                                        
    else                                                                        
        policy_bin_flag = 1;                                                        
    printk("partition_policy::: message inside the files is %s, partition %d\n",part_mesg,policy_bin_flag);          
   // strcpy(part_mesg,"");                                                            
                                                                                
}


static ssize_t reserve_show(struct kobject *kobj, struct kobj_attribute *attr,  
                      char *buf)                                                
{                                                                               
    printk("reserve:: starting catenation\n");
    reserve_t *temp;                                                            
    int i, len;                                                                      
    char mesg_resv[250]="";
    char mesg_buf[4096]="";                                                            
    sprintf(mesg_resv,"TID\tPID\tPRIO\tCPU\tNAME\n");
    printk("%s",mesg_resv);
        for (i=0;i<4;i++){
            printk("getting head\n");
            temp= get_rhead_sysfs(i);                                                    
            //temp=NULL;                                                        
            while (temp!=NULL){
                printk("temp not null\n");
                if (temp->thread!=NULL)
                    printk("thread not null\n");
                sprintf(mesg_resv,"%ld\t%ld\t%ld\t%d\t%s\n",temp->thread->pid,
                        temp->thread->parent->pid, temp->thread->rt_priority,i,temp->thread->comm);
                //printk("printing mesg_resv\n");
                //printk("%s\n",mesg_resv);
                //strcpy(mesg,"");                                              
                strcat(mesg_buf,mesg_resv);                                               
                strcpy(mesg_resv,"");

                temp=temp->next;
            }                                                                   
                                                                                
        }                                                                       
        len = sprintf(buf,"%s",mesg_buf);
        //printk("%s\n",mesg_buf);
    return len;                                                                  
}                                                                               
                                                                                
static ssize_t reserve_store(struct kobject *kobj, struct kobj_attribute *attr, 
                      char *buf, size_t count)                                  
{                                                                               
        return -ENOTSUPP;                                                       
}     

void delete_sysnode(pid_t pid){
    msgstore *r_delete;                                                        
    msgstore *prev; 
   if (head == NULL){                                                 
                                                                                
       // spin_unlock_irqrestore(&thread_lock,fl);                                
        printk(KERN_INFO "sysfs: No node to delete\n");                           
        return -1;                                                              
    }                                                                           
    else if(head->pid == pid){                                 
                                                                                
        if(head->next == NULL){                                        
            kfree(head);                                               
            head = NULL;                                               
        }                                                                       
        else{                                                                   
            prev = head->next;                                         
            kfree(head);                                               
            head = prev;                                               
        }                                                                       
    }                                                                           
    else{                                                                       
                                                                                
        prev = head;                                                   
                                                                                
        while(prev->next != NULL && prev->next->pid != pid){            
            prev = prev->next;                                                  
        }                                                                       
        if(prev->next->pid==pid){                                       
            r_delete = prev->next;                                              
            prev->next = prev->next->next;                                      
            kfree(r_delete);                                                    
        }                                                                       
        else{                                                                   
           // spin_unlock_irqrestore(&thread_lock,fl);                            
            printk(KERN_INFO "sysfs:node NOT FOUND\n");                               
            return -1;                                                          
        }  
    }
    return 0; 

}

void createfiles_pid(pid_t pid, struct timespec C, struct timespec T){
    unsigned long sysfs_fl;
    //rcu_read_lock();

    char temp1[64] = "";
    char temp2[64] = ""; 
    char temp3[64] = ""; 
    char T_string[64] = "0";
    char C_string[64] = "0";
    ktime_t T_ktime, C_ktime;

    char util[64] = "";
    char timestmp[64] = "";
    char temp[100] = "";
    msgstore *instr;
    
    spin_lock_irqsave(&sysfs_thread_lock,sysfs_fl);

    T_ktime = timespec_to_ktime(T);
    C_ktime = timespec_to_ktime(C);

    sprintf(C_string,"%ld",ktime_to_ns(C_ktime));
    sprintf(T_string,"%ld",ktime_to_ns(T_ktime));
    sprintf(temp1,"%ld",ktime_to_ms(T_ktime));
    

    rtes_calculateResult(C_string,T_string,'/', util);
    
    instr = find_pid_here(pid);
    if (instr == NULL)
        instr = insert_new_pid(pid);
    
    if(strcmp(util,"0") != 0)
    {   
        strcpy(temp2, instr->timestamp);
        rtes_calculateResult(temp1, temp2,'+',timestmp);
        strcpy(instr->timestamp, timestmp);
    }          
            
	if(monitoring_flag == 1){ 
        if(strcmp(util,"0") != 0)
        {
           // strcpy(temp1, temp3);
           sprintf(temp,"%s %s\n", timestmp , util);
           strcat(instr->mesg,temp);
         //  printk("util: %s",util);
        }
    }
    spin_unlock_irqrestore(&sysfs_thread_lock,sysfs_fl);
}

void deletefiles_pid(pid_t pid){
    unsigned long sysfs_fl;
    msgstore *instr;
    spin_lock_irqsave(&sysfs_thread_lock,sysfs_fl);
    instr = find_pid_here(pid);
    sysfs_remove_file(utilobj,&instr->pid_attrb.attr);
    delete_sysnode(pid);
    spin_unlock_irqrestore(&sysfs_thread_lock,sysfs_fl);
}

static struct kobj_attribute reserve_attrb =__ATTR(reserves, 0666, reserve_show,
                                                           reserve_store);


static struct kobj_attribute partition_attrb =__ATTR(partition_policy, 0666, partition_show,
                                                           partition_store);

struct kobj_attribute enabled_attrb =
    __ATTR(enabled, 0666, enabled_show, enabled_store);


static int __init rtes_init(void)
{
    int retval, retval1, retval2;

    printk("sysfsmod:::: initialised");
    policy_bin_flag = 1;
    
    rtesobj = kobject_create_and_add("rtes", NULL);
    taskmonobj = kobject_create_and_add("taskmon",rtesobj);
    utilobj = kobject_create_and_add("util",taskmonobj);
    //partitionobj = kobject_create_and_add("partition_policy",rtesobj);
    //reserveobj = kobject_create_and_add("reserves",rtesobj);

    if (!rtesobj || !taskmonobj || !utilobj)
        return -ENOMEM; 
    retval = sysfs_create_file(taskmonobj, &enabled_attrb.attr);
    retval1 = sysfs_create_file(rtesobj, &partition_attrb.attr);
    retval2 = sysfs_create_file(rtesobj, &reserve_attrb.attr);

    if (retval || retval1 || retval2)
    {
        printk("File enabled was not created");
        kobject_put(utilobj);
        kobject_put(taskmonobj);
       // kobject_put(partitionobj);
        kobject_put(rtesobj);
        return -1;
    }

    return retval;
}

static void __exit rtes_exit(void)
{
    sysfs_remove_file(taskmonobj, &enabled_attrb.attr);
    sysfs_remove_file(rtesobj, &reserve_attrb.attr);
    sysfs_remove_file(rtesobj, &partition_attrb.attr);
    kobject_put(utilobj);
    kobject_put(taskmonobj);
    //kobject_put(partitionobj);
    kobject_put(rtesobj);
}

module_init(rtes_init);
module_exit(rtes_exit);
MODULE_LICENSE("GPL");
