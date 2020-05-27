###Тестовое задание для syntacore###

Для сборки модуля просто прописать make
Инициализация модуля sudo insmod rtc_mod.ko
Удаление модуля sudo rmmod rtc_mod.ko

Для просмотра времени через /dev/ sudo cat /dev/rtc_0
Через /proc/ sudo /proc/rtc_0

Есть возможность менять параметры через аргументы модуля, либо через /proc/
В модуле следующие аргументы(в скобках сокращения при изменении через /proc/):
	major - major номер для устройства;
	h_shift(h) - разница в часовых поясах относительно UTC
	mode(m) - режим работы RTC(0 - обычный, 1 - ускоренный, 2 - замедленный, 3 - случайный);
	time_param(t) - параметр, влияющий на изменение времени в режиме работы RTC.
Команда отправки аргументов:

sudo insmod major=1 h_shift=3 mode=0 time_param=1
sudo echo m 2 > /proc/rtc_0
Результат тестирования:
![image](https://github.com/imblowfish/RTC_emulator_linux/blob/master/test_res.png)

