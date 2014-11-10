

#KERNELDIR ?= /your/target/source/directory/

USER:=$(shell whoami)
TARGET_NAME=cn1100_linux
PWD := $(shell pwd)
EXTRA_CFLAGS+=-DCN1100_LINUX
SOURCE_PREFIX=stm32/Projects/TC1126Only/

CROSS_COMPILE=$(TOOLCHAIN)
KERNELDIR := $(KERNEL)
EXTRA_CFLAGS+=-D$(MACRO)
DRIVER = drivers/$(DRV)
LIB = $(SLIB)

SOURCE_DIR=/home/$(USER)/Work/TCMCode/stm32/Projects/TC1126Only/
CC = $(CROSS_COMPILE)gcc
LD = $(CROSS_COMPILE)ld
AR = $(CROSS_COMPILE)ar
EXTRA_CFLAGS+=-I/home/$(USER)/Work/TCMCode/stm32/Projects/TC1126Only/
EXTRA_CFLAGS+=-I/home/$(USER)/Work/TCMCode/drivers/
EXTRA_CFLAGS+=-DBUILD_DATE=$(shell date +%s)

ALGO_OBJS := $(patsubst %.c,%.o,$(filter-out %TC1126_hwDebug.c,$(wildcard $(SOURCE_DIR)/*.c)))
ALGO_OBJS := $(notdir $(ALGO_OBJS))
ALGO_OBJS := $(addprefix $(SOURCE_PREFIX),$(ALGO_OBJS))

ifneq ($(KERNELRELEASE),) 
$(TARGET_NAME)-objs := $(DRIVER).o $(ALGO_OBJS) drivers/debug_proc.o
obj-m += $(TARGET_NAME).o
else
ifeq ("$(P)","RK3026")
all:modules $(LIB) 
else

ifeq ("$(P)","LX")
all:modules $(LIB) zImage
else
all:modules $(LIB)
endif

endif

modules:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules
$(LIB):modules
	@$(AR) cvr $@ $(SOURCE_PREFIX)/TC1126_data.o 
	rm -rf /opt/tmp/CN1100/*
	cp -rf * /opt/tmp/CN1100
	chmod 777 -R /opt/tmp/CN1100/*

update:
	rm -rf /opt/tmp/CN1100/*
	cp -rf * /opt/tmp/CN1100
	chmod 777 -R /opt/tmp/CN1100/*

kernel.img:
	cp $(KERNELDIR)/chm_ts_lcd_1024x600 $(KERNELDIR)/.config
	make -C $(KERNELDIR) kernel.img
	cp $(KERNELDIR)/kernel.img /opt/CN1100/out/kernels/kernel_1024x600_$(shell date +%y%m%d%H%M).img
	cp $(KERNELDIR)/chm_ts_lcd_800x480 $(KERNELDIR)/.config
	make -C $(KERNELDIR) kernel.img
	cp $(KERNELDIR)/kernel.img /opt/CN1100/out/kernels/kernel_800x480_$(shell date +%y%m%d%H%M).img
zImage:
	make -C /opt/CN1100/Kernels/Datang/Wensheng/linux-3.10 lc1913_pad_hs_v3_0_ws_l706_defconfig
	make -C /opt/CN1100/Kernels/Datang/Wensheng/linux-3.10 zImage -j8
	cp ${KERNELDIR}/arch/arm/boot/zImage /opt/CN1100/out/
	

tar:
	./mkpkg.sh
clean:
	@rm -rf  rm *.o *.mod.o *~ core .depend *.cmd *.ko *.mod.c .tmp_versions *.order *.symvers .*.cmd $(SOURCE_PREFIX)/*.o $(SOURCE_PREFIX)/.*.cmd *.bin\
		drivers/*.o drivers/*.cmd drivers/.*.cmd stm32/Projects/TC1126Only/*.o stm32/Projects/TC1126Only/*.cmd stm32/Projects/TC1126Only/.*.cmd

.PHONY: modules modules_install clean

endif

