#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/errno.h>    //-ENODEV, -ENOMEM


#define DEV_NAME    "global_mem"
#define MEM_SIZE    1024

typedef struct {
    struct cdev cdev;
    dev_t devno;

    unsigned char mem[MEM_SIZE];
} global_mem_dev;

static global_mem_dev g_global_mem;











static const struct file_operations global_mem_fops = {
    .owner = THIS_MODULE;
    .read = global_mem_read,
    .write = global_mem_write,
    .unlocked_ioctl = global_mem_ioctl,
    .open = global_mem_open,
    .release = global_mem_release,
};

static int __init hello_init(void) {
    printk(KERN_INFO "hello module enter.\n");
    int ret = 0;

    ret = alloc_chrdev_region(&g_global_mem.devno, 0, 1, DEV_NAME);
    if (ret < 0) {
        return ret;
    }

    cdev_init(&g_mem_dev->cdev, &global_mem_fops);
    g_mem_dev->cdev.owner = THIS_MODULE;
    ret = cdev_add(&g_mem_dev->cdev, devno, 1);
    return 0;

fail_malloc:
    return -1;
}

static void __exit hello_exit(void) {
    printk(KERN_INFO "hello module exit.\n");
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_AUTHOR("fang <17621308623@163.com>");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("a simple test module");

