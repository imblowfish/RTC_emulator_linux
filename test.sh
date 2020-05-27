#!bin/bash
echo "start testing"
dir=$(pwd)
echo $dir
echo "install module..."
$(sudo insmod $dir/rtc_mod.ko)
# проверка

res=$(lsmod | grep rtc_mod)

if [[ ${#res} == 0 ]]
then
	echo "install module error"
	exit
fi

echo $res
sudo ls -l /dev/rtc_0
sudo ls -l /proc/rtc_0
echo "module installed"

echo "*******TEST******"
echo "*****************"
echo "check NORMAL_MODE"
sudo cat /dev/rtc_0
sudo date -u '+%H:%M:%S %d-%m-%Y'

echo "*****************"
echo "check FAST_MODE"
echo "setup mode in /proc/rtc_0"
sudo echo m 1 > /proc/rtc_0
echo "setup time_param in /proc/rtc_0"
sudo echo t 100 > /proc/rtc_0
sleep 2
echo "RTC_Emul:" $(sudo cat /dev/rtc_0)
echo "Date:" $(sudo date -u '+%H:%M:%S %d-%m-%Y')

echo "*****************"
echo "check SLOW_MODE"
echo "setup mode in /proc/rtc_0"
sudo echo m 2 > /proc/rtc_0
echo "setup time_param in /proc/rtc_0"
sudo echo t 5 > /proc/rtc_0
sleep 2
echo "RTC_Emul:" $(sudo cat /dev/rtc_0)
echo "Date:" $(sudo date -u '+%H:%M:%S %d-%m-%Y')

echo "*****************"
echo "check RAND_MODE"
echo "setup mode in /proc/rtc_0"
sudo echo m 3 > /proc/rtc_0
sleep 2
echo "RTC_Emul:" $(sudo cat /dev/rtc_0)
echo "RTC_Emul:" $(sudo cat /dev/rtc_0)
echo "Date:" $(sudo date -u '+%H:%M:%S %d-%m-%Y')

echo "*****************"
echo "UTC+3 MOSCOW"
sudo echo m 0 > /proc/rtc_0
sudo echo h 3 > /proc/rtc_0
echo "RTC_Emul:" $(sudo cat /dev/rtc_0)
echo "Date:" $(sudo date '+%H:%M:%S %d-%m-%Y')
echo "*****************"

echo "Test output from /proc interface"
sudo cat /proc/rtc_0
echo "***TEST END***"

echo "delete module..."
$(sudo rmmod $dir/rtc_mod.ko)

res=$(lsmod | grep rtc_mod)
if [[ ${#res} > 0 ]]
then
	echo "delete module error"
	exit
fi
echo "delete success"

# проверка удаления