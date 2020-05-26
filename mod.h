#include <linux/fs.h>
#include <linux/module.h>
#include <linux/uaccess.h>
#include <linux/proc_fs.h>
#include <linux/device.h>
#include <linux/version.h>
#include <linux/init.h>
#include <linux/cdev.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Yury Getman <stagg7777@gmail.com>");

// инициализация модуля
static int __init rtc_init(void);
// удаление модуля
static void __exit rtc_exit(void);

module_init(rtc_init);
module_exit(rtc_exit);