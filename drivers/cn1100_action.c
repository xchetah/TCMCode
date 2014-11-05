/*
 * Report_Coordinate
 * SPI_write_DATAs
 */
#include "CN1100_common.h"
#include "CN1100_linux.h"
#include "CN1100_Function.h"

int SCREEN_HIGH=800;
int SCREEN_WIDTH=480;
int CN1100_RESET_PIN = ASOC_GPIO_PORTB(3);
int CN1100_INT_PIN = IRQ_SIRQ0; 
struct cn1100_spi_dev *spidev = NULL;
uint16_t chip_addr = 0x5d;
////POWER
#define CTP_POWER_ID			("sensor28")//("touchPannel_power")
#define CTP_POWER_MIN_VOL	(3300000)
#define CTP_POWER_MAX_VOL	(3300000)

#if CFG_TP_USE_CONFIG
struct tc1126_cfg_xml {
	unsigned int sirq;
	unsigned int i2cNum;
	unsigned int i2cAddr;
	unsigned int xMax;
	unsigned int yMax;
	unsigned int rotate; 
	unsigned int xRevert;
	unsigned int yRevert;
}; 
static struct tc1126_cfg_xml cfg_xml;
#endif

#ifdef CONFIG_HAS_EARLYSUSPEND
static void chm_ts_early_suspend(struct early_suspend *h);
static void chm_ts_late_resume(struct early_suspend *h);
#endif
volatile int current_val = 0;
static struct regulator *tp_regulator = NULL;
static inline void regulator_deinit(struct regulator *);
static struct regulator *regulator_init(const char *, int, int);

static struct regulator *regulator_init(const char *name, int minvol, int maxvol)
{
	struct regulator *power;

	power = regulator_get(NULL,"sensor28");// name);
	if (IS_ERR(power)) {
		printk("Nova err,regulator_get fail\n!!!");
		return NULL;
	}

	if (regulator_set_voltage(power, minvol, maxvol)) {
		printk("Nova err,cannot set voltage\n!!!");
		regulator_put(power);

		return NULL;
	}
	regulator_enable(power);
	return (power);
}

static inline void regulator_deinit(struct regulator *power)
{
	regulator_disable(power);
	regulator_put(power);
}

#if CFG_TP_USE_CONFIG
static int tp_get_config(void)
{
	int ret = -1;
	int unuse = 0;
	struct gpio_pre_cfg pcfg;
	char * name = "tp_reset";

	printk("\nTP_USE_CONFIG\n");

	memset((void *)&pcfg, 0, sizeof(struct gpio_pre_cfg));
	if (gpio_get_pre_cfg(name, &pcfg)){
		printk("[gslx680] get led gpio failed!\n");
		unuse ++;
	}else {
		CN1100_RESET_PIN = ASOC_GPIO_PORT(pcfg.iogroup, pcfg.pin_num);
	}

	ret = get_config("ctp.sirq", (char *)(&cfg_xml.sirq), sizeof(unsigned int));
	if (ret != 0) {
		printk(KERN_ERR"get irq %d fail\n", cfg_xml.sirq);
		cfg_xml.sirq = CN1100_RESET_PIN;
		unuse ++;
	}

	ret = get_config("ctp.i2cNum", (char *)(&cfg_xml.i2cNum), sizeof(unsigned int));
	if (ret != 0) {
		printk(KERN_ERR"get i2cNum %d fail\n", cfg_xml.i2cNum);
		cfg_xml.i2cNum = 1;
		unuse ++;
	}

	ret = get_config("ctp.i2cAddr", (char *)(&cfg_xml.i2cAddr), sizeof(unsigned int));
	if (ret != 0) {
		printk(KERN_ERR"get i2cAddr %d fail\n", cfg_xml.i2cAddr);
		cfg_xml.i2cAddr = 0x20;
		unuse ++;
	}

	ret = get_config("ctp.xMax", (char *)(&cfg_xml.xMax), sizeof(unsigned int));
	if (ret != 0) {
		printk(KERN_ERR"get xMax %d fail\n", cfg_xml.xMax);
		cfg_xml.xMax = SCREEN_HIGH;
		unuse ++;
	}else{
		SCREEN_HIGH=cfg_xml.xMax;
	}

	ret = get_config("ctp.yMax", (char *)(&cfg_xml.yMax), sizeof(unsigned int));
	if (ret != 0) {
		printk(KERN_ERR"get yMax %d fail\n", cfg_xml.yMax);
		cfg_xml.yMax = SCREEN_WIDTH;
		unuse ++;
	}else{
		SCREEN_WIDTH=cfg_xml.yMax;
	}

	ret = get_config("ctp.xRevert", (char *)(&cfg_xml.xRevert), sizeof(unsigned int));
	if (ret != 0) {
		printk(KERN_ERR"get xRevert %d fail\n", cfg_xml.xRevert);
		cfg_xml.xRevert = 0;
		unuse ++;
	}

	ret = get_config("ctp.yRevert", (char *)(&cfg_xml.yRevert), sizeof(unsigned int));
	if (ret != 0) {
		printk(KERN_ERR"get yRevert %d fail\n", cfg_xml.yRevert);
		cfg_xml.yRevert = 0;
		unuse ++;
	}

	ret = get_config("ctp.rotate", (char *)(&cfg_xml.rotate), sizeof(unsigned int));
	if (ret != 0) {
		printk(KERN_ERR"get rotate %d fail\n", cfg_xml.rotate);
		cfg_xml.rotate = 0;
		unuse ++;
	}
	get_config("ctp_gslX680.i2cAddr", (char *)(&cfg_xml.i2cAddr), sizeof(unsigned int));
	get_config("ctp_gslX680.xMax", (char *)(&cfg_xml.xMax), sizeof(unsigned int));
	get_config("ctp_gslX680.yMax", (char *)(&cfg_xml.yMax), sizeof(unsigned int));
	get_config("ctp_gslX680.xRevert", (char *)(&cfg_xml.xRevert), sizeof(unsigned int));
	get_config("ctp_gslX680.yRevert", (char *)(&cfg_xml.yRevert), sizeof(unsigned int));
	get_config("ctp_gslX680.rotate", (char *)(&cfg_xml.rotate), sizeof(unsigned int));

	printk("--------------------------\n");
	printk("  tp_reset GPIO%c%d\n",65+CN1100_RESET_PIN/32,CN1100_RESET_PIN%32);
	printk("  sirq SIRQ%d\n",((cfg_xml.sirq==IRQ_SIRQ0) ? 0 : cfg_xml.sirq));
	printk("  i2cNum %d\n",cfg_xml.i2cNum);
	printk("  i2cAddr 0x%x\n",cfg_xml.i2cAddr);
	printk("  xMax %d\n",cfg_xml.xMax);
	printk("  yMax %d\n",cfg_xml.yMax);
	printk("  xRevert %d\n",cfg_xml.xRevert);
	printk("  yRevert %d\n",cfg_xml.yRevert);
	printk("  rotate %d\n",cfg_xml.rotate);
	printk("--------------------------\n");

	return unuse;
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
#ifdef CN1100_RK3026
	msgs[1].scl_rate=400*1000;
#endif

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
			CN1100_print("retry:%d\n",ret);
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
#ifdef CN1100_RK3026
	msgs[0].scl_rate=400*1000;
#endif

	msgs[1].addr = chip_addr;
	msgs[1].flags =  I2C_M_RD;
	msgs[1].buf = rx;
	msgs[1].len = 2*num;
#ifdef CN1100_RK3026
	msgs[1].scl_rate=400*1000;
#endif

	if(!spidev->client->adapter){
		CN1100_print("cannot get i2c adapter\n");
		status = -1;
		goto out;
	}
	ret = 0;
	while(ret < 3){
		status = i2c_transfer(spidev->client->adapter,msgs,2);
		if(status !=  2){
			ret++;
			CN1100_print("retry:%d\n",ret);
		}else{
			break;
		}
	}
	if(ret >= 3){
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
#ifdef CN1100_RK3026
	msg.scl_rate=400*1000;
#endif

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
			CN1100_print("retry:%d\n",ret);
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
#ifdef CN1100_RK3026
	msg.scl_rate=400*1000;
#endif

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
			CN1100_print("retry:%d\n",ret);
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

void Report_Coordinate()
{
	int fnum = FINGER_NUM_MAX;
	int X=0,Y=0,i=0;
	for(i=0; i<fnum; i++) {    
		Y  = bdt.DPD[i].Finger_Y_Reported; // Y -> RECV (480)
		X  = bdt.DPD[i].Finger_X_Reported; // X -> XTMR (800) 
		Y  = (uint16_t)(( ((uint32_t)Y) * RECV_SCALE )>>16);
		X  = (uint16_t)(( ((uint32_t)X) * XMTR_SCALE )>>16);

		if(X > 0 || Y > 0){
#if 0 
			if(cfg_xml.rotate == 1){
				int tmp = 0;
				tmp = X;
				X = (SCREEN_HIGH-Y)*SCREEN_WIDTH/SCREEN_HIGH;
				Y = tmp*SCREEN_HIGH/SCREEN_WIDTH;
			}else if(cfg_xml.rotate == 2){
				X = SCREEN_HIGH - X;
				Y = SCREEN_WIDTH - Y;
			}else if(cfg_xml.rotate == 3){
				int tmp = 0;
				tmp = X;
				X = Y*SCREEN_WIDTH/SCREEN_HIGH;
				Y = (SCREEN_WIDTH-tmp)*SCREEN_HIGH/SCREEN_WIDTH;
			}
#endif

			input_report_abs(spidev->dev, ABS_MT_TRACKING_ID,i);
			input_report_abs(spidev->dev, ABS_MT_TOUCH_MAJOR, 5);

			input_report_abs(spidev->dev, ABS_MT_POSITION_X, Y); 
			input_report_abs(spidev->dev, ABS_MT_POSITION_Y, X); 

			input_report_abs(spidev->dev, ABS_MT_WIDTH_MAJOR, 5); 

			input_mt_sync(spidev->dev);
		}else{
			input_mt_sync(spidev->dev);
		}   
	}    
	input_sync(spidev->dev);
}

static irqreturn_t cn1100_irq_handler(int irq, void *dev_id)
{
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
	return IRQ_RETVAL(IRQ_HANDLED);

}


static int chm_ts_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
	int status = 0;
	int retry = 0;
	printk("==========>123%s",__func__);
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

	set_bit(ABS_MT_TOUCH_MAJOR, spidev->dev->absbit);
	set_bit(ABS_MT_POSITION_X, spidev->dev->absbit);
	set_bit(ABS_MT_POSITION_Y, spidev->dev->absbit);              
	set_bit(ABS_MT_WIDTH_MAJOR, spidev->dev->absbit);                     
	set_bit(ABS_MT_TRACKING_ID,spidev->dev->absbit);
	set_bit(EV_ABS, spidev->dev->evbit);                  
	set_bit(EV_KEY, spidev->dev->evbit);       

	input_set_abs_params(spidev->dev, ABS_MT_POSITION_X, 0, SCREEN_WIDTH, 0, 0);  
	input_set_abs_params(spidev->dev, ABS_MT_POSITION_Y, 0, SCREEN_HIGH, 0, 0);  
	input_set_abs_params(spidev->dev, ABS_MT_TOUCH_MAJOR, 0, 255, 0, 0);
	input_set_abs_params(spidev->dev, ABS_MT_WIDTH_MAJOR, 0, 200, 0, 0); 
	input_set_abs_params(spidev->dev,ABS_MT_TRACKING_ID,0,10,0,0);


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
	get_chip_addr();


	cn1100_init();
	update_cfg();
	/*Related to specified hardware*/
	if(spidev->mode & CN1100_USE_IRQ){
		spidev->irq = CN1100_INT_PIN;
		status = request_irq(spidev->irq,cn1100_irq_handler,IRQ_TYPE_LEVEL_HIGH,"cn1100-interupt", NULL);
		if(status < 0){
			printk("Unable to setup irq handler\n");
			goto fail5;
		}
	}
	spidev->chm_ts_proc = create_proc_entry("chm_ts",0666,NULL);
	if(spidev->chm_ts_proc){
		spidev->chm_ts_proc->read_proc = chm_ts_read_proc;
		spidev->chm_ts_proc->write_proc = chm_ts_write_proc;
	}else{
		printk("failed to create proc directory\n");
	}

	hrtimer_start(&spidev->systic, ktime_set(0, SCAN_SYSTIC_INTERVAL), HRTIMER_MODE_REL);
	printk("RESET_PIN:%d,IRQ_NUM:%d\n",CN1100_RESET_PIN,IRQ_SIRQ0);
	return 0;
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
	unregister_early_suspend(&spidev->early_suspend);
	if(spidev->mode & (CN1100_USE_IRQ)){
		disable_irq_nosync(spidev->irq);
	}
	hrtimer_cancel(&spidev->systic);
	flush_workqueue(spidev->workqueue);
	if(bd){
		kfree(bd);
	}
	input_unregister_device(spidev->dev);
	destroy_workqueue(spidev->workqueue);
	free_irq(spidev->irq,NULL);

	gpio_free(CN1100_RESET_PIN);
	if(spidev){
		kfree(spidev);
	}
	return status;
}

int chm_ts_suspend(struct i2c_client *client,pm_message_t mesg)
{
	gpio_set_value(CN1100_RESET_PIN,0);
	if ( tp_regulator ){
		current_val = regulator_get_voltage(tp_regulator);
		regulator_disable(tp_regulator);
		printk("Nova disable regulator %d\n",current_val);
	}
	msleep(20);
	return 0;
}

int chm_ts_resume(struct i2c_client *client)
{
	gpio_set_value(CN1100_RESET_PIN,1);
	msleep(50);
	tp_regulator = regulator_init(CTP_POWER_ID,current_val,current_val);
	msleep(20);
	gpio_set_value(CN1100_RESET_PIN, 1);
	return 0;
}

#ifdef CONFIG_HAS_EARLYSUSPEND
static void chm_ts_early_suspend(struct early_suspend *h){
	spidev->mode |= CN1100_IS_SUSPENDED;
	hrtimer_cancel(&spidev->systic);
	flush_workqueue(spidev->workqueue);
	printk("%s:1\n",__func__);
	disable_irq_nosync(spidev->irq);
	printk("ealy suspend done\n");
}

static void chm_ts_late_resume(struct early_suspend *h){
	printk("%s\n",__func__);
	spidev->mode &= ~(CN1100_IS_SUSPENDED);
	printk("System Return From Deep Sleep\n");
	get_chip_addr();
	msleep(10);
	cn1100_init();
	update_cfg();
	msleep(10);
	if(spidev->mode & CN1100_USE_IRQ){
		enable_irq(spidev->irq);
	}
	hrtimer_start(&spidev->systic, ktime_set(0, SCAN_SYSTIC_INTERVAL), HRTIMER_MODE_REL);
}
#endif

static const struct i2c_device_id chm_ts_id[] = {
	{"tc1126",0},
	{"cn1100",0},
	{},
};

static struct i2c_board_info tp_info = {
	.type	= "tc1126",
	.addr	=  0x20,
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
	int ret;
	struct i2c_adapter *adap = NULL;
	printk("==gsl_ts_init==\n");

#if CFG_TP_USE_CONFIG
	tp_get_config();  //读取config.xml配置信息
	tp_info.addr = cfg_xml.i2cAddr; //获取i2c address
#endif

	tp_regulator = regulator_init(CTP_POWER_ID,CTP_POWER_MIN_VOL, CTP_POWER_MAX_VOL);
	if ( !tp_regulator ) {
		printk("Nova tp init power failed");
		ret = -EINVAL;
		return ret;
	}
	current_val = regulator_get_voltage(tp_regulator);
	printk("====voltage:%d======\n",current_val);


#if CFG_TP_USE_CONFIG
	adap = i2c_get_adapter(cfg_xml.i2cNum); //
#else
	adap = i2c_get_adapter(1);
#endif

	/* binzhang(2012/9/1):  */
	ret = i2c_add_driver(&chm_ts_driver);  //
	printk("i2c_add_driver,ret=%d\n",ret);

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
	i2c_new_device(adap, &tp_info);



	return ret;
fail2:
	kfree(bd);
fail1:
	return -1;
}
module_init(cn1100_spi_init);

static void __exit cn1100_spi_exit(void)
{
	printk("==tc1126_ts_exit==\n");
	i2c_del_driver(&chm_ts_driver);
	if (tp_regulator)
		regulator_deinit(tp_regulator);
	i2c_unregister_device(spidev->client);
	if(spidev){
		kfree(spidev);
	}
	return;

}
module_exit(cn1100_spi_exit);

MODULE_DESCRIPTION("cn1100 spi transfer driver");
MODULE_LICENSE("GPL");
MODULE_ALIAS("spi:cn1100_spi");
