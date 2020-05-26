CURRENT=$(shell uname -r)
KDIR=/lib/modules/$(CURRENT)/build
PWD=$(shell pwd)
DEST=/lib/modules/$(CURRENT)/misc

TARGET=rtc_mod

obj-m:=$(TARGET).o

default:
	$(MAKE) -C $(KDIR) M=$(PWD) modules -Wall
	
clean:
	@rm -f *.o .*.cmd .*.flags *.mod.c *.order
	@rm -f .*.*.cmd *.symvers *~ *.*~ TODO.*
	@rm -fR .tmp*
	@rm -rf .tmp_version