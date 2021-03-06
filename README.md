### Тестовое задание для syntacore
#### Постановка задачи
Разработать драйвер (модуль ядра) Linux для виртуального устройства класса RTC.
Модуль должен:
- эмулировать устройство с неравномерным подсчетом времени
(ускоренное/замедленное/случайное)
- поддерживать операции установки/чтения значений
- иметь возможность настойки параметров и получения статистики через интерфейс
/proc
- иметь возможность динамической загрузки (insmod/rmmod)
- встраиваться в /dev как дополнительное устройство rtcN

Для проверки написать shell скрипт, в котором протестировать работоспособность
драйвера (загрузка/выгрузка, настройка модели поведения, чтение/установка
значений, сравнение с системным временем) используя стандартные утилиты Linux.

Для сборки модуля просто прописать make
Инициализация модуля 
<pre><code>
sudo insmod rtc_mod.ko
</code></pre>
Удаление модуля 
<pre><code>
sudo rmmod rtc_mod.ko
</code></pre>

Для просмотра времени через /dev/ 
<pre><code>
sudo cat /dev/rtc_0
</code></pre>
Через /proc/ 
<pre><code>
sudo /proc/rtc_0
</code></pre>

Есть возможность менять параметры через аргументы модуля, либо через /proc/
В модуле следующие аргументы(в скобках сокращения при изменении через /proc/):
- major - major номер для устройства;
- h_shift(h) - разница в часовых поясах относительно UTC
- mode(m) - режим работы RTC(0 - обычный, 1 - ускоренный, 2 - замедленный, 3 - случайный);
- time_param(t) - параметр, влияющий на изменение времени в режиме работы RTC.
Иницилизация с аргументами:
<pre><code>
sudo insmod major=1 h_shift=3 mode=0 time_param=1
</code></pre>
Установка параметров через /proc/:
<pre><code>
sudo echo m 2 > /proc/rtc_0
</code></pre>
Результат тестирования:<br>
![image](https://github.com/imblowfish/RTC_emulator_linux/blob/master/test_res.png)

