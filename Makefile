obj-m := drv_uio.o
objs=drv.o test.o
srcs=$(objs:.o=.c)
KERNELDIR ?= /usr/src/linux-headers-5.10.0-8-amd64
PWD := $(shell pwd)

test:$(objs)
	gcc $^ -o $@
	make -C $(KERNELDIR) M=$(PWD) modules
modules:
		make -C $(KERNELDIR) M=$(PWD) modules
modules_install:
		make -C $(KERNELDIR) M=$(PWD) modules_install
clean:
	rm -rf test *.o *~ core .depend  .*.cmd *.ko *.mod.c *.mod .tmp_versions *.symvers *.order
