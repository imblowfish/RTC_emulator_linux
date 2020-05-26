#include "mod.h"
#include "rtc_defs.h"

static char* hello = "Hello\n";
static int major=0;
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