#CONFIG_MODULE_FORCE_UNLOAD=y
#override EXTRA_CFLAGS += -g -O0
obj-m += newmodule.o
obj-m += watchpoint.o

all:
	@echo EXTRA_CFLAGS = $(EXTRA_CFLAGS)
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
