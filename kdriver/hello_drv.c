#include <linux/init.h>
#include <linux/module.h>
#include <linux/errno.h>    //-ENODEV, -ENOMEM


static int __init hello_init(void) {
    printk(KERN_INFO "hello module enter.\n");
    return 0;
}

static void __exit hello_exit(void) {
    printk(KERN_INFO "hello module exit.\n");
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_AUTHOR("fang <17621308623@163.com>");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("a simple test module");

