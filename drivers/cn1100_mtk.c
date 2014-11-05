/*
 * Report_Coordinate

 * SPI_write_DATAs
 */
#include "CN1100_common.h"
#include "CN1100_linux.h"
#include "CN1100_Function.h"

#define GTP_SUPPORT_I2C_DMA  1
#if GTP_SUPPORT_I2C_DMA
	#include <linux/dma-mapping.h>
#endif

int SCREEN_HIGH = 800;
int SCREEN_WIDTH = 480;
int CN1100_RESET_PIN=GPIO_CTP_RST_PIN;
int CN1100_INT_PIN=CUST_EINT_TOUCH_PANEL_NUM;
#define I2C_MASTER_CLOCK     400
#define MAX_TRANSACTION_LENGTH 8
#define MAX_I2C_TRANSFER_SIZE 6
#define GTP_ADDR_LENGTH  2
#define I2C_DMA_MAX		250

#define  ADD_I2C_DEVICE_ANDROID_4_0
struct cn1100_spi_dev *spidev = NULL;
uint16_t chip_addr = 0x5d;
static char eint_flag = 0;
static int tpd_flag = 0;
static struct task_struct *thread = NULL;
static int bytes_per_frame = 0;
extern struct tpd_device *tpd;

#if GTP_SUPPORT_I2C_DMA
static u8 *gpDMABuf_va = NULL;
static u32 gpDMABuf_pa = NULL;
#endif


#ifdef ADD_I2C_DEVICE_ANDROID_4_0
static struct i2c_board_info __initdata i2c_tpd={ I2C_BOARD_INFO("tc1126", (0x20))};
#else
static unsigned short force[] = {0, (0x20 << 1), I2C_CLIENT_END,I2C_CLIENT_END};
static const unsigned short * const forces[] = { force, NULL };
static struct i2c_client_address_data addr_data = { .forces = forces,};
#endif

extern void mt_eint_unmask(unsigned int line);
extern void mt_eint_mask(unsigned int line);

void update_cfg(void){
	int i = 0;
	cn1100_t config[] = { 
#include "chm_ts.cfg" 
	};  
	for(i = 0;i < ARRAY_SIZE(config);i++){
		chm_ts_write_config(config[i]);
		msleep(10);
	}   
	bdt.BFD.InitCount = 0;
}

int SPI_read_singleData(uint32_t addr)
{
	/*Test to see if sth wrong,change FAST_READ_DATA to READ_CHIP_ID*/
	int16_t ret = 0;
	uint8_t tmp[] = {
		((addr>>8)&0xff),addr&0xff,
	};
	struct i2c_msg *msgs;
	int status = 0;
	uint8_t rx[8] = {0};
	msgs = kmalloc(sizeof(struct i2c_msg)*I2C_MSGS,GFP_KERNEL);
	msgs[0].addr = chip_addr;
	msgs[0].ext_flag = I2C_ENEXT_FLAG,
	msgs[0].flags = 0;
	msgs[0].buf = tmp;
	msgs[0].len = ARRAY_SIZE(tmp);
	msgs[0].timing = I2C_MASTER_CLOCK;


	msgs[1].addr = chip_addr;
	msgs[1].flags =  I2C_M_RD;
	msgs[1].ext_flag = I2C_ENEXT_FLAG,
	msgs[1].buf = rx;
	msgs[1].len = 2; 
	msgs[1].timing = I2C_MASTER_CLOCK;

	if(!spidev->client){
		CN1100_print("cannot get i2c adapter\n");
		ret = -1;
		goto out;
	}
	ret = 0;
	while(ret < 3){
		status = i2c_transfer(spidev->client->adapter,msgs,2);
		if(status !=  2){
			ret++;
			CN1100_print("========I2C_READ_ERROR==========\n");
		}else{
			break;
		}
	}
	if(ret >= 3){
		CN1100_print("failed to send i2c message:%d\n",status);
		ret = status;
		goto out;
	}
	ret = rx[0] << 8;
	ret +=rx[1];
out:
	kfree(msgs);
	return ret;
}

int SPI_read_DATAs(uint32_t addr, uint16_t num, uint16_t *data)
{
	u8 buffer[2];
	u8 rx[512] = {0};
	u16 partial = (2*num)%bytes_per_frame;
	u16 total = ((partial==0)?((2*num)/bytes_per_frame):(((2*num)/bytes_per_frame)+1))+1;
	int last = 0;
	int i=0,j = 0,ret=0;
	struct i2c_msg *msg = NULL;

	buffer[0] = (addr >>8)&0xff;
	buffer[1] = (addr)&0xff;
	if(partial){
		last = partial;	
	}else{
		last = bytes_per_frame;
	}
	msg = kcalloc(total,sizeof(struct i2c_msg),GFP_KERNEL);
	for(i = 0;i < total;i++){
		if(i == 0){
			msg[i].addr = ((chip_addr &I2C_MASK_FLAG) | (I2C_ENEXT_FLAG));
			msg[i].flags = 0;
			msg[i].buf = buffer;
			msg[i].len = 2;
			msg[i].timing = 400;
		}else{
			msg[i].addr = chip_addr;
			msg[i].flags = I2C_M_RD;
			msg[i].len = (i == (total-1)) ? last : bytes_per_frame;
#if GTP_SUPPORT_I2C_DMA
			msg[i].buf = gpDMABuf_pa + bytes_per_frame * (i - 1);
			msg[i].ext_flag = (spidev->client->ext_flag | I2C_ENEXT_FLAG | I2C_DMA_FLAG);
#else
			msg[i].buf = rx+bytes_per_frame*(i-1);
			msg[i].ext_flag = (spidev->client->ext_flag | I2C_ENEXT_FLAG);
#endif
			msg[i].timing = 400;
		}
	}

	ret = i2c_transfer(spidev->client->adapter, msg, total);
#if GTP_SUPPORT_I2C_DMA
	memcpy(rx, gpDMABuf_va, 2*num);
#endif
        j = 0;
        for(i=0;i<num;i++){
                data[i] = (uint16_t)rx[j++]<<8;
                data[i] += (uint16_t)rx[j++];
        }   

	kfree(msg);
	return ret;

}

int SPI_write_singleData(uint32_t addr, uint16_t data)
{
	uint8_t tmp[] = {
		((addr>>8)&0xff),addr&0xff,((data>>8)&0xff),data&0xff,
	};
	struct i2c_msg msg;
	int status = 0,ret = 0;
	msg.addr = chip_addr;
	msg.flags = 0;
	msg.ext_flag = I2C_ENEXT_FLAG,
	msg.buf = tmp;
	msg.len = ARRAY_SIZE(tmp);
	msg.timing = I2C_MASTER_CLOCK;
	if(!spidev->client->adapter){
		CN1100_print("cannot get i2c adapter\n");
		status = -1;
		goto out;
	}
	ret = 0;
	while(ret < 3){
		status = i2c_transfer(spidev->client->adapter,&msg,1);
		if(status !=  1){
			ret++;
			CN1100_print("=======I2C_WRITE_ERROR===========\n");
		}else{
			break;
		}
	}
	if(ret >= 3){
		CN1100_print("failed to send i2c message:%d\n",status);
	}
out:
	return status;
}

int SPI_write_DATAs(uint32_t addr, uint16_t num, uint16_t *data)
{
	int i =0,j=0;
	unsigned char *buf_va = NULL;
	uint8_t tx[512] = {
		0
	};
	uint8_t tmp[2] = {
		(addr>>8)&0xff,addr&0xff,
	};
	struct i2c_msg msg[] = {
		{
			.addr = chip_addr,
			.flags = 0,
			.len = 2*num + 2,
			.timing = 400,
		},
	};

        for(i=0;i<ARRAY_SIZE(tmp);i++){
                tx[i] = tmp[i];
        }   

        for(j=0;j<num;j++){
                tx[i++] = ((data[j]>>8)&0xff);
                tx[i++] = (data[j])&0xff;
        }   
#if GTP_SUPPORT_I2C_DMA
	buf_va = gpDMABuf_va;
	msg[0].buf = gpDMABuf_pa;
	msg[0].ext_flag=(spidev->client->ext_flag|I2C_ENEXT_FLAG|I2C_DMA_FLAG);
#else
	msg[0].buf = tx;
	msg[0].ext_flag=(spidev->client->ext_flag|I2C_ENEXT_FLAG);
#endif

#if GTP_SUPPORT_I2C_DMA
	memcpy(buf_va,tx,2*num+2);
#endif
	i2c_transfer(spidev->client->adapter,&msg,1);
}
void tpd_down( int id, int x, int y)
{
        input_report_key(tpd->dev, BTN_TOUCH, 1);
        input_report_abs(tpd->dev, ABS_MT_TOUCH_MAJOR, 1);
        input_report_abs(tpd->dev, ABS_MT_POSITION_X, x);
        input_report_abs(tpd->dev, ABS_MT_POSITION_Y, y);
        input_report_abs(tpd->dev, ABS_MT_TRACKING_ID, id);
        input_mt_sync(tpd->dev);

        if (FACTORY_BOOT == get_boot_mode()|| RECOVERY_BOOT == get_boot_mode())
        {
        #ifdef TPD_HAVE_BUTTON 
                tpd_button(x, y, 1);
        #endif
        }
}


void tpd_up(void){

        input_report_key(tpd->dev, BTN_TOUCH, 0);
        input_mt_sync(tpd->dev);
     
        if (FACTORY_BOOT == get_boot_mode()|| RECOVERY_BOOT == get_boot_mode())
        {    
        #ifdef TPD_HAVE_BUTTON 
            tpd_button(0, 0, 0);  
        #endif
        }   
}

void Report_Coordinate_Wait4_SingleTime(int id,int X, int Y)
{
	Y  = (uint16_t)(( ((uint32_t)Y) * RECV_SCALE )>>16);
	X  = (uint16_t)(( ((uint32_t)X) * XMTR_SCALE )>>16);

	if(X > 0 || Y > 0){ 
	//	tpd_down(id,SCREEN_HIGH-X,SCREEN_WIDTH-Y);
		tpd_down(id,X,Y);
	}
}
uint16_t FingProc_Dist2PMeasure(int16_t x1, int16_t y1, int16_t x2, int16_t y2);
void Report_Coordinate()
{
	bool finger_exist = false;
	int fnum = FINGER_NUM_MAX;
	int X=0, Y=0, i, count;
	int Wait4Flag = 0;

	for(i=0; i<fnum; i++) {
		if(bdt.DPD[i].JustPassStateFlag4) Wait4Flag = 1;
	}
	if(Wait4Flag)
	{
		 int LongEnoughFlag = 0;
             for(i=0; i<fnum; i++)
             {
                 if(bdt.DPD[i].JustPassStateFlag4)
                 {
                    if(FingProc_Dist2PMeasure(bdt.DPD[i].Prev_Finger_X[3], bdt.DPD[i].Prev_Finger_Y[3], bdt.DPD[i].Prev_Finger_X[0], bdt.DPD[i].Prev_Finger_Y[0]) > 100)
                     LongEnoughFlag = 3;  // 4 points should be reported
                 }
             }

             for(count=LongEnoughFlag; count>=0; count--)
             {

		//*************************************
		// Report Old 4 Point with others
		//*************************************
			for(i=0; i<fnum; i++) 
			{
				if(bdt.DPD[i].JustPassStateFlag4)
				{
					Y  = bdt.DPD[i].Prev_Finger_Y[count];
					X  = bdt.DPD[i].Prev_Finger_X[count];
				}
				else
				{
					Y  = bdt.DPD[i].Finger_Y_Reported; // Y -> RECV (480)
					X  = bdt.DPD[i].Finger_X_Reported; // X -> XTMR (800) 
				}
				Report_Coordinate_Wait4_SingleTime(i,X, Y);
			}
			input_sync(tpd->dev);
			mdelay(1);
		}
	}
	//*************************************
	// Report Old 4 Point with others
	//*************************************
	for(i=0; i<fnum; i++) 
	{    
		Y  = bdt.DPD[i].Finger_Y_Reported; // Y -> RECV (480)
		X  = bdt.DPD[i].Finger_X_Reported; // X -> XTMR (800) 
		if((X>0)||(Y>0)){
			finger_exist = true;
		}
		Report_Coordinate_Wait4_SingleTime(i,X, Y);
	}
	if (!finger_exist){
		tpd_up();
	}
	input_sync(tpd->dev);
}



static int touch_event_handler(void *unused)
{
	struct sched_param param = { .sched_priority = RTPM_PRIO_TPD };
	sched_setscheduler(current, SCHED_RR, &param);
	do
	{
#if 1
		set_current_state(TASK_INTERRUPTIBLE);
		wait_event_interruptible(spidev->syncq, tpd_flag != 0);
		tpd_flag = 0;
		TPD_DEBUG_SET_TIME;
		set_current_state(TASK_RUNNING);

		eint_flag = 0;
		CN1100_FrameScanDoneInt_ISR();
		mt_eint_unmask(CUST_EINT_TOUCH_PANEL_NUM);
#endif
	} while (!kthread_should_stop());

	return 0;
}

void tpd_eint_interrupt_handler(void)
{
	if(spidev->i2c_ok){
#ifdef CAL_TIME_CONSUMED
		if(0 == spidev->irq_interval){
			do_gettimeofday(&spidev->pretime);
			spidev->irq_interval = 1;
		}else{
			do_gettimeofday(&spidev->curtime);
			spidev->irq_interval = (spidev->curtime.tv_sec-spidev->pretime.tv_sec)*1000000+(spidev->curtime.tv_usec-spidev->pretime.tv_usec);
			spidev->pretime = spidev->curtime;
		}   
#endif
		mt_eint_mask(CUST_EINT_TOUCH_PANEL_NUM);
		eint_flag = 1;
		tpd_flag=1;
		wake_up_interruptible(&spidev->syncq);
	}
}

static int tpd_i2c_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
	int status = 0;
	spidev->mode = CN1100_USE_IRQ;
	spidev->client = client;
#if GTP_SUPPORT_I2C_DMA
	bytes_per_frame = 250;
#else
	bytes_per_frame = 8;
#endif

	mt_set_gpio_mode(GPIO_CTP_RST_PIN, GPIO_CTP_RST_PIN_M_GPIO);
	mt_set_gpio_dir(GPIO_CTP_RST_PIN, GPIO_DIR_OUT);
	mt_set_gpio_out(GPIO_CTP_RST_PIN, GPIO_OUT_ZERO);
	msleep(100);
	//      hwPowerOn(TP_POWER, VOL_2800, "TP");
	//      msleep(100);
	mt_set_gpio_out(GPIO_CTP_RST_PIN, GPIO_OUT_ONE);

	mt_set_gpio_mode(GPIO_CTP_EINT_PIN, GPIO_CTP_EINT_PIN_M_EINT);
	mt_set_gpio_dir(GPIO_CTP_EINT_PIN, GPIO_DIR_IN);
	//        mt_set_gpio_pull_enable(GPIO_CTP_EINT_PIN, GPIO_PULL_ENABLE);
	//        mt_set_gpio_pull_select(GPIO_CTP_EINT_PIN, GPIO_PULL_UP);
	msleep(50);

	mt_eint_registration(CUST_EINT_TOUCH_PANEL_NUM, EINTF_TRIGGER_HIGH, tpd_eint_interrupt_handler, 0);
	mt_eint_unmask(CUST_EINT_TOUCH_PANEL_NUM);

	tpd_load_status = 1;
	thread = kthread_run(touch_event_handler, 0, TPD_DEVICE);
	if (IS_ERR(thread)) {
		status = PTR_ERR(thread);
		TPD_DMESG(TPD_DEVICE " failed to create kernel thread: %d\n", status);
	}    


	hrtimer_init(&spidev->systic,CLOCK_MONOTONIC,HRTIMER_MODE_REL);
	spidev->systic.function = CN1100_SysTick_ISR;

	get_chip_addr();
	if(spidev->i2c_ok){
		cn1100_init();
		update_cfg();
	}
	/*Related to specified hardware*/

	spidev->chm_ts_proc = create_proc_entry("chm_ts",0666,NULL);
	if(spidev->chm_ts_proc){
		spidev->chm_ts_proc->read_proc = chm_ts_read_proc;
		spidev->chm_ts_proc->write_proc = chm_ts_write_proc;
	}else{
		printk("failed to create proc directory\n");
	}

	hrtimer_start(&spidev->systic, ktime_set(0, SCAN_SYSTIC_INTERVAL), HRTIMER_MODE_REL);
	return 0;
fail:
	return -1;
}


static int tpd_i2c_detect(struct i2c_client *client, struct i2c_board_info *info) {
	strcpy(info->type, TPD_DEVICE);
	return 0;
}


static int tpd_i2c_remove(struct i2c_client *client)
{
	printk("==tpd_i2c_remove==\n");

	return 0;
}


void tpd_suspend(struct early_suspend *h)
{
	printk("==========%s========\n",__func__);
	mt_eint_mask(CUST_EINT_TOUCH_PANEL_NUM);
	hrtimer_cancel(&spidev->systic);
        mt_set_gpio_mode(GPIO_CTP_RST_PIN, GPIO_CTP_RST_PIN_M_GPIO);
        mt_set_gpio_dir(GPIO_CTP_RST_PIN, GPIO_DIR_OUT);
        mt_set_gpio_out(GPIO_CTP_RST_PIN, GPIO_OUT_ZERO);
}

void tpd_resume(struct early_suspend *h)
{
	printk("==========%s========\n",__func__);
	mt_set_gpio_mode(GPIO_CTP_RST_PIN, GPIO_CTP_RST_PIN_M_GPIO);
        mt_set_gpio_dir(GPIO_CTP_RST_PIN, GPIO_DIR_OUT);
        mt_set_gpio_out(GPIO_CTP_RST_PIN, GPIO_OUT_ONE);
	msleep(50);
	get_chip_addr();
	if(spidev->i2c_ok){
		cn1100_init();
		update_cfg();
	}
	hrtimer_start(&spidev->systic, ktime_set(0, SCAN_SYSTIC_INTERVAL), HRTIMER_MODE_REL);
	mt_eint_unmask(CUST_EINT_TOUCH_PANEL_NUM);
}

static const struct i2c_device_id tpd_i2c_id[] = {
	{"tc1126",0},
	{"cn1100",0},
	{},
};


struct i2c_driver tpd_i2c_driver = {
	.driver		= {
		.name	= "tc1126",
#ifndef ADD_I2C_DEVICE_ANDROID_4_0       
		.owner = THIS_MODULE,
#endif
	},
	.probe		= tpd_i2c_probe,
	.remove		= tpd_i2c_remove,
	.id_table	= tpd_i2c_id,
	.detect  	= tpd_i2c_detect,
#ifndef ADD_I2C_DEVICE_ANDROID_4_0
	.address_data = &addr_data,
#endif
};

static int tpd_local_init(void)
{

#if GTP_SUPPORT_I2C_DMA
	gpDMABuf_va = (u8 *)dma_alloc_coherent(NULL, 512, &gpDMABuf_pa, GFP_KERNEL);
	if(!gpDMABuf_va){
		printk("[Error] Allocate DMA I2C Buffer failed!\n");
	}
#endif
	if(i2c_add_driver(&tpd_i2c_driver)!=0) {
		TPD_DMESG("unable to add i2c driver.\n");
		return -1;
	}
	/*
	   if(tpd_load_status == 0)
	   {
	   TPD_DMESG("add error touch panel driver.\n");
	   i2c_del_driver(&tpd_i2c_driver);
	   return -1;
	   }
	   */
#ifdef TPD_HAVE_BUTTON
	tpd_button_setting(TPD_KEY_COUNT, tpd_keys_local, tpd_keys_dim_local);// initialize tpd button data
#endif
	SCREEN_HIGH = TPD_RES_X;
	SCREEN_WIDTH = TPD_RES_Y;

#if (defined(TPD_WARP_START) && defined(TPD_WARP_END))
	TPD_DO_WARP = 1;
	memcpy(tpd_wb_start, tpd_wb_start_local, TPD_WARP_CNT*4);
	memcpy(tpd_wb_end, tpd_wb_start_local, TPD_WARP_CNT*4);
#endif

#if (defined(TPD_HAVE_CALIBRATION) && !defined(TPD_CUSTOM_CALIBRATION))
	memcpy(tpd_calmat, tpd_calmat_local, 8*4);
	memcpy(tpd_def_calmat, tpd_def_calmat_local, 8*4);
#endif
	tpd_type_cap = 1;

	printk("==tpd_local_init end==\n");
	return 0;
}


static struct tpd_driver_t tpd_device_driver =
{
	.tpd_device_name = "tc1126",
	.tpd_local_init = tpd_local_init,
	.suspend = tpd_suspend,
	.resume = tpd_resume,
#ifdef TPD_HAVE_BUTTON
	.tpd_have_button = 1, 
#else
	.tpd_have_button = 0, 
#endif
};

static int __init tpd_driver_init(void)
{
	bd = kmalloc(sizeof(bd_t),GFP_KERNEL);

	spidev = kmalloc(sizeof(*spidev),GFP_KERNEL);

	spidev->mode &= ~(CN1100_DATA_PREPARED);

	init_waitqueue_head(&spidev->waitq);
	init_waitqueue_head(&spidev->syncq);

	printk("MediaTek tc1126 touch panel driver init");
#ifdef ADD_I2C_DEVICE_ANDROID_4_0
	i2c_register_board_info(1, &i2c_tpd, 1);     
#endif
	if (tpd_driver_add(&tpd_device_driver) < 0) 
		printk("add generic driver failed");

	return 0;
}

static void __exit tpd_driver_exit(void)
{
	printk("MediaTek tc1126 touch panel driver exit");
	//input_unregister_device(tpd->dev);
	tpd_driver_remove(&tpd_device_driver);
}
module_init(tpd_driver_init);
module_exit(tpd_driver_exit);

