/*
 * Report_Coordinate
 * cn1100_reset
 * SPI_write_DATAs
 */
#include "CN1100_common.h"
#include "CN1100_linux.h"
#include "CN1100_Function.h"

int SCREEN_HIGH=800;
int SCREEN_WIDTH=480;

int CN1100_INT_PIN=RK30_PIN1_PB0;
/*
 * If RESET_PIN not supported set CN1100_RESET_PIN=-1;
 */
int CN1100_RESET_PIN=RK30_PIN3_PC1;

#define REPORT_DATA_ANDROID_4_0
struct cn1100_spi_dev *spidev = NULL;
uint16_t chip_addr = 0x5d;

#ifdef CONFIG_HAS_EARLYSUSPEND
static void chm_ts_early_suspend(struct early_suspend *h);
static void chm_ts_late_resume(struct early_suspend *h);
#endif
static int retries = 1;

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
	msgs[0].scl_rate=400*1000;

	msgs[1].addr = chip_addr;
	msgs[1].flags =  I2C_M_RD;
	msgs[1].buf = rx;
	msgs[1].len = 2; 
	msgs[1].scl_rate=400*1000;

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
	msgs[0].scl_rate=400*1000;

	msgs[1].addr = chip_addr;
	msgs[1].flags =  I2C_M_RD;
	msgs[1].buf = rx;
	msgs[1].len = 2*num;
	msgs[1].scl_rate=400*1000;

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
	msg.scl_rate=400*1000;

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
	msg.scl_rate=400*1000;

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
#if 0 
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
	return IRQ_RETVAL(IRQ_HANDLED);
}


static int chm_ts_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
	int status = 0;
	int retry = 0;
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

	if(CN1100_RESET_PIN != -1){
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
	}

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

	if(CN1100_RESET_PIN != -1){
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
	}

	get_chip_addr();

	if(spidev->i2c_ok){
		cn1100_init();
		update_cfg();
	}
	/*Related to specified hardware*/
	if(spidev->mode & CN1100_USE_IRQ){
		spidev->irq = gpio_to_irq(CN1100_INT_PIN);
		if(spidev->irq < 0){
			printk("Cannot get request IRQ\n");
		}

		if(spidev->irq){

			status = gpio_direction_input(CN1100_INT_PIN);
			if(status < 0){
				printk("Unable set GPIO/%d as input\n",CN1100_INT_PIN);
				goto fail6;
			}

			status = request_irq(spidev->irq,cn1100_irq_handler,IRQF_TRIGGER_HIGH | IRQF_DISABLED,"cn1100-interrup", NULL);
			if(status < 0){
				printk("Unable to setup irq handler\n");
				goto fail6;
			}
		}
	}

#if DEBUG_PROC_USED
	spidev->chm_ts_proc = create_proc_entry("chm_ts",0666,NULL);
	if(spidev->chm_ts_proc){
		spidev->chm_ts_proc->read_proc = chm_ts_read_proc;
		spidev->chm_ts_proc->write_proc = chm_ts_write_proc;
	}else{
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
	if(CN1100_RESET_PIN != -1){
		gpio_free(CN1100_RESET_PIN);
	}
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
	if(CN1100_RESET_PIN != -1){
		gpio_set_value(CN1100_RESET_PIN,0);
	}
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
		if(CN1100_RESET_PIN != -1){
			gpio_set_value(CN1100_RESET_PIN,1);
			msleep(50);
		}
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
