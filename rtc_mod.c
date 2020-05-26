// модуль устройства RCT
#include <linux/fs.h>
#include <linux/module.h>
#include <linux/uaccess.h>

#include <linux/device.h>
#include <linux/version.h>
#include <linux/init.h>
#include <linux/cdev.h>
//#include "mod.h"
// Добавление устройства в /dev/rtcN

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Yury Getman <stagg7777@gmail.com>");

// инициализация модуля
static int __init dev_init(void);
// удаление модуля
static void __exit dev_exit(void);
// чтение данных
static ssize_t rtc_mod_read(struct file *f, char *buf, size_t cnt, loff_t *ppos);
// создание устройства
static int rtc_mod_open(struct inode *n, struct file *f);
// удаление устройства
static int rtc_mod_release(struct inode *n, struct file *f);

// диапазон номеров устройств(миноров)
#define DEV_FIRST 0
#define DEV_CNT 1
#define DEV_NAME "rtc_"
// имя модуля
#define MOD_NAME "rtc_module"

static char* hello = "Hello\n";
static int major=0;
static int deviceOpen = 0;
static struct cdev hcdev;
static struct class *devclass;

// иницилазация параметров и реализация функций модуля
// читаем параметр major
module_param(major, int, S_IRUGO); // S_IRUGO - может читаться, кем угодно, но не может быть изменен

static int rtc_mod_open(struct inode *n, struct file *f){
	if(deviceOpen)
		return -EBUSY;
	deviceOpen = !deviceOpen;
	return 0;
}

static int rtc_mod_release(struct inode *n, struct file *f){
	deviceOpen = !deviceOpen;
	return 0;
}

static ssize_t rtc_mod_read(struct file *f, char *buf, size_t cnt, loff_t *ppos){
	int len = strlen(hello);
	printk(KERN_INFO "read: %ld\n", cnt);
	if(cnt < len)
		return -EINVAL;
	if(*ppos){
		printk(KERN_INFO "read return: 0\n");
		return 0;
	}
	if(copy_to_user(buf, hello, len))
		return -EINVAL;
	*ppos = len;
	printk(KERN_INFO "read return: %d\n", len);
	return len;
}

static const struct file_operations dev_fops = {
	.owner = THIS_MODULE,
	.open = rtc_mod_open,
	.release = rtc_mod_release,
	.read = rtc_mod_read
};

static int __init dev_init(void){
	int res;
	// dev_t тип для представления номера устройства в ядре
	dev_t dev;
	// если задали в параметрах major
	if(major != 0){
		dev = MKDEV(major, DEV_FIRST);
		// регистрируем интервал номеров устройств для модуля
		res = register_chrdev_region(dev, DEV_CNT, MOD_NAME);
	}else{
	// иначе, выбираем динамически
		// резервируем интервал символьных имен адресов
		res = alloc_chrdev_region(&dev, DEV_FIRST, DEV_CNT, MOD_NAME);
		//записываем динамически выданный major
		major = MAJOR(dev);
	}
	if(res < 0){
		printk(KERN_ERR "can't register device region\n");
		return res;
	}
	// готовим устройство к добавлению 
	cdev_init(&hcdev, &dev_fops);
	hcdev.owner = THIS_MODULE;
	// добавляем устройство
	res = cdev_add(&hcdev, dev, DEV_CNT);
	if(res < 0){
		unregister_chrdev_region(MKDEV(major, DEV_FIRST), DEV_CNT);
		printk(KERN_ERR "can't add char device\n");
		return res;
	}
	devclass = class_create(THIS_MODULE, "dyn_class");
	dev = MKDEV(major, DEV_FIRST);
	device_create(devclass, NULL, dev, NULL, "%s%d", DEV_NAME, DEV_FIRST);
	printk(KERN_INFO "module installed %d/%d\n", MAJOR(dev), DEV_FIRST);
	return 0;
}

static void __exit dev_exit(void){
	dev_t dev;
	dev = MKDEV(major, DEV_FIRST);
	device_destroy(devclass, dev);
	class_destroy(devclass);
	cdev_del(&hcdev);
	unregister_chrdev_region(MKDEV(major, DEV_FIRST), DEV_CNT);
	printk(KERN_INFO "rtc_module removed\n");
} 

module_init(dev_init);
module_exit(dev_exit);





