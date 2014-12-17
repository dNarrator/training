obj-m := hello1.o
KDIR := /lib/modules/$(shell uname -r)/build
INC :=-I/usr/include
PWD := $(shell pwd)
default:
	$(MAKE) -C $(KDIR) $(INC) SUBDIRS=$(PWD) modules
