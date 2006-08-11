KERNEL_DIR=/lib/modules/2.6.15.7/build/
RTAI_INSTALL_DIR=/usr/realtime/rtai-3.3/
OROCOS_INSTALL_DIR=/src/orocos-trunk/build-lxrt/packages/install

CPPFLAGS+=-I$(RTAI_INSTALL_DIR)/include -I$(RTNET_INSTALL_DIR)/include -I$(OROCOS_INSTALL_DIR)/include
CFLAGS+=-march=pentium4 -mcpu=pentium4

obj-m := apci_lxrt.o

all:
	@cat README

driver:
	make -C $(KERNEL_DIR) SUBDIRS=$(PWD) modules
	make -C $(KERNEL_DIR) SUBDIRS=$(PWD)/apci1710 modules
	make -C $(KERNEL_DIR) SUBDIRS=$(PWD)/apci1032 modules
	make -C $(KERNEL_DIR) SUBDIRS=$(PWD)/apci2200 modules

clean:
	make -C $(KERNEL_DIR) SUBDIRS=$(PWD) clean
	make -C $(KERNEL_DIR) SUBDIRS=$(PWD)/apci1710 clean
	make -C $(KERNEL_DIR) SUBDIRS=$(PWD)/apci1032 clean
	make -C $(KERNEL_DIR) SUBDIRS=$(PWD)/apci2200 clean


# type "make -C /usr/src/linux-2.6.9_adeos-r9c3/ SUBDIRS=$PWD modules"
