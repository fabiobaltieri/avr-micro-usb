KERNELDIR ?= $(HOME)/linux
PWD := $(shell pwd)

obj-m := avr-usb-led.o

default:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules

clean:
	rm -f *.o *.ko modules.order Module.symvers *.mod.c *.mod.o \.*.cmd
	rm -rf .tmp_versions
