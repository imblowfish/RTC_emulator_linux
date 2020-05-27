#include "rtc_mod.h"
#include <linux/random.h>

#include <linux/ctype.h>
//#include "timer.h"

// реализовать возможность добавления подсчета времени
// возможность добавления нескольких устройств

// иницилазация параметров и реализация функций модуля
// читаем параметр major

static int check_params(void){
	if(mode < NORM_MODE || mode > RAND_MODE){
		printk(KERN_ERR "wrong rtc mode %d, must be %d, %d, %d, %d\n", mode, NORM_MODE, FAST_MODE, SLOW_MODE, RAND_MODE);
		return -1;
	}
	if(!time_param){
		printk(KERN_ERR "time param is ZERO\n");
		return -1;
	}
	printk(KERN_INFO "UTC%+d mode=%d time_param=%d\n", h_shift, mode, time_param);
	return 0;
}

static int update_param(char par, int val){
	switch(par){
		case 'h':
			h_shift = val;
		break;
		case 'm':
			if(val < NORM_MODE || val > RAND_MODE)
				break;
			mode = val;
		break;
		case 't':
			if(val <= 0){
				time_param = 1;
				break;
			}
			time_param = val;
		break;
	}
}

static long int update_last_time(void){
	long int diff = get_diff(last_time, h_shift);
	printk(KERN_INFO "diff before = %d\n", diff);
	int seed;
	switch(mode){
		case FAST_MODE:
			diff *= time_param;
		break;
		case SLOW_MODE:
			diff /= time_param;
		break;
		case RAND_MODE:
			get_random_bytes(&seed, sizeof(seed));
			seed %= 10;
			if(seed < 0)
				diff /= -seed;
			else
				diff *= seed;
		break;
		default:
		
		break;
	}
	printk(KERN_INFO "diff = %d\n", diff);
	last_time += diff;
	return 0;
}

static char* get_buf(void){
	static char buf_msg[LEN_MSG+1];
	return buf_msg;
}

static void parse_parameters(char *pars){
	while(isspace(*pars))
		pars++;
	// получаем имя параметра
	char modified_param = *pars++;
	// ищем начало цифр
	while(isspace(*pars))
		pars++;
	char sign = 1;
	if(*pars == '-'){
		sign = -1;
		pars++;
	}
	int val = 0;
	while(isdigit(*pars))
		val = val * 10 + *pars++ - '0';
	val *= sign;
	printk(KERN_INFO "%c %d\n", modified_param, val);
	// проверяем новые значения параметров
	update_param(modified_param, val);
	last_time = get_now_seconds(h_shift);
	printk(KERN_INFO "updated UTC%+d mode=%d time_param=%d\n", h_shift, mode, time_param);
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
	update_last_time();
	struct time_info ti = time_info_from_seconds(last_time);
	char *buf_msg = get_buf();
	sprintf(buf_msg, "%d:%d:%d %d-%d-%d\n", ti.hour, ti.minute, ti.second, ti.day, ti.month, ti.year);
	int res;
	if(*ppos >= strlen(buf_msg)){
		*ppos = 0;
		return 0;
	}
	if(cnt > strlen(buf_msg) - *ppos){
		cnt = strlen(buf_msg) - *ppos;
	}
	res = copy_to_user((void*)buf, buf_msg + *ppos, cnt);
	*ppos += cnt;
	return cnt;
}

static ssize_t rtc_write(struct file *f, const char *buf, size_t cnt, loff_t *ppos){
	char *buf_msg = get_buf();
	int res;
	uint len = cnt < LEN_MSG? cnt: LEN_MSG;
	printk(KERN_INFO "get new parameters\n");
	res = copy_from_user(buf_msg, (void*)buf, len);
	buf_msg[len] = '\0';
	parse_parameters(buf_msg);
	return len;
}

static int __init rtc_init(void){
	if(check_params() < 0)
		return -1;
	last_time = get_now_seconds(h_shift);
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





