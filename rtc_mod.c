#include "rtc_mod.h"

// реализовать возможность добавления подсчета времени
// возможность добавления нескольких устройств

// иницилазация параметров и реализация функций модуля
// читаем параметр major
#include <linux/rtc.h>

static char *get_now_time(void){
	
}

static char *get_buf(void){
	static char buf_msg[LEN_MSG+1] = "..1..2..3..4..5\n";
	return buf_msg;
}

static int rtc_open(struct inode *n, struct file *f){
	if(deviceOpen)
		return -EBUSY;
	deviceOpen = !deviceOpen;
	return 0;
}

static int rtc_release(struct inode *n, struct file *f){
	deviceOpen = !deviceOpen;
	return 0;
}

static ssize_t rtc_read(struct file *f, char *buf, size_t cnt, loff_t *ppos){
	char *buf_msg = get_buf();
	int res;
	printk(KERN_INFO "read: %ld bytes {ppos%}\n", (long)cnt, *ppos);
	if(*ppos >= strlen(buf_msg)){
		*ppos = 0;
		printk(KERN_INFO "EOF\n");
		return 0;
	}
	if(cnt > strlen(buf_msg) - *ppos){
		cnt = strlen(buf_msg) - *ppos;
	}
	res = copy_to_user((void*)buf, buf_msg + *ppos, cnt);
	*ppos += cnt;
	printk(KERN_INFO "return %ld bytes\n", (long)cnt);
	return cnt;
}

static ssize_t rtc_write(struct file *f, const char *buf, size_t cnt, loff_t *ppos){
	//proc
	char *buf_msg = get_buf();
	int res;
	uint len = cnt < LEN_MSG? cnt: LEN_MSG;
	printk("write: %ld bytes\n", (long)cnt);
	res = copy_from_user(buf_msg, (void*)buf, len);
	buf_msg[len] = '\0';
	printk(KERN_INFO "put %d bytes\n", len);
	return len;
}

static int __init rtc_init(void){
	int res;
	//!!!
	struct proc_dir_entry *own_proc_node;
	own_proc_node = proc_create(NAME_NODE, S_IFREG|S_IRUGO|S_IWUGO, NULL, &dev_fops);
	if(!own_proc_node){
		printk(KERN_ERR "can't create /proc/%s\n", NAME_NODE);
		return -ENOENT;
	}
	printk(KERN_INFO "/proc/%s installed\n", NAME_NODE);
	//!!!
	
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

static void __exit rtc_exit(void){
	//!!!
	remove_proc_entry(NAME_NODE, NULL);
	//!!!
	dev_t dev;
	dev = MKDEV(major, DEV_FIRST);
	device_destroy(devclass, dev);
	class_destroy(devclass);
	cdev_del(&hcdev);
	unregister_chrdev_region(MKDEV(major, DEV_FIRST), DEV_CNT);
	printk(KERN_INFO "rtc_module removed\n");
} 





