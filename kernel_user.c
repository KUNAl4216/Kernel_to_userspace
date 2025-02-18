#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

#define DEVICE_NAME "char_device"
#define BUFFER_SIZE 1024
#define MAJOR_NUM 511 

static char kernel_buffer[BUFFER_SIZE];

static int device_open(struct inode *inode, struct file *file);
static int device_release(struct inode *inode, struct file *file);
static ssize_t device_write(struct file *file, const char __user *user_buffer, size_t len, loff_t *offset);
static ssize_t device_read(struct file *file, char __user *user_buffer, size_t len, loff_t *offset);


static struct file_operations fops = {
    .open = device_open,
    .release = device_release,
    .write = device_write,
    .read = device_read, 
};


static int device_open(struct inode *inode, struct file *file) {
    printk(KERN_INFO "char_device: Device opened\n");
    return 0;
}


static int device_release(struct inode *inode, struct file *file) {
    printk(KERN_INFO "char_device: Device closed\n");
    return 0;
}


static ssize_t device_write(struct file *file, const char __user *user_buffer, size_t len, loff_t *offset) {
    if (len > BUFFER_SIZE - 1) {
        len = BUFFER_SIZE - 1;
    }

    if (copy_from_user(kernel_buffer, user_buffer, len)) {
        printk(KERN_ERR "char_device: Failed to copy data from user\n");
        return -EFAULT;
    }

    kernel_buffer[len] = '\0';
    printk(KERN_INFO "char_device: Received from user: %s\n", kernel_buffer);

    return len;
}


static ssize_t device_read(struct file *file, char __user *user_buffer, size_t len, loff_t *offset) {
    size_t message_len = strlen(kernel_buffer);

    if (*offset >= message_len) {
        return 0; 
    }

    if (len > message_len - *offset) {
        len = message_len - *offset;
    }

    if (copy_to_user(user_buffer, kernel_buffer + *offset, len)) {
        printk(KERN_ERR "char_device: Failed to copy data to user\n");
        return -EFAULT;
    }

    *offset += len; 
    return len;
}


static int __init char_driver_init(void) {
    int ret;

    
    ret = register_chrdev(MAJOR_NUM, DEVICE_NAME, &fops);
    if (ret < 0) {
        printk(KERN_ALERT "char_device: Failed to register the device with major number %d\n", MAJOR_NUM);
        return ret;
    }

    printk(KERN_INFO "char_device: Registered successfully with major number %d\n", MAJOR_NUM);
    return 0;
}


static void __exit char_driver_exit(void) {
    unregister_chrdev(MAJOR_NUM, DEVICE_NAME);
    printk(KERN_INFO "char_device: Unregistered\n");
}

module_init(char_driver_init);
module_exit(char_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Kunal");
MODULE_DESCRIPTION("A simple character device driver with read and write functionality");
