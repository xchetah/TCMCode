#ifndef __CN1100_H__ //CN1100_LINUX_H
#define __CN1100_H__

#ifdef CN1100_LINUX //CN1100_LINIUX
#include <linux/miscdevice.h>
#include <linux/types.h>
#include <linux/mutex.h>
#include <linux/input/mt.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <linux/ctype.h>
#include <linux/err.h>
#include <linux/wait.h>
#include <linux/sched.h>
#include <linux/interrupt.h>
#include <linux/proc_fs.h>
#include <linux/input.h>
#include <linux/platform_device.h>
#include <linux/workqueue.h>
#include <linux/irq.h>
#include <linux/delay.h>
#include <linux/spi/spi.h>
#include <linux/earlysuspend.h>
#include <mach/gpio.h>
#include <linux/module.h>
#include <linux/gpio.h>
#ifdef CN1100_ATM
#include <linux/regulator/consumer.h>
#endif
#ifdef CN1100_WINNER
#include <linux/init-input.h>
#include <asm/gpio.h>
#endif
#ifdef CN1100_S5PV210
#include <plat/irqs.h>
#endif
#include <asm/uaccess.h>

#ifdef CN1100_ATM
#define CFG_TP_USE_CONFIG 0
#endif

#define PROC_CMD_MIN 0
#define PROC_WRITE_CFG 0
#define PROC_READ_CFG 1
#define PROC_READ_ALL 2
#define PROC_READ_RAW 3
#define PROC_READ_DIFF 4
#define PROC_READ_BASE 5
#define PROC_READ_MANUAL 7
#define PROC_READ_DEBUG 8
#define PROC_READ_INFO 9
#define PROC_READ_COORDINATE 6
#define PROC_CMD_MAX 20

//define all mode cn1100 need
#define RAW_READ_MODE		(1 << 0)
#define DIFF_READ_MODE		(1 << 1)
#define BASE_READ_MODE		(1 << 2)
#define ALL_READ_MODE		(1 << 3)
#define CFG_READ_MODE		(1 << 4) 
#define SHELL_READ_MODE		(1 << 5)
#define COOR_READ_MODE		(1 << 6)
#define MANUAL_READ_MODE	(1 << 7)
#define DEBUG_READ_MODE		(1 << 8)
#define INFO_READ_MODE		(1 << 9)

#define CN1100_USE_IRQ		(1 << 16)
#define CN1100_DATA_PREPARED	(1 << 17)
#define CN1100_IRQ_DEAD		(1 << 18)
#define CN1100_CHIP_DEAD	(1 << 19)
#define CN1100_IS_SUSPENDED	(1 << 20)
#define CN1100_IS_DOZE		(1 << 21)
//define mode end

#define CN1100_IRQ_ENABLE(status) do{\
					if(status){\
						enable_irq(spidev->irq);\
					}else{\
						disable_irq_nosync(spidev->irq);\
					}\
				    }while(0);

#define FRAME_HANDLE_SPEED_NORMAL 12500000
#define SCAN_SYSTIC_INTERVAL 1000000
//#define CN1100_RESET_ENABLE

#define CN1100_HANDLE_INTERVAL 25000000

#define CN1100_LINUX_SPI 0//only one kind of transmission  can be used at one time 

#if(CN1100_LINUX_SPI)
/*Define SPI*/
#define SPI_LOW_SPEED   500*1000
#define SPI_FAST_SPEED  1000*1000
#define SPI_HIGH_SPEED	2000*1000
#define CN1100_IOC_RD_SPI_SPEED _IOR(CN1100_IOC_MAGIC,1,__u8)
#define CN1100_IOC_WR_SPI_SPEED _IOW(CN1100_IOC_MAGIC,1,__u8) 
#define SPI_BITS_PER_WORD 8
#define SPI_DUMMY_BYTE 0xA5
#define SPI_CN1100_DEBUG 1
#define SPI_DEVICE_NAME "s3c64xx-spi"
#define SMDK_MMCSPI_CS 0
#define CN1100_SPI_BUS 1
#define CN1100_LINUX_I2C 0
/*SPI End*/
#else
/*Define I2C*/
#define CN1100_LINUX_I2C 1
#define CN1100_I2C_BUS 2

#ifdef CN1100_RK3026
#define CN1100_DEVICE_ADDR 0x5d
#endif
#ifdef CN1100_S5PV210
#define CN1100_DEVICE_ADDR 0x20
#endif

#define I2C_MSGS 2
#define CN1100_IOC_RD_I2C_SPEED _IOR(CN1100_IOC_MAGIC,2,__u8)
#define CN1100_IOC_WR_I2C_SPEED _IOW(CN1100_IOC_MAGIC,2,__u8)
/*I2C End*/
#endif

typedef struct {
	uint16_t base[XMTR_NUM][RECV_NUM];
	uint16_t raw[XMTR_NUM][RECV_NUM];
	int16_t diff[XMTR_NUM][RECV_NUM];
}data_t;

typedef struct {
	char		type;
	int16_t		index;
	int		value;
} cn1100_t;

struct cn1100_spi_dev {
	uint32_t mode;
	int irq;
	uint32_t irq_count;
	int frames_undo;
	wait_queue_head_t waitq;
	struct input_dev *dev;
	struct proc_dir_entry *chm_ts_proc;
#if(CN1100_LINUX_SPI)
        struct spi_device *spi;
#elif(CN1100_LINUX_I2C)
	struct i2c_client *client;
#endif
	struct hrtimer systic;
	struct early_suspend early_suspend;

	struct workqueue_struct *workqueue;
	struct work_struct main;
	struct work_struct reset_work;
};


extern struct cn1100_spi_dev *spidev;
extern uint16_t chip_addr;
#endif //CN1100_LINUX
#endif//CN1100_LINUX_H
