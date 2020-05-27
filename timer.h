#include <linux/ktime.h>
#include <linux/time.h>

#define SEC_IN_YEAR 31556952
#define SEC_IN_HOUR 3600

struct time_info{
	unsigned int year;
	unsigned int month;
	unsigned int day;
	unsigned int hour;
	unsigned int minute;
	unsigned int second;
};

static long int get_now_seconds(int hour_shift){
	struct timespec64 now;
	ktime_get_real_ts64(&now);
	return now.tv_sec + hour_shift * SEC_IN_HOUR;
}

static long int get_diff(long int last_seconds, int hour_shift){
	long int now_seconds = get_now_seconds(hour_shift);
	return now_seconds - last_seconds;
} 

static struct time_info time_info_from_seconds(long int seconds){
	static struct tm time;
	time64_to_tm(seconds, 0, &time);
	struct time_info tinfo;
	tinfo.month = time.tm_mon + 1;
	tinfo.day = time.tm_mday;
	tinfo.year = (1970 + (seconds / SEC_IN_YEAR));
	tinfo.second = seconds%60;
	seconds /= 60;
	tinfo.minute = seconds%60;
	seconds /= 60;
	tinfo.hour = seconds%24;
	return tinfo;
}
