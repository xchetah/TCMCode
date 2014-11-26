/*
 * Report_Coordinate
 * cn1100_reset
 * SPI_write_DATAs
 */
#include "CN1100_common.h"
#include "CN1100_linux.h"
#include "CN1100_Function.h"
//#include "CN1100_data.h"
#define SPI_MODE_MASK   (SPI_CPHA | SPI_CPOL | SPI_CS_HIGH \
		| SPI_LSB_FIRST | SPI_3WIRE | SPI_LOOP \
		| SPI_NO_CS | SPI_READY)


int SCREEN_HIGH=800;
int SCREEN_WIDTH=480;

int CN1100_INT_PIN=S5PV210_GPH1(6);
int CN1100_RESET_PIN=S5PV210_GPD0(3);

#define REPORT_DATA_ANDROID_4_0
struct cn1100_spi_dev *spidev = NULL;
uint16_t chip_addr = 0x5d;

#ifdef PRESS_KEY_DETECT

uint16_t key_pressed = 0;
//static int touch_keys[] = {
//    KEY_BACK,KEY_HOME,KEY_MENU,KEY_SEARCH,
//};

struct keys{
    uint16_t key;
    uint16_t value;
};

static struct keys chm_ts_keys[]={
    {KEY_BACK,TOUCH_KEY_1},
    {KEY_HOMEPAGE,TOUCH_KEY_2},
    {KEY_MENU,TOUCH_KEY_3},
    {KEY_SEARCH,TOUCH_KEY_4},
};

#define MAX_KEY_NUM ((sizeof(chm_ts_keys)/sizeof(chm_ts_keys[0])))

static int touch_key_pressed = 0;
#endif

#ifdef CONFIG_HAS_EARLYSUSPEND
static void chm_ts_early_suspend(struct early_suspend *h);
static void chm_ts_late_resume(struct early_suspend *h);
#endif
static int retries = 1;

#ifdef TPD_PROXIMITY
static int prox_work = 0; 
u8 tpd_proximity_flag = 0; 
u8 tpd_proximity_detect = 1; 
int last_tpd_proximity_state = 1; 
static struct wake_lock ps_lock;
static struct ws_data *ws_datap;

static int tpd_enable_ps(int enable)
{
	if(enable){
		wake_lock(&ps_lock);
		tpd_proximity_flag = 1;
	}else{
		tpd_proximity_flag = 0;
		wake_unlock(&ps_lock);
	}
	return 0;
}

static ssize_t prox_read(struct file *file,char *buf, size_t count, loff_t *ppos)
{
#if 1
	int len, err;
	struct ws_data *data = (struct ws_data*)file->private_data;

	if (!data->prox_event && (!(file->f_flags & O_NONBLOCK))) {
		wait_event_interruptible(data->proximity_event_wait, data->prox_event);
	}

	if (!data->prox_event|| !buf)
		return 0;
	data->prox_state = tpd_proximity_detect;

	len = sizeof(data->prox_state);
	err = copy_to_user(buf, &data->prox_state, len);
	if (err) {
		printk("%s Copy to user returned %d\n" ,__FUNCTION__,err);
		return -EFAULT;
	}

	data->prox_event = 0;
	return len;
#endif
}

static unsigned int prox_poll(struct file *file, struct poll_table_struct *poll)
{
#if 1
	int mask = 0;
	struct ws_data *data = (struct ws_data*)file->private_data;
	poll_wait(file, &data->proximity_event_wait, poll);
	if (data->prox_event){
		mask |= POLLIN | POLLRDNORM;
	}
	return mask;
#endif
}

static long prox_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	int retval = 0;
	unsigned int flag = 0;
	switch (cmd) {
		case    PROXIMITY_SET_DELAY:
			break;
		case PROXIMITY_SET_ENALBE:
			flag = arg ? 1 : 0;
			if (flag)
			{
				printk("enable proximity\n");
				prox_work = 1;
				tpd_enable_ps(1);
			}
			else
			{
				printk("disable proximity\n");
				prox_work = 0;
				tpd_enable_ps(0);
			}
			break;
		default:
			retval = -EINVAL;
	}

	return retval;
}

static int prox_open(struct inode *inode, struct file *file)
{
	file->private_data = ws_datap;
	printk("%s\n",__func__);
	return 0;
}

static int prox_release(struct inode *inode, struct file *file)
{
	file->private_data = NULL;
	printk("%s\n",__func__);
	return 0;
}

const struct file_operations ws_prox_fops = {
	.owner = THIS_MODULE,
	.read = prox_read,
	.poll = prox_poll,
	.unlocked_ioctl = prox_ioctl,
	.open = prox_open,
	.release = prox_release,
};

static int prox_init(void)
{
	int ret = 0;
	ws_datap = kmalloc(sizeof(struct ws_data), GFP_KERNEL);
	if (!ws_datap) {
		printk("%s:Cannot get memory!\n", __func__);
		return -ENOMEM;
	}
	memset(ws_datap,0,sizeof(struct ws_data));
	init_waitqueue_head(&ws_datap->proximity_event_wait);

	ws_datap->prox_dev.minor = MISC_DYNAMIC_MINOR;
	ws_datap->prox_dev.name = "proximity";
	ws_datap->prox_dev.fops = &ws_prox_fops;

	ret = misc_register(&(ws_datap->prox_dev));
	if (ret) {
		printk("%s:Register misc device for light sensor failed(%d)!\n",
				__func__, ret);
		return -1;
	}
	return ret;
}
#endif

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
	msgs[0].flags = 0;
	msgs[0].buf = tmp;
	msgs[0].len = ARRAY_SIZE(tmp);

	msgs[1].addr = chip_addr;
	msgs[1].flags =  I2C_M_RD;
	msgs[1].buf = rx;
	msgs[1].len = 2; 

	if(!spidev->client){
		CN1100_print("cannot get i2c adapter\n");
		ret = -1;
		goto out;
	}
	ret = 0;
	while(ret < retries){
		status = i2c_transfer(spidev->client->adapter,msgs,2);
		if(status !=  2){
			ret++;
			CN1100_print("retry:%d\n",ret);
		}else{
			break;
		}
	}
	if(ret >= retries){
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
	uint32_t i = 0,j = 0;

	uint8_t rx[512] = {0};
	uint8_t tmp[] = {
		(addr>>8)&0xff,(addr)&0xff,
	};
	struct i2c_msg *msgs;
	int status = 0,ret = 0;

	msgs = kmalloc(sizeof(struct i2c_msg)*I2C_MSGS,GFP_KERNEL);

	msgs[0].addr = chip_addr;
	msgs[0].flags = 0;
	msgs[0].buf = tmp;
	msgs[0].len = ARRAY_SIZE(tmp);

	msgs[1].addr = chip_addr;
	msgs[1].flags =  I2C_M_RD;
	msgs[1].buf = rx;
	msgs[1].len = 2*num;

	if(!spidev->client->adapter){
		CN1100_print("cannot get i2c adapter\n");
		status = -1;
		goto out;
	}
	ret = 0;
	while(ret < retries){
		status = i2c_transfer(spidev->client->adapter,msgs,2);
		if(status !=  2){
			ret++;
			CN1100_print("retry:%d\n",ret);
		}else{
			break;
		}
	}
	if(ret >= retries){
		CN1100_print("failed to send i2c message:%d\n",status);
		ret = status;
		goto out;
	}
	j = 0;
	for(i=0;i<num;i++){
		data[i] = (uint16_t)rx[j++]<<8;
		data[i] += (uint16_t)rx[j++];
	}
out:
	kfree(msgs);
	return status;
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
	msg.buf = tmp;
	msg.len = ARRAY_SIZE(tmp);

	if(!spidev->client->adapter){
		CN1100_print("cannot get i2c adapter\n");
		status = -1;
		goto out;
	}
	ret = 0;
	while(ret < retries){
		status = i2c_transfer(spidev->client->adapter,&msg,1);
		if(status !=  1){
			ret++;
			CN1100_print("retry:%d\n",ret);
		}else{
			break;
		}
	}
	if(ret >= retries){
		CN1100_print("failed to send i2c message:%d\n",status);
	}
out:
	return status;
}

int SPI_write_DATAs(uint32_t addr, uint16_t num, uint16_t *data)
{
	uint32_t i =0 ,count = 0,j = 0;
	int status = 0,ret = 0;
	uint8_t tmp[] = {
		((addr>>8)&0xff),(addr&0xff),
	};
	struct i2c_msg msg;
	uint8_t tx[512] = {0};
	for(i=0;i<ARRAY_SIZE(tmp);i++){
		tx[i] = tmp[i];
	}

	for(j=0;j<num;j++){
		tx[i++] = ((data[j]>>8)&0xff);
		tx[i++] = (data[j])&0xff;
	}
	count = i;
	msg.addr = chip_addr;
	msg.flags = 0;
	msg.buf = tx;
	msg.len = count;

	if(!spidev->client->adapter){
		CN1100_print("cannot get i2c adapter\n");
		status = -1;
		goto out;
	}
	ret = 0;
	while(ret < retries){
		status = i2c_transfer(spidev->client->adapter,&msg,1);
		if(status !=  1){
			ret++;
			CN1100_print("retry:%d\n",ret);
		}else{
			break;
		}
	}
	if(ret >= retries){
		CN1100_print("failed to send i2c message:%d\n",status);
	}
out:
	return status;
}

#ifdef PRESS_KEY_DETECT


void set_finger_num(int num)
{
    if(bdt.FingerReqNum != num){
        bdt.FingerReqNum = num;
    }else{
        bdt.FingerReqNum = 1;
    }
    printk("bdt.FingerReqNum:%d\n",bdt.FingerReqNum);
}

#define KEY1_THRESH 300
#define KEY2_THRESH 300
#define KEY3_THRESH 200
#define KEY4_THRESH 300
void DataProc_PressKeyDetect()
{
    bdt.PressKeyFlag1          = NO_KEY_PRESSED;
    //printk("KEY1:(%-5d,%-5d),KEY2:(%-5d,%-5d),KEY3:(%-5d),KEY4:(%-5d,%-5d)\n",bdt.DeltaDat_kp[2],bdt.DeltaDat_kp[3],bdt.DeltaDat_kp[7],bdt.DeltaDat_kp[8],bdt.DeltaDat_kp[5],bdt.DeltaDat_kp[12],bdt.DeltaDat_kp[13]);
    if((bdt.DeltaDat_kp[2] > KEY1_THRESH)|| (bdt.DeltaDat_kp[3] > KEY1_THRESH))
    {
		  bdt.MTD.NoFingerCnt4Doze = 0;
          bdt.PressKeyFlag1 = TOUCH_KEY_1;
    }

    if((bdt.DeltaDat_kp[7] > KEY2_THRESH)|| (bdt.DeltaDat_kp[8] > KEY2_THRESH))
    {
		  bdt.MTD.NoFingerCnt4Doze = 0;
          bdt.PressKeyFlag1 = TOUCH_KEY_2;
    }

    if((bdt.DeltaDat_kp[5] > KEY3_THRESH))
    {
		  bdt.MTD.NoFingerCnt4Doze = 0;
          bdt.PressKeyFlag1 = TOUCH_KEY_3;
    }

    if((bdt.DeltaDat_kp[12] > KEY4_THRESH)|| (bdt.DeltaDat_kp[13] > KEY4_THRESH))
    {
		  bdt.MTD.NoFingerCnt4Doze = 0;
          bdt.PressKeyFlag1 = TOUCH_KEY_4;
    }
}
void report_key(void)
{
    int i = 0;
    for(i = 0;i < MAX_KEY_NUM;i++){
        if(chm_ts_keys[i].value == bdt.PressKeyFlag1){
            input_report_key(spidev->dev,chm_ts_keys[i].key,1);
            key_pressed = chm_ts_keys[i].key;
            touch_key_pressed = 1;
            printk("KEY_PRESSED:%d\n",chm_ts_keys[i].key);
            break;
        }
    }
}
#endif

void Report_Coordinate_Wait4_SingleTime(int id,int X, int Y)
{
     Y  = (uint16_t)(( ((uint32_t)Y) * RECV_SCALE )>>16);
     X  = (uint16_t)(( ((uint32_t)X) * XMTR_SCALE )>>16);
        
    if(X > 0 || Y > 0){ 
#ifdef REPORT_DATA_ANDROID_4_0
        input_mt_slot(spidev->dev,id);
        input_mt_report_slot_state(spidev->dev,MT_TOOL_FINGER,true);
#endif
        input_report_abs(spidev->dev, ABS_MT_TRACKING_ID,id);
        input_report_abs(spidev->dev, ABS_MT_TOUCH_MAJOR, 5); 
#if 0
        input_report_abs(spidev->dev, ABS_MT_POSITION_X, X);
#else
        input_report_abs(spidev->dev, ABS_MT_POSITION_X, SCREEN_HIGH-X);
#endif
#if 1 
        input_report_abs(spidev->dev, ABS_MT_POSITION_Y, SCREEN_WIDTH-Y); 
#else
        input_report_abs(spidev->dev, ABS_MT_POSITION_Y, Y); 
#endif
        input_report_abs(spidev->dev, ABS_MT_WIDTH_MAJOR, 5); 
#ifndef REPORT_DATA_ANDROID_4_0
        input_mt_sync(spidev->dev);
#endif
    }else{
#ifdef REPORT_DATA_ANDROID_4_0
        input_mt_slot(spidev->dev,id);
        input_mt_report_slot_state(spidev->dev,MT_TOOL_FINGER,false);
#else
        input_mt_sync(spidev->dev);
#endif
    }   
}
uint16_t FingProc_Dist2PMeasure(int16_t x1, int16_t y1, int16_t x2, int16_t y2);
void Report_Coordinate()
{
    int fnum = FINGER_NUM_MAX;
    int X=0, Y=0, i, count;
    int Wait4Flag = 0;

    for(i=0; i<fnum; i++) {
        if(bdt.DPD[i].JustPassStateFlag4) Wait4Flag = 1;
    }
    #ifdef PRESS_KEY_DETECT
    if(bdt.PressKeyFlag1){
        report_key();    
        bdt.PressKeyFlag1 = 0;
        return;
    }
    if(touch_key_pressed&&(!bdt.PressKeyFlag1)){
        input_report_key(spidev->dev,key_pressed,0);
        input_sync((spidev->dev));
		if(key_pressed == KEY_MENU){
			set_finger_num(5);
		}
        key_pressed = 0;
        touch_key_pressed = 0;
        printk("key released\n");
    }

    #endif

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

        //*************************************
        // Report Old 4 Point with others
        //*************************************
        for(count=LongEnoughFlag; count>=0; count--)
        { 
            //count=0;
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
            input_sync(spidev->dev);
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
        Report_Coordinate_Wait4_SingleTime(i,X, Y);
    }
    input_sync(spidev->dev);
}


static irqreturn_t cn1100_irq_handler(int irq, void *dev_id)
{
    if(spidev->i2c_ok){
		if((bdt.ModeSelect==DOZE_MODE)&&(spidev->irq_count<10)){
			goto out;
		}
        disable_irq_nosync(spidev->irq);
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
        queue_work(spidev->workqueue,&spidev->main);
    }
out:
    return IRQ_RETVAL(IRQ_HANDLED);
}

#ifdef DEBUG_PROC_USED
static int chm_proc_open(struct inode *inode, struct file *file)
{
    return 0;
}

int chm_proc_release(struct inode *inode, struct file *file)
{
    return 0;
}

static const struct file_operations chm_proc_fops = {
    .owner      = THIS_MODULE,
    .open       = chm_proc_open,
    .read       = chm_proc_read,
    .write      = chm_proc_write,
    .release    = chm_proc_release,
};
#endif

static int chm_ts_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
    int status = 0;
    int index = 0;
#if defined(CN1100_S5PV210)
    int retry = 0;
#endif
    spidev->mode = CN1100_USE_IRQ;
    spidev->client = client;
    if (!i2c_check_functionality(spidev->client->adapter, I2C_FUNC_I2C)) {
        dev_err(&spidev->client->dev, "I2C functionality not supported\n");
        return -ENODEV;
    }    

    /*
     *Get reset pin,and initialize the as an output
     *with value 0 to disable chip function
     *Maybe RESET PIN will not needed anymore
     */

#if defined(CN1100_S5PV210) || defined(CN1100_NUF)
    gpio_free(CN1100_RESET_PIN);
    status = gpio_request(CN1100_RESET_PIN,"pin reset");
    if(status < 0){
        printk("can not get reset pin\n");
        return -1;
    }

    status = gpio_direction_output(CN1100_RESET_PIN,0);
    if(status < 0){
        printk("unable to set reset pin as output\n");
        return -1;
    }
    status = gpio_get_value(CN1100_RESET_PIN);
    if(status){
        printk("cannot set reset pin value");
    }
    msleep(100);
#endif

    spidev->workqueue = create_singlethread_workqueue("spi_read_cn1100");
    if(spidev -> workqueue == NULL){
        printk("Unable to create workqueue\n");
        goto fail2;
    }

    INIT_WORK(&spidev->main,CN1100_FrameScanDoneInt_ISR);
    INIT_WORK(&spidev->reset_work,chm_ts_reset_func);

    /*Allocate an input device to report coordinate to android*/
    spidev->dev = input_allocate_device();
    if(!spidev->dev){
        printk("cannot get input device\n");
        goto fail3;
    }
#ifdef REPORT_DATA_ANDROID_4_0
    __set_bit(EV_ABS,spidev->dev->evbit);
    __set_bit(EV_KEY,spidev->dev->evbit);
    __set_bit(EV_REP,spidev->dev->evbit);
    __set_bit(INPUT_PROP_DIRECT,spidev->dev->propbit);
    input_mt_init_slots(spidev->dev,11);
#else
    input_set_abs_params(spidev->dev,ABS_MT_TRACKING_ID,0,10,0,0);
    set_bit(EV_ABS, spidev->dev->evbit);                  
    set_bit(EV_KEY, spidev->dev->evbit);       
#endif

    set_bit(ABS_MT_TOUCH_MAJOR, spidev->dev->absbit);
    set_bit(ABS_MT_POSITION_X, spidev->dev->absbit);
    set_bit(ABS_MT_POSITION_Y, spidev->dev->absbit);              
    set_bit(ABS_MT_WIDTH_MAJOR, spidev->dev->absbit);                     
    set_bit(ABS_MT_TRACKING_ID,spidev->dev->absbit);
#ifdef PRESS_KEY_DETECT
    for (index = 0; index < MAX_KEY_NUM; index++)
    {    
        input_set_capability(spidev->dev,EV_KEY,chm_ts_keys[index].key);  
    }    
#endif

    input_set_abs_params(spidev->dev, ABS_MT_POSITION_X, 0, SCREEN_HIGH, 0, 0);  
    input_set_abs_params(spidev->dev, ABS_MT_POSITION_Y, 0, SCREEN_WIDTH, 0, 0);  
    input_set_abs_params(spidev->dev, ABS_MT_TOUCH_MAJOR, 0, 255, 0, 0);
    input_set_abs_params(spidev->dev, ABS_MT_WIDTH_MAJOR, 0, 200, 0, 0); 


    spidev->dev->name = "chm_ts";
    spidev->dev->phys = "input/ts";
    spidev->dev->id.bustype = BUS_I2C;
    spidev->dev->id.vendor = 0xDEAD;
    spidev->dev->id.product = 0xBEEF;
    spidev->dev->id.version = 0x0101;

    status = input_register_device(spidev->dev);
    if(status){
        printk("Cannot register input device\n");
        goto fail4;
    }	

#ifdef TPD_PROXIMITY
    prox_init();
#endif
    hrtimer_init(&spidev->systic,CLOCK_MONOTONIC,HRTIMER_MODE_REL);
    spidev->systic.function = CN1100_SysTick_ISR;


#ifdef CONFIG_HAS_EARLYSUSPEND
    spidev->early_suspend.level =  EARLY_SUSPEND_LEVEL_BLANK_SCREEN + 1;
    spidev->early_suspend.suspend = chm_ts_early_suspend;
    spidev->early_suspend.resume = chm_ts_late_resume;
    register_early_suspend(&spidev->early_suspend);
#endif

    gpio_free(CN1100_INT_PIN);
    status = gpio_request(CN1100_INT_PIN,"cn1100-interrupt");	
    if(status < 0){
        printk("Cannot Request GPIO/%d\n",CN1100_INT_PIN);
        goto fail5;
    }
#if defined(CN1100_S5PV210)
    if(!gpio_get_value(CN1100_RESET_PIN)){
        while(retry < 10){
            gpio_set_value(CN1100_RESET_PIN,1);
            status = gpio_get_value(CN1100_RESET_PIN);
            if(!status){
                printk("failed to set RESET_PIN\n");
                retry++;
            }else{
                retry = 0;
                msleep(200);
                printk("cn1100 prepared\n");
                break;
            }
            msleep(10);
        }
    }
#endif
    get_chip_addr();

    spidev->ticks = 0;
    if(spidev->i2c_ok){
        cn1100_init();
        update_cfg();
    }
    /*Related to specified hardware*/
    if(spidev->mode & CN1100_USE_IRQ){
#ifndef CN1100_NUF
        spidev->irq = gpio_to_irq(CN1100_INT_PIN);
        if(spidev->irq < 0){
            printk("Cannot get request IRQ\n");
        }
#else
        spidev->irq = CN1100_INT_PIN;
#endif

        if(spidev->irq){

#ifndef CN1100_NUF
            status = gpio_direction_input(CN1100_INT_PIN);
            if(status < 0){
                printk("Unable set GPIO/%d as input\n",CN1100_INT_PIN);
                goto fail6;
            }
#endif

            status = request_irq(spidev->irq,cn1100_irq_handler,IRQF_TRIGGER_HIGH | IRQF_DISABLED,"cn1100-interrup", NULL);
            if(status < 0){
                printk("Unable to setup irq handler\n");
                goto fail6;
            }
        }
    }

#ifdef DEBUG_PROC_USED
    spidev->chm_ts_proc = proc_create("chm_ts",0666,NULL,&chm_proc_fops);
    if(!spidev->chm_ts_proc){
        printk("failed to create proc directory\n");
    }
#endif

    hrtimer_start(&spidev->systic, ktime_set(0, SCAN_SYSTIC_INTERVAL), HRTIMER_MODE_REL);
    return 0;
fail6:
    if(spidev->mode &(CN1100_USE_IRQ)){
        gpio_free(CN1100_INT_PIN);	
    }
    unregister_early_suspend(&spidev->early_suspend);
fail5:
    input_unregister_device(spidev->dev);
fail4:
    input_free_device(spidev->dev);	
fail3:
    destroy_workqueue(spidev->workqueue);
fail2:
    kfree(spidev);
    return status;
}



static int chm_ts_remove(struct i2c_client *client)
{
    int status = 0;
    printk("driver is removing\n");
    remove_proc_entry("chm_ts", NULL);
#ifdef CONFIG_HAS_EARLYSUSPEND
    unregister_early_suspend(&spidev->early_suspend);
#endif
#ifdef TPD_PROXIMITY
    misc_deregister(&(ws_datap->prox_dev));
#endif
    if(spidev->mode & (CN1100_USE_IRQ)){
        disable_irq_nosync(spidev->irq);
    }
    hrtimer_cancel(&spidev->systic);
    flush_workqueue(spidev->workqueue);

    kfree(bd);
    spidev->client = NULL;
    input_unregister_device(spidev->dev);
    destroy_workqueue(spidev->workqueue);
    if(spidev->mode & CN1100_USE_IRQ){
        free_irq(spidev->irq,NULL);
        gpio_free(CN1100_INT_PIN);
    }
#if defined(CN1100_S5PV210)
    gpio_free(CN1100_RESET_PIN);
#endif
    if(spidev){
        kfree(spidev);
    }
    return status;
}


int chm_ts_suspend(struct i2c_client *client,pm_message_t mesg)
{
    int ret = 0;
#if defined(SLEEP_EVENT_SIM)
    int retry = 0;
#endif
    spidev->mode |= CN1100_IS_SUSPENDED;
    hrtimer_cancel(&spidev->systic);
    flush_workqueue(spidev->workqueue);
    printk("%s:1\n",__func__);
#ifndef SLEEP_EVENT_SIM
    disable_irq_nosync(spidev->irq);
#if defined(CN1100_S5PV210)
    gpio_set_value(CN1100_RESET_PIN,0);
#endif
#else
    spidev->irq_count = 255;
    bdt.Prepare2SleepMode = 1;
    spidev->mode &= ~CN1100_IS_SUSPENDED;
    ret = wait_event_timeout(spidev->waitq,(spidev->mode & CN1100_IS_SUSPENDED),msecs_to_jiffies(500));
    spidev->mode &= ~(CN1100_IS_SUSPENDED);
    if(!ret){
        printk("INT->1 TIMEOUT\n");
        goto out;
    }
    printk("%s:2\n",__func__);

    while(retry < 10){
        if(gpio_get_value(CN1100_INT_PIN)){
            break;
        }
        retry++;
        msleep(50);
    }
    if(retry >= 10){
        printk("Waiting irq time out\n");
        goto out;
    }
    enable_irq(spidev->irq);
    /* After Setting Sleep Mode,Waiting Until It Complete*/
    if(spidev->mode & CN1100_USE_IRQ){
        ret = wait_event_timeout(spidev->waitq,(spidev->mode & CN1100_IS_SUSPENDED),msecs_to_jiffies(500));
        spidev->mode &= ~(CN1100_IS_SUSPENDED);
        if(!ret){
            printk("INT->2 TIMEOUT\n");
            goto out;
        }
        printk("%s:3\n",__func__);
    }
out:
#endif
    printk("suspend done\n");
    return ret;
}

int chm_ts_resume(struct i2c_client *client)
{
    int ret = 0;
    printk("%s\n",__func__);
    spidev->mode &= ~(CN1100_IS_SUSPENDED);


#ifdef SLEEP_EVENT_SIM
    gpio_direction_output(CN1100_INT_PIN,1);
    msleep(50);
    gpio_direction_input(CN1100_INT_PIN);
    msleep(50);
#ifdef CN1100_S5PV210
    s3c_gpio_cfgpin(CN1100_INT_PIN,S3C_GPIO_SFN(0xf));
#endif
    msleep(10);
    //	CN1100_Goto_AutoScanMode(iAUTOSCAN_MODE);
    ret = SPI_read_singleData(0x1f);	
    ret |= FLAG_CHIP_RST;
    SPI_write_singleData(0x1f,(uint16_t)ret);
    msleep(10);
    cn1100_init();

    if(spidev->mode & CN1100_USE_IRQ){
        enable_irq(spidev->irq);
    }
#else
    printk("System Return From Deep Sleep\n");
#if defined(CN1100_S5PV210)
    gpio_set_value(CN1100_RESET_PIN,1);
    msleep(50);
#endif
    get_chip_addr();
    msleep(10);
    if(spidev->i2c_ok){
        cn1100_init();
        update_cfg();
    }
    msleep(10);
    if(spidev->mode & CN1100_USE_IRQ){
        enable_irq(spidev->irq);
    }
#endif
    hrtimer_start(&spidev->systic, ktime_set(0, SCAN_SYSTIC_INTERVAL), HRTIMER_MODE_REL);

    return ret;
}

#ifdef CONFIG_HAS_EARLYSUSPEND
static void chm_ts_early_suspend(struct early_suspend *h){
#ifdef TPD_PROXIMITY
    if (tpd_proximity_flag == 1)
    {
        return;
    }
#endif
    chm_ts_suspend(spidev->client,PMSG_SUSPEND);
}

static void chm_ts_late_resume(struct early_suspend *h){
    chm_ts_resume(spidev->client);
}
#endif

static const struct i2c_device_id chm_ts_id[] = {
    {"tc1126",0},
    {"cn1100",0},
    {},
};


static struct i2c_driver chm_ts_driver = {
    .probe		= chm_ts_probe,
    .remove		= __devexit_p(chm_ts_remove),
    .id_table	= chm_ts_id,
#ifndef CONFIG_HAS_EARLYSUSPEND
    .suspend	= chm_ts_suspend,
    .resume		= chm_ts_resume,
#endif
    .driver		= {
        .name	= "tc1126",
        .owner	= THIS_MODULE, 
    },
};


static int __init cn1100_spi_init(void)
{
    int status = 0;

    bd = kmalloc(sizeof(bd_t),GFP_KERNEL);
    if(!bd){
        goto fail1;
    }

    spidev = kmalloc(sizeof(*spidev),GFP_KERNEL);
    if(!spidev){
        goto fail2;
    }

    spidev->mode &= ~(CN1100_DATA_PREPARED);

    init_waitqueue_head(&spidev->waitq);

#ifdef BUILD_DATE
    printk("Build Time:%u\n",BUILD_DATE);
#endif
    status =  i2c_add_driver(&chm_ts_driver);
    if(status < 0){
        printk("Unable to add an i2c driver\n");
        goto fail3;
    }
    return 0;
fail3:
    kfree(spidev);
fail2:
    kfree(bd);
fail1:
    return status;
}
module_init(cn1100_spi_init);

static void __exit cn1100_spi_exit(void)
{
    i2c_del_driver(&chm_ts_driver);

}
module_exit(cn1100_spi_exit);

MODULE_AUTHOR("chm@ubuntu");
MODULE_DESCRIPTION("cn1100 spi transfer driver");
MODULE_LICENSE("GPL");
MODULE_ALIAS("spi:cn1100_spi");
