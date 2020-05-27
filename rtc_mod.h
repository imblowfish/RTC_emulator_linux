#include "mod.h"
#include "rtc_defs.h"
#include "timer.h"

#define NORM_MODE 0
#define FAST_MODE 1
#define SLOW_MODE 2
#define RAND_MODE 3

static int major=0;
static int h_shift = 0;	// UTC default
static int mode = 0;	// normal default
static int time_param = 1;

static long int last_time;
static int deviceOpen = 0;
static struct cdev hcdev;
static struct class *devclass;

// чтение данных
static ssize_t rtc_read(struct file *f, char *buf, size_t cnt, loff_t *ppos);
// запись
static ssize_t rtc_write(struct file *f, const char *buf, size_t cnt, loff_t *pos);
// создание устройства
static int rtc_open(struct inode *n, struct file *f);
// удаление устройства
static int rtc_release(struct inode *n, struct file *f);

static const struct file_operations dev_fops = {
	.owner = THIS_MODULE,
	.open = rtc_open,
	.release = rtc_release,
	.read = rtc_read,
	.write = rtc_write
};

module_param(major, int, S_IRUGO);
module_param(h_shift, int, 0);
module_param(mode, int, 0);
module_param(time_param, int, 0);