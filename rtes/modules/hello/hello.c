#include <linux/init.h>
#include <linux/module.h>
MODULE_LICENSE("Dual BSD/GPL");
static int hello_init(void)
{
printk(KERN_ALERT "Hello, world! Kernel-space -- the land of the free and the home of the brave.\n");
return 0;
}
static void hello_exit(void)
{
printk(KERN_ALERT "Exiting...\n");
}
module_init(hello_init);
module_exit(hello_exit);
