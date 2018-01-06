#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/proc_fs.h>
#include <linux/sched.h>
#include <asm-generic/uaccess.h>
#define DEVICE_NAME "psdev"
#define CLASS_NAME "dev"

MODULE_LICENSE("GPL");

static int majorNumber;
static int Device_Open=0;
static char *msg;
static char temp[80];
static short size_of_message=0;
static struct class*  charClass  = NULL;
static struct device* charDevice = NULL;

static int dev_open(struct inode *, struct file *);
static int dev_release(struct inode *, struct file *);
static ssize_t dev_read(struct file *, char *, size_t, loff_t *);
static ssize_t dev_write(struct file *, const char *, size_t, loff_t *);
struct task_struct *task_list;

static struct file_operations fops =
{
    .open = dev_open,
    .read = dev_read,
    .write = dev_write,
    .release = dev_release,
};

static int psdev_init(void)
{
    printk(KERN_INFO "psdev: Initializing the psdev LKM\n");

    majorNumber = register_chrdev(0, DEVICE_NAME, &fops);
    if (majorNumber < 0) {
        printk(KERN_ALERT "psdev failed to register a major number\n");
        return majorNumber;
    }

    charClass = class_create(THIS_MODULE, CLASS_NAME);
    if (IS_ERR(charClass)) {
        unregister_chrdev(majorNumber, DEVICE_NAME);
        printk(KERN_ALERT "Failed to register device class\n");
        return PTR_ERR(charClass);
    }

    printk(KERN_INFO "psdev: device class registered correctly\n");

    charDevice = device_create(charClass, NULL, MKDEV(majorNumber, 0),NULL, DEVICE_NAME);
    if (IS_ERR(charDevice)) {
        class_destroy(charClass);
        unregister_chrdev(majorNumber, DEVICE_NAME);
        printk(KERN_ALERT "Failed to create the device\n");
        return PTR_ERR(charDevice);
    }

    printk(KERN_INFO "psdev: device class created correctly\n");
    printk(KERN_INFO "psdev: registered correctly with major number%d\n", majorNumber);

    return 0;
}

static void psdev_exit(void)
{
    device_destroy(charClass, MKDEV(majorNumber, 0));
    class_unregister(charClass);
    class_destroy(charClass);
    printk(KERN_INFO "Unregistered psdev\n");
    unregister_chrdev(majorNumber, DEVICE_NAME);
}

static int dev_open(struct inode *inodep, struct file *filep)
{
    struct task_struct *task_list;
    int leni,lenj;

    strcpy(temp,"TID \tPID \tPri \tName\n");
    lenj = strlen(temp);
    leni = lenj;

    msg = (char *) kmalloc(sizeof(char)*leni, GFP_KERNEL);
    strcpy(msg,temp);

    if (Device_Open) 
        return -EBUSY;

    for_each_process(task_list) {
        if (task_list->pid != task_list->parent->pid) {
            lenj = sprintf(temp, "%d \t%d \t%u \t%s\n",task_list->pid,
                    task_list->parent->pid,task_list->rt_priority,
                    task_list->comm);
            leni += lenj;
            msg = (char *) krealloc(msg, sizeof(char)*leni, GFP_KERNEL);
            strcpy(msg+leni-lenj, temp);
        }
    }
    Device_Open++;
    return 0;
}

static ssize_t dev_read(struct file *filep, char *buffer, size_t len, loff_t *offset)
{
    int error_count=0;
    int lenoo = strlen(msg);
    printk(KERN_INFO "Length:::%d\n",lenoo);
    copy_to_user(buffer,msg,lenoo);
    strcpy(msg,"");

    if (error_count == 0) {
        printk(KERN_INFO "psdev: Sent %d characters to the user\n", size_of_message);
        return (lenoo);
    } else {
        printk(KERN_INFO "psdev: Failed to send %d characters to the user\n", error_count);
        return -EFAULT;
    }
}

static ssize_t dev_write(struct file *filep, const char *buffer, size_t len, loff_t *offset)
{
    printk ("Sorry, this operation isn't supported.\n");
    return -ENOTSUPP;
}

static int dev_release(struct inode *inodep, struct file *filep)
{
    printk(KERN_INFO "psdev: Device successfully closed\n");
    kfree(msg);
    Device_Open--;
    return 0;
}

module_init(psdev_init);
module_exit(psdev_exit);
