modname := terrible
obj-m := $(modname).o

terrible-objs := terriblemod.o terriblekbs.o terminal.o

KVERSION = $(shell uname -r)
KDIR := /lib/modules/$(KVERSION)/build

ccflags-y := -std=gnu99 -Wno-declaration-after-statement
#ccflags-y += -DDEBUG

all:
	make -C $(KDIR) M=$(PWD) modules
	bash -c "/usr/src/kernels/$(shell uname -r)/scripts/sign-file sha256 private_key.priv public_key.der $(modname).ko"

clean:
	make -C $(KDIR) M=$(PWD) clean

load:
	-rmmod $(modname)
	insmod $(modname).ko

unload:
	-rmmod $(modname)
