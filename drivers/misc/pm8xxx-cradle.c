/*
 * Copyright LG Electronics (c) 2011
 * All rights reserved.
 * Author: Fred Cho <fred.cho@lge.com>
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/interrupt.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <linux/mfd/pm8xxx/core.h>
#include <linux/mfd/pm8xxx/cradle.h>
#include <linux/gpio.h>
#include <linux/switch.h>
#include <linux/delay.h>
#include <linux/of_gpio.h>
#include <linux/wakelock.h>
#include <mach/board_lge.h>

#define HALL_DETECT_DELAY 200
#define POUCH_DETECT_DELAY 100

static int pre_set_flag;
struct pm8xxx_cradle {
	struct switch_dev sdev;
	struct delayed_work pouch_work;
	struct device *dev;
	struct wake_lock wake_lock;
	const struct pm8xxx_cradle_platform_data *pdata;
	int pouch;
	spinlock_t lock;
	int state;
#if defined CONFIG_HALLIC_PEN
	int pen;
	struct delayed_work pen_work;
#else
	#if !defined (CONFIG_MACH_MSM8974_T1LTE_GLOBAL_COM) && !defined (CONFIG_MACH_MSM8974_T1WIFI_GLOBAL_COM) && !defined (CONFIG_MACH_MSM8974_T1WIFIN_GLOBAL_COM) && !defined(CONFIG_MACH_MSM8974_T1_ATT)
	int camera;
	struct delayed_work camera_work;
	#endif
#endif
};

static struct workqueue_struct *cradle_wq;
static struct pm8xxx_cradle *cradle;

#if defined(CONFIG_TOUCHSCREEN_ATMEL_S540)
static int is_smart_cover_closed = 0; /* check status of smart cover to resize quick window area */
int cradle_smart_cover_status(void)
{
	return is_smart_cover_closed;/* check status of smart cover to resize quick window area */
}
#endif
#if defined HALLIC_TOUCH_IF
static int smartcover_status;

int get_smartcover_status(void){
	return smartcover_status;
}

static void set_smartcover_status(int status){
	smartcover_status = status;
}
#endif

static void boot_cradle_det_func(void)
{
	int state;

	if (cradle->pdata->hallic_pouch_detect_pin)
		cradle->pouch = !gpio_get_value(cradle->pdata->hallic_pouch_detect_pin);

	printk("%s : boot pouch === > %d \n", __func__ , cradle->pouch);

#if defined CONFIG_HALLIC_PEN
	if (cradle->pdata->hallic_pen_detect_pin)
		cradle->pen = !gpio_get_value(cradle->pdata->hallic_pen_detect_pin);

	printk("%s : boot pen === > %d \n", __func__ , cradle->pen);

	if (cradle->pouch == 1)
		state = SMARTCOVER_POUCH_CLOSED;
	else if (cradle->pen == 1)
		state = SMARTCOVER_PEN_IN;
	else
		state = SMARTCOVER_POUCH_OPENED;
#else

	#if defined (CONFIG_MACH_MSM8974_T1LTE_GLOBAL_COM) || defined (CONFIG_MACH_MSM8974_T1WIFI_GLOBAL_COM) || defined (CONFIG_MACH_MSM8974_T1WIFIN_GLOBAL_COM) || defined(CONFIG_MACH_MSM8974_T1_ATT)
	    if(cradle->pouch == 1)
            state = SMARTCOVER_POUCH_CLOSED;
            else
            state = SMARTCOVER_POUCH_OPENED;
	#else
	if (cradle->pdata->hallic_camera_detect_pin)
		cradle->camera = !gpio_get_value(cradle->pdata->hallic_camera_detect_pin);

	printk("%s : boot camera === > %d \n", __func__ , cradle->camera);

	if (cradle->pouch == 1 && cradle->camera == 1)
		state = SMARTCOVER_CAMERA_VIEW;
	else if (cradle->pouch == 1)
		state = SMARTCOVER_POUCH_CLOSED;
	else if (cradle->camera == 1)
		state = SMARTCOVER_CAMERA_OPENED;
	else
		state = SMARTCOVER_POUCH_OPENED;
	#endif

#endif

	printk("%s : [Cradle] boot cradle value is %d\n", __func__ , state);
	cradle->state = state;
	wake_lock_timeout(&cradle->wake_lock, msecs_to_jiffies(3000));
	switch_set_state(&cradle->sdev, cradle->state);
#if defined(CONFIG_TOUCHSCREEN_ATMEL_S540)
	is_smart_cover_closed = cradle->pouch;
#endif
}

#if defined CONFIG_HALLIC_PEN
static void pm8xxx_pen_work_func(struct work_struct *work)
{
	int state = 0;
	unsigned long flags;

	spin_lock_irqsave(&cradle->lock, flags);

	if (cradle->pdata->hallic_pouch_detect_pin)
		cradle->pouch = !gpio_get_value(cradle->pdata->hallic_pouch_detect_pin);

	if (cradle->pdata->hallic_pen_detect_pin)
		cradle->pen = !gpio_get_value(cradle->pdata->hallic_pen_detect_pin);


	printk("%s : pouch === > %d \n", __func__ , cradle->pouch);
	printk("%s : pen === > %d \n", __func__ , cradle->pen);


	if (cradle->pen == 1)
		state = SMARTCOVER_PEN_IN;
	else if (cradle->pen == 0)
		state = SMARTCOVER_PEN_OUT;

	if (cradle->state != state) {
		cradle->state = state;
		spin_unlock_irqrestore(&cradle->lock, flags);
		wake_lock_timeout(&cradle->wake_lock, msecs_to_jiffies(3000));
		switch_set_state(&cradle->sdev, cradle->state);
		printk("%s : [Cradle] pen value is %d\n", __func__ , state);
	}
	else {
		spin_unlock_irqrestore(&cradle->lock, flags);
		printk("%s : [Cradle] pen value is %d (no change)\n", __func__ , state);
	}
}

#else

#if !defined (CONFIG_MACH_MSM8974_T1LTE_GLOBAL_COM) && !defined (CONFIG_MACH_MSM8974_T1WIFI_GLOBAL_COM) && !defined (CONFIG_MACH_MSM8974_T1WIFIN_GLOBAL_COM) && !defined(CONFIG_MACH_MSM8974_T1_ATT)
static void pm8xxx_camera_work_func(struct work_struct *work)
{
	int state = 0;
	unsigned long flags;
	int tmp_pouch = 0;
	spin_lock_irqsave(&cradle->lock, flags);

	if (cradle->pdata->hallic_camera_detect_pin)
		cradle->camera = !gpio_get_value(cradle->pdata->hallic_camera_detect_pin);
	if (cradle->pdata->hallic_pouch_detect_pin)
		tmp_pouch = !gpio_get_value(cradle->pdata->hallic_pouch_detect_pin);

	printk("%s : camera === > %d \n", __func__ , cradle->camera);
	printk("%s : pouch === > %d \n", __func__ , tmp_pouch);

	if (cradle->camera == 1 && tmp_pouch == 1)
		state = SMARTCOVER_CAMERA_VIEW;
	else if (cradle->camera == 1)
		state = SMARTCOVER_CAMERA_OPENED;
	else if (cradle->camera == 0)
		state = SMARTCOVER_CAMERA_CLOSED;

	if (cradle->state != state) {
		cradle->state = state;
		spin_unlock_irqrestore(&cradle->lock, flags);
		wake_lock_timeout(&cradle->wake_lock, msecs_to_jiffies(3000));
		switch_set_state(&cradle->sdev, cradle->state);
		printk("%s : [Cradle] camera value is %d\n", __func__ , state);
	}
	else {
		spin_unlock_irqrestore(&cradle->lock, flags);
		printk("%s : [Cradle] camera value is %d (no change)\n", __func__ , state);
	}
}
#endif

#endif

static void pm8xxx_pouch_work_func(struct work_struct *work)
{
	int state = 0;
	unsigned long flags;
#if !defined CONFIG_HALLIC_PEN

#if !defined (CONFIG_MACH_MSM8974_T1LTE_GLOBAL_COM) && !defined (CONFIG_MACH_MSM8974_T1WIFI_GLOBAL_COM) && !defined (CONFIG_MACH_MSM8974_T1WIFIN_GLOBAL_COM) && !defined(CONFIG_MACH_MSM8974_T1_ATT)
	int tmp_camera = 0;
#endif

#endif
	spin_lock_irqsave(&cradle->lock, flags);

	if (cradle->pdata->hallic_pouch_detect_pin)
		cradle->pouch = !gpio_get_value(cradle->pdata->hallic_pouch_detect_pin);
	printk("%s : pouch === > %d \n", __func__ , cradle->pouch);
#if defined CONFIG_HALLIC_PEN
	if (cradle->pdata->hallic_pen_detect_pin)
			cradle->pen = !gpio_get_value(cradle->pdata->hallic_pen_detect_pin);
	printk("%s : pen === > %d \n", __func__ , cradle->pen);
#else
	#if !defined (CONFIG_MACH_MSM8974_T1LTE_GLOBAL_COM) && !defined (CONFIG_MACH_MSM8974_T1WIFI_GLOBAL_COM) && !defined (CONFIG_MACH_MSM8974_T1WIFIN_GLOBAL_COM) && !defined(CONFIG_MACH_MSM8974_T1_ATT)
	if (cradle->pdata->hallic_camera_detect_pin)
		tmp_camera = !gpio_get_value(cradle->pdata->hallic_camera_detect_pin);
	printk("%s : camera === > %d \n", __func__ , tmp_camera);
	#endif
#endif

#if defined CONFIG_HALLIC_PEN
	if (cradle->pouch == 1)
		state = SMARTCOVER_POUCH_CLOSED;
	else if (cradle->pouch == 0)
		state = SMARTCOVER_POUCH_OPENED;
#else

	#if defined (CONFIG_MACH_MSM8974_T1LTE_GLOBAL_COM) || defined (CONFIG_MACH_MSM8974_T1WIFI_GLOBAL_COM) || defined (CONFIG_MACH_MSM8974_T1WIFIN_GLOBAL_COM) || defined(CONFIG_MACH_MSM8974_T1_ATT)
	    if (cradle->pouch == 1)
        state = SMARTCOVER_POUCH_CLOSED;
    else if (cradle->pouch == 0)
        state = SMARTCOVER_POUCH_OPENED;
	#else
	if (tmp_camera == 1 && cradle->pouch == 1)
		state = SMARTCOVER_CAMERA_VIEW;
	else if (cradle->pouch == 1)
		state = SMARTCOVER_POUCH_CLOSED;
	else if (cradle->pouch == 0)
		state = SMARTCOVER_POUCH_OPENED;
	#endif

#endif

#if defined HALLIC_TOUCH_IF
	set_smartcover_status(state);
#endif
	if (cradle->state != state) {
		cradle->state = state;
		spin_unlock_irqrestore(&cradle->lock, flags);
		wake_lock_timeout(&cradle->wake_lock, msecs_to_jiffies(3000));
		switch_set_state(&cradle->sdev, cradle->state);
		printk("%s : [Cradle] pouch value is %d\n", __func__ , state);
	}
	else {
		spin_unlock_irqrestore(&cradle->lock, flags);
		printk("%s : [Cradle] pouch value is %d (no change)\n", __func__ , state);
	}
}

void cradle_set_deskdock(int state)
{
	unsigned long flags;

	if (&cradle->sdev) {
		spin_lock_irqsave(&cradle->lock, flags);
		if (cradle->state != state) {
			cradle->state = state;
			spin_unlock_irqrestore(&cradle->lock, flags);
			wake_lock_timeout(&cradle->wake_lock, msecs_to_jiffies(3000));
			switch_set_state(&cradle->sdev, cradle->state);
		}
		else {
			spin_unlock_irqrestore(&cradle->lock, flags);
		}
	} else {
		pre_set_flag = state;
	}
}

int cradle_get_deskdock(void)
{
	if (!cradle)
		return pre_set_flag;

	return cradle->state;
}

static irqreturn_t pm8xxx_pouch_irq_handler(int irq, void *handle)
{
	struct pm8xxx_cradle *cradle_handle = handle;
	int v;
#if defined(CONFIG_TOUCHSCREEN_ATMEL_S540)
	int status;

	status = !gpio_get_value(cradle->pdata->hallic_pouch_detect_pin);
	printk("pouch irq!!!! %d\n", status);
	v = 1 + 1*status;
	is_smart_cover_closed = status;
#else
	printk("pouch irq!!!!\n");
	v = 1 + 1*(!gpio_get_value(cradle->pdata->hallic_pouch_detect_pin));
#endif
	wake_lock_timeout(&cradle->wake_lock, msecs_to_jiffies(POUCH_DETECT_DELAY*v+5));
	queue_delayed_work(cradle_wq, &cradle_handle->pouch_work, msecs_to_jiffies(POUCH_DETECT_DELAY*v));
	return IRQ_HANDLED;
}

#if defined CONFIG_HALLIC_PEN
static irqreturn_t pm8xxx_pen_irq_handler(int irq, void *handle)
{
	struct pm8xxx_cradle *cradle_handle = handle;
	printk("pen irq!!!!\n");
	wake_lock_timeout(&cradle->wake_lock, msecs_to_jiffies(HALL_DETECT_DELAY+5));
	queue_delayed_work(cradle_wq, &cradle_handle->pen_work, msecs_to_jiffies(HALL_DETECT_DELAY));
	return IRQ_HANDLED;
}
#else

#if !defined (CONFIG_MACH_MSM8974_T1LTE_GLOBAL_COM) && !defined (CONFIG_MACH_MSM8974_T1WIFI_GLOBAL_COM) && !defined (CONFIG_MACH_MSM8974_T1WIFIN_GLOBAL_COM) && !defined(CONFIG_MACH_MSM8974_T1_ATT)
static irqreturn_t pm8xxx_camera_irq_handler(int irq, void *handle)
{
	struct pm8xxx_cradle *cradle_handle = handle;
	printk("camera irq!!!!\n");
	wake_lock_timeout(&cradle->wake_lock, msecs_to_jiffies(HALL_DETECT_DELAY+5));
	queue_delayed_work(cradle_wq, &cradle_handle->camera_work, msecs_to_jiffies(HALL_DETECT_DELAY));
	return IRQ_HANDLED;
}
#endif

#endif

static ssize_t
cradle_pouch_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	int len;
	struct pm8xxx_cradle *cradle = dev_get_drvdata(dev);
    len = snprintf(buf, PAGE_SIZE, "pouch : %d\n", cradle->pouch);

	return len;
}


static ssize_t
cradle_sensing_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	int len;
	struct pm8xxx_cradle *cradle = dev_get_drvdata(dev);
    len = snprintf(buf, PAGE_SIZE, "sensing(cradle state) : %d\n", cradle->state);

	return len;
}
#if defined CONFIG_HALLIC_PEN
static ssize_t
cradle_pen_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	int len;
	struct pm8xxx_cradle *cradle = dev_get_drvdata(dev);
    len = snprintf(buf, PAGE_SIZE, "pen : %d\n", cradle->pen);

	return len;
}
#else

#if !defined (CONFIG_MACH_MSM8974_T1LTE_GLOBAL_COM) && !defined (CONFIG_MACH_MSM8974_T1WIFI_GLOBAL_COM)  && !defined (CONFIG_MACH_MSM8974_T1WIFIN_GLOBAL_COM) && !defined(CONFIG_MACH_MSM8974_T1_ATT)
static ssize_t
cradle_camera_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	int len;
	struct pm8xxx_cradle *cradle = dev_get_drvdata(dev);
    len = snprintf(buf, PAGE_SIZE, "camera : %d\n", cradle->camera);

	return len;
}
#endif

#endif
#if defined CONFIG_HALLIC_PEN
static struct device_attribute cradle_pen_attr   = __ATTR(pen, S_IRUGO | S_IWUSR, cradle_pen_show, NULL);
#else

#if !defined (CONFIG_MACH_MSM8974_T1LTE_GLOBAL_COM) && !defined (CONFIG_MACH_MSM8974_T1WIFI_GLOBAL_COM) && !defined (CONFIG_MACH_MSM8974_T1WIFIN_GLOBAL_COM) && !defined(CONFIG_MACH_MSM8974_T1_ATT)
static struct device_attribute cradle_camera_attr  = __ATTR(camera, S_IRUGO | S_IWUSR, cradle_camera_show, NULL);
#endif

#endif
static struct device_attribute cradle_sensing_attr = __ATTR(sensing, S_IRUGO | S_IWUSR, cradle_sensing_show, NULL);
static struct device_attribute cradle_pouch_attr   = __ATTR(pouch, S_IRUGO | S_IWUSR, cradle_pouch_show, NULL);

static ssize_t cradle_print_name(struct switch_dev *sdev, char *buf)
{
	switch (switch_get_state(sdev)) {
	case 0:
		return sprintf(buf, "UNDOCKED\n");
	case 2:
		return sprintf(buf, "CARKIT\n");
	}
	return -EINVAL;
}

#if defined(CONFIG_S5717)
static void s5717_parse_dt(struct device *dev,
		struct pm8xxx_cradle_platform_data *pdata)
{
	struct device_node *np = dev->of_node;

	if ((pdata->hallic_pouch_detect_pin = of_get_named_gpio_flags(np, "hallic-pouch-irq-gpio", 0, NULL)) > 0)
		pdata->hallic_pouch_irq = gpio_to_irq(pdata->hallic_pouch_detect_pin);

	printk("[Hall IC] hallic_pouch_gpio: %d\n", pdata->hallic_pouch_detect_pin);

	pdata->irq_flags = IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING;
}
#else
static void bu52014hfv_parse_dt(struct device *dev,
		struct pm8xxx_cradle_platform_data *pdata)
{
	struct device_node *np = dev->of_node;

	/* hallic info
		1 : only pouch (KR, ATT ~Rev.A)
		2 : pouch and camera (KR, ATT Rev.B~, SPR, TUMS, DCM Rev.A~)
	*/
	if ((pdata->hallic_pouch_detect_pin = of_get_named_gpio_flags(np, "hallic-pouch-irq-gpio", 0, NULL)) > 0)
		pdata->hallic_pouch_irq = gpio_to_irq(pdata->hallic_pouch_detect_pin);
#if defined CONFIG_HALLIC_PEN
	if ((pdata->hallic_pen_detect_pin = of_get_named_gpio_flags(np, "hallic-pen-irq-gpio", 0, NULL)) > 0)
		pdata->hallic_pen_irq = gpio_to_irq(pdata->hallic_pen_detect_pin);
	printk("[Hall IC] hallic_pouch_gpio: %d, hallic_pen_gpio: %d\n",
		pdata->hallic_pouch_detect_pin, pdata->hallic_pen_detect_pin);
#else

#if !defined (CONFIG_MACH_MSM8974_T1LTE_GLOBAL_COM) && !defined (CONFIG_MACH_MSM8974_T1WIFI_GLOBAL_COM) && !defined (CONFIG_MACH_MSM8974_T1WIFIN_GLOBAL_COM) && !defined(CONFIG_MACH_MSM8974_T1_ATT)
	if ((pdata->hallic_camera_detect_pin = of_get_named_gpio_flags(np, "hallic-camera-irq-gpio", 0, NULL)) > 0)
		pdata->hallic_camera_irq = gpio_to_irq(pdata->hallic_camera_detect_pin);
	printk("[Hall IC] hallic_pouch_gpio: %d, hallic_camera_gpio: %d\n",
		pdata->hallic_pouch_detect_pin, pdata->hallic_camera_detect_pin);
#endif

#endif

	pdata->irq_flags = IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING;
}
#endif

static int __devinit pm8xxx_cradle_probe(struct platform_device *pdev)
{
	int ret;
#if defined CONFIG_HALLIC_PEN
	unsigned int hall_pen_gpio_irq = 0, hall_pouch_gpio_irq = 0;
#else

#if defined (CONFIG_MACH_MSM8974_T1LTE_GLOBAL_COM) || defined (CONFIG_MACH_MSM8974_T1WIFI_GLOBAL_COM) || defined (CONFIG_MACH_MSM8974_T1WIFIN_GLOBAL_COM) || defined(CONFIG_MACH_MSM8974_T1_ATT)
	unsigned int hall_pouch_gpio_irq = 0;
#else
	unsigned int hall_camera_gpio_irq = 0, hall_pouch_gpio_irq = 0;
#endif

#endif

	struct pm8xxx_cradle_platform_data *pdata;

	if (pdev->dev.of_node) {
		pdata = devm_kzalloc(&pdev->dev,
				sizeof(struct pm8xxx_cradle_platform_data),
				GFP_KERNEL);
		if (pdata == NULL) {
			pr_err("%s: no pdata\n", __func__);
			return -ENOMEM;
		}
		pdev->dev.platform_data = pdata;
#if defined(CONFIG_S5717)
		s5717_parse_dt(&pdev->dev, pdata);
#else
		bu52014hfv_parse_dt(&pdev->dev, pdata);
#endif
	} else {
		pdata = pdev->dev.platform_data;
	}
	if (!pdata) {
		pr_err("%s: no pdata\n", __func__);
		return -ENOMEM;
	}

	cradle = kzalloc(sizeof(*cradle), GFP_KERNEL);
	if (!cradle)
		return -ENOMEM;

	cradle->pdata	= pdata;

	cradle->sdev.name = "smartcover";
	cradle->sdev.print_name = cradle_print_name;
	cradle->pouch = 0;
#if defined CONFIG_HALLIC_PEN
	cradle->pen = 0;
#else

#if !defined (CONFIG_MACH_MSM8974_T1LTE_GLOBAL_COM) && !defined (CONFIG_MACH_MSM8974_T1WIFI_GLOBAL_COM) && !defined (CONFIG_MACH_MSM8974_T1WIFIN_GLOBAL_COM) && !defined(CONFIG_MACH_MSM8974_T1_ATT)
	cradle->camera = 0;
#endif

#endif

	spin_lock_init(&cradle->lock);

	ret = switch_dev_register(&cradle->sdev);
	if (ret < 0)
		goto err_switch_dev_register;

	if (pre_set_flag) {
		cradle_set_deskdock(pre_set_flag);
		cradle->state = pre_set_flag;
	}
	wake_lock_init(&cradle->wake_lock, WAKE_LOCK_SUSPEND, "hall_ic_wakeups");

	INIT_DELAYED_WORK(&cradle->pouch_work, pm8xxx_pouch_work_func);
#if defined CONFIG_HALLIC_PEN
	INIT_DELAYED_WORK(&cradle->pen_work, pm8xxx_pen_work_func);
#else

#if !defined (CONFIG_MACH_MSM8974_T1LTE_GLOBAL_COM) && !defined (CONFIG_MACH_MSM8974_T1WIFI_GLOBAL_COM) && !defined (CONFIG_MACH_MSM8974_T1WIFIN_GLOBAL_COM) && !defined(CONFIG_MACH_MSM8974_T1_ATT)
	INIT_DELAYED_WORK(&cradle->camera_work, pm8xxx_camera_work_func);
#endif

#endif

	printk("%s : init cradle\n", __func__);

	/* initialize irq of gpio_hall */
	if (cradle->pdata->hallic_pouch_detect_pin > 0) {
		hall_pouch_gpio_irq = gpio_to_irq(cradle->pdata->hallic_pouch_detect_pin);
		printk("%s : hall_pouch_gpio_irq = [%d]\n", __func__, hall_pouch_gpio_irq);
		if (hall_pouch_gpio_irq < 0) {
			printk("Failed : GPIO TO IRQ \n");
			ret = hall_pouch_gpio_irq;
			goto err_request_irq;
		}

		ret = request_irq(hall_pouch_gpio_irq, pm8xxx_pouch_irq_handler, pdata->irq_flags, HALL_IC_DEV_NAME, cradle);
		if (ret > 0) {
			printk(KERN_ERR "%s: Can't allocate irq %d, ret %d\n", __func__, hall_pouch_gpio_irq, ret);
			goto err_request_irq;
		}

		if (enable_irq_wake(hall_pouch_gpio_irq) == 0)
			printk("%s :enable_irq_wake Enable(1)\n",__func__);
		else
			printk("%s :enable_irq_wake failed(1)\n",__func__);
	}

#if defined CONFIG_HALLIC_PEN
	if (cradle->pdata->hallic_pen_detect_pin > 0) {
		hall_pen_gpio_irq = gpio_to_irq(cradle->pdata->hallic_pen_detect_pin);
		printk("%s : hall_pen_gpio_irq = [%d]\n", __func__, hall_pen_gpio_irq);
		if (hall_pen_gpio_irq < 0) {
			printk("Failed : GPIO TO IRQ \n");
			ret = hall_pen_gpio_irq;
			goto err_request_irq;
		}
		ret = request_irq(hall_pen_gpio_irq, pm8xxx_pen_irq_handler, pdata->irq_flags, HALL_IC_DEV_NAME, cradle);
		if (ret > 0) {
			printk(KERN_ERR "%s: Can't allocate irq %d, ret %d\n", __func__, hall_pen_gpio_irq, ret);
			goto err_request_irq;
		}

		if (enable_irq_wake(hall_pen_gpio_irq) == 0)
			printk("%s :enable_irq_wake Enable(2)\n",__func__);
		else
			printk("%s :enable_irq_wake failed(2)\n",__func__);
	}
#else

#if !defined (CONFIG_MACH_MSM8974_T1LTE_GLOBAL_COM) && !defined (CONFIG_MACH_MSM8974_T1WIFI_GLOBAL_COM) && !defined (CONFIG_MACH_MSM8974_T1WIFIN_GLOBAL_COM) && !defined(CONFIG_MACH_MSM8974_T1_ATT)
	if (cradle->pdata->hallic_camera_detect_pin > 0) {
		hall_camera_gpio_irq = gpio_to_irq(cradle->pdata->hallic_camera_detect_pin);
		printk("%s : hall_camera_gpio_irq = [%d]\n", __func__, hall_camera_gpio_irq);
		if (hall_camera_gpio_irq < 0) {
			printk("Failed : GPIO TO IRQ \n");
			ret = hall_camera_gpio_irq;
			goto err_request_irq;
		}
		ret = request_irq(hall_camera_gpio_irq, pm8xxx_camera_irq_handler, pdata->irq_flags, HALL_IC_DEV_NAME, cradle);
		if (ret > 0) {
			printk(KERN_ERR "%s: Can't allocate irq %d, ret %d\n", __func__, hall_camera_gpio_irq, ret);
			goto err_request_irq;
		}

		if (enable_irq_wake(hall_camera_gpio_irq) == 0)
			printk("%s :enable_irq_wake Enable(2)\n",__func__);
		else
			printk("%s :enable_irq_wake failed(2)\n",__func__);
	}
#endif

#endif
	printk("%s : pdata->irq_flags = [%d]\n", __func__,(int)pdata->irq_flags);

	printk("%s :boot_cradle_det_func START\n",__func__);
	boot_cradle_det_func();

	ret = device_create_file(&pdev->dev, &cradle_sensing_attr);
	if (ret)
		goto err_request_irq;

	if (cradle->pdata->hallic_pouch_detect_pin > 0) {
		ret = device_create_file(&pdev->dev, &cradle_pouch_attr);
		if (ret)
			goto err_request_irq;
	}

#if defined CONFIG_HALLIC_PEN
	if (cradle->pdata->hallic_pen_detect_pin > 0) {
		ret = device_create_file(&pdev->dev, &cradle_pen_attr);
		if (ret)
			goto err_request_irq;
	}
#else

#if !defined (CONFIG_MACH_MSM8974_T1LTE_GLOBAL_COM) && !defined (CONFIG_MACH_MSM8974_T1WIFI_GLOBAL_COM) && !defined (CONFIG_MACH_MSM8974_T1WIFIN_GLOBAL_COM) && !defined(CONFIG_MACH_MSM8974_T1_ATT)
	if (cradle->pdata->hallic_camera_detect_pin > 0) {
		ret = device_create_file(&pdev->dev, &cradle_camera_attr);
		if (ret)
			goto err_request_irq;
	}
#endif

#endif
	platform_set_drvdata(pdev, cradle);
	return 0;

err_request_irq:
	if (hall_pouch_gpio_irq)
		free_irq(hall_pouch_gpio_irq, 0);
#if defined CONFIG_HALLIC_PEN
	if (hall_pen_gpio_irq)
		free_irq(hall_pen_gpio_irq, 0);
#else
	#if !defined (CONFIG_MACH_MSM8974_T1LTE_GLOBAL_COM) && !defined (CONFIG_MACH_MSM8974_T1WIFI_GLOBAL_COM) && !defined (CONFIG_MACH_MSM8974_T1WIFIN_GLOBAL_COM) && !defined(CONFIG_MACH_MSM8974_T1_ATT)
	if (hall_camera_gpio_irq)
		free_irq(hall_camera_gpio_irq, 0);
	#endif
#endif

err_switch_dev_register:
	switch_dev_unregister(&cradle->sdev);
	kfree(cradle);
	return ret;
}

static int __devexit pm8xxx_cradle_remove(struct platform_device *pdev)
{
	struct pm8xxx_cradle *cradle = platform_get_drvdata(pdev);
	cancel_delayed_work_sync(&cradle->pouch_work);
#if defined CONFIG_HALLIC_PEN
	cancel_delayed_work_sync(&cradle->pen_work);
#else
	#if !defined (CONFIG_MACH_MSM8974_T1LTE_GLOBAL_COM) && !defined (CONFIG_MACH_MSM8974_T1WIFI_GLOBAL_COM) && !defined (CONFIG_MACH_MSM8974_T1WIFIN_GLOBAL_COM) && !defined(CONFIG_MACH_MSM8974_T1_ATT)
	cancel_delayed_work_sync(&cradle->camera_work);
	#endif
#endif
	switch_dev_unregister(&cradle->sdev);
	platform_set_drvdata(pdev, NULL);
	kfree(cradle);

	return 0;
}

static int pm8xxx_cradle_suspend(struct device *dev)
{
	return 0;
}

static int pm8xxx_cradle_resume(struct device *dev)
{
	return 0;
}

static const struct dev_pm_ops pm8xxx_cradle_pm_ops = {
	.suspend = pm8xxx_cradle_suspend,
	.resume = pm8xxx_cradle_resume,
};

#ifdef CONFIG_OF
#if defined(CONFIG_S5717)
static struct of_device_id s5717_match_table[] = {
	{ .compatible = "seiko,hall-s5717", },
	{ },
};
#else
static struct of_device_id bu52031nvx_match_table[] = {
	{ .compatible = "rohm,hall-bu52031nvx", },
	{ },
};
#endif
#endif

static struct platform_driver pm8xxx_cradle_driver = {
	.probe		= pm8xxx_cradle_probe,
	.remove		= __devexit_p(pm8xxx_cradle_remove),
	.driver		= {
#if defined (CONFIG_MACH_MSM8974_T1LTE_GLOBAL_COM) || defined (CONFIG_MACH_MSM8974_T1WIFI_GLOBAL_COM) || defined (CONFIG_MACH_MSM8974_T1WIFIN_GLOBAL_COM) || defined(CONFIG_MACH_MSM8974_T1_ATT)
        .name    = "s5717",
#else
        .name    = HALL_IC_DEV_NAME,
#endif
		.owner	= THIS_MODULE,
#ifdef CONFIG_OF
#if defined(CONFIG_S5717)
		.of_match_table = s5717_match_table,
#else
		.of_match_table = bu52031nvx_match_table,
#endif
#endif
#ifdef CONFIG_PM
		.pm	= &pm8xxx_cradle_pm_ops,
#endif
	},
};

static int __init pm8xxx_cradle_init(void)
{
	cradle_wq = create_singlethread_workqueue("cradle_wq");
       printk(KERN_ERR "cradle init \n");
	if (!cradle_wq)
		return -ENOMEM;
	return platform_driver_register(&pm8xxx_cradle_driver);
}
module_init(pm8xxx_cradle_init);

static void __exit pm8xxx_cradle_exit(void)
{
	if (cradle_wq)
		destroy_workqueue(cradle_wq);
	platform_driver_unregister(&pm8xxx_cradle_driver);
}
module_exit(pm8xxx_cradle_exit);

MODULE_ALIAS("platform:" HALL_IC_DEV_NAME);
MODULE_AUTHOR("LG Electronics Inc.");
MODULE_DESCRIPTION("pm8xxx cradle driver");
MODULE_LICENSE("GPL");
