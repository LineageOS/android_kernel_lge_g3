/*
 * arch/arm/mach-msm/fast_hotplug.c
 *
 * Copyright (C) 2014 Basile Maret
 * 
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * Author: Basile Maret (pec0ra)
 *
 */
#include <linux/cpu.h>
#include <linux/input.h>
#include <linux/slab.h>
#include <linux/workqueue.h>
#include <linux/sched.h>
#include <linux/cpufreq.h>
#include <linux/module.h>
#include <linux/powersuspend.h>

#define FAST_HOTPLUG_ENABLED	0

// #define DEBUG_ENABLED		1
#define HOTPLUG_INFO_TAG	"[HOTPLUG] : "

#ifndef CONFIG_NR_CPUS
#define CONFIG_NR_CPUS		4
#endif

#define REFRESH_RATE		100  /* ms */

#define BOOST_DURATION		1000 /* ms */

#define THRESHOLD_TO_BOOST		3000


#define SCREEN_OFF_SINGLECORE		1


#define PLUG_IN_CORE_1_THRESHOLD	2500
#define PLUG_IN_CORE_2_THRESHOLD	4000
#define PLUG_IN_CORE_3_THRESHOLD	4500

#define PLUG_IN_CORE_1_DELAY		1
#define PLUG_IN_CORE_2_DELAY		3
#define PLUG_IN_CORE_3_DELAY		3

#define PLUG_OUT_CORE_1_THRESHOLD	2500
#define PLUG_OUT_CORE_2_THRESHOLD	3000
#define PLUG_OUT_CORE_3_THRESHOLD	3500

#define PLUG_OUT_CORE_1_DELAY		3
#define PLUG_OUT_CORE_2_DELAY		2
#define PLUG_OUT_CORE_3_DELAY		1

static int fast_hotplug_enabled = FAST_HOTPLUG_ENABLED;
static int enable_fast_hotplug(const char *val, const struct kernel_param *kp);

static struct kernel_param_ops params_ops_enable = {
       .set = enable_fast_hotplug,
       .get = param_get_uint,
};
module_param_cb(fast_hotplug_enabled, &params_ops_enable, &fast_hotplug_enabled, 0644);

static DEFINE_MUTEX(mutex);

static unsigned int refresh_rate = REFRESH_RATE;
module_param(refresh_rate, uint, 0644);

static struct workqueue_struct *hotplug_wq;
static struct delayed_work hotplug_work;


static unsigned long boost_duration = BOOST_DURATION;
module_param(boost_duration, ulong, 0644);

static struct timer_list unboost_timer;
static int is_boosted = false;

static unsigned long idle_threshold = PLUG_IN_CORE_1_THRESHOLD;
module_param(idle_threshold, ulong, 0644);
static unsigned long threshold_to_boost = THRESHOLD_TO_BOOST;
module_param(threshold_to_boost, ulong, 0644);

static int screen_off_singlecore = SCREEN_OFF_SINGLECORE;
module_param(screen_off_singlecore, int, 0644);

static int is_sleeping = false;

static int singlecore = false;
static int max_cpu_on = CONFIG_NR_CPUS;
module_param(max_cpu_on, int, 0644);
static int min_cpu_on = 1;
module_param(min_cpu_on, int, 0644);

unsigned long plug_in_core_1_threshold = PLUG_IN_CORE_1_THRESHOLD;
unsigned long plug_in_core_2_threshold = PLUG_IN_CORE_2_THRESHOLD;
unsigned long plug_in_core_3_threshold = PLUG_IN_CORE_3_THRESHOLD;
module_param(plug_in_core_1_threshold, ulong, 0644);
module_param(plug_in_core_2_threshold, ulong, 0644);
module_param(plug_in_core_3_threshold, ulong, 0644);

unsigned int plug_in_core_1_delay = PLUG_IN_CORE_1_DELAY;
unsigned int plug_in_core_2_delay = PLUG_IN_CORE_2_DELAY;
unsigned int plug_in_core_3_delay = PLUG_IN_CORE_3_DELAY;
module_param(plug_in_core_1_delay, uint, 0644);
module_param(plug_in_core_2_delay, uint, 0644);
module_param(plug_in_core_3_delay, uint, 0644);

unsigned long plug_out_core_1_threshold = PLUG_OUT_CORE_1_THRESHOLD;
unsigned long plug_out_core_2_threshold = PLUG_OUT_CORE_2_THRESHOLD;
unsigned long plug_out_core_3_threshold = PLUG_OUT_CORE_3_THRESHOLD;
module_param(plug_out_core_1_threshold, ulong, 0644);
module_param(plug_out_core_2_threshold, ulong, 0644);
module_param(plug_out_core_3_threshold, ulong, 0644);

unsigned int plug_out_core_1_delay = PLUG_OUT_CORE_1_DELAY;
unsigned int plug_out_core_2_delay = PLUG_OUT_CORE_2_DELAY;
unsigned int plug_out_core_3_delay = PLUG_OUT_CORE_3_DELAY;
module_param(plug_out_core_1_delay, uint, 0644);
module_param(plug_out_core_2_delay, uint, 0644);
module_param(plug_out_core_3_delay, uint, 0644);

unsigned long zero = 0;
unsigned long max = ~0;
unsigned int izero = 0;

static unsigned long *plug_in_threshold[] = {
	&zero,
	&plug_in_core_1_threshold,
#if CONFIG_NR_CPUS > 2
	&plug_in_core_2_threshold,
	&plug_in_core_3_threshold,
#endif
	&max
};

static unsigned int delay_in;
static unsigned int *plug_in_delay[] = {
	&izero,
	&plug_in_core_1_delay,
#if CONFIG_NR_CPUS > 2
	&plug_in_core_2_delay,
	&plug_in_core_3_delay,
#endif
	&izero
};

static unsigned long *plug_out_threshold[] = {
	&zero,
	&plug_out_core_1_threshold,
#if CONFIG_NR_CPUS > 2
	&plug_out_core_2_threshold,
	&plug_out_core_3_threshold,
#endif
	&zero
};

static unsigned int delay_out;
static unsigned int *plug_out_delay[] = {
	&izero,
	&plug_out_core_1_delay,
#if CONFIG_NR_CPUS > 2
	&plug_out_core_2_delay,
	&plug_out_core_3_delay,
#endif
	&izero
};

extern unsigned long avg_nr_running(void);
extern unsigned long avg_cpu_nr_running(unsigned int cpu);


static struct power_suspend hotplug_power_suspend_handler;
static void plug_in(int online_cpu_count);

static int get_slowest_cpu(void){
	int cpu, slowest_cpu = 1;
	unsigned long load, min_load = ~0;
	for_each_online_cpu(cpu){
		if(cpu == 0)
			continue;
		load = avg_cpu_nr_running((unsigned int)cpu);
		if(load < min_load){
			min_load = load;
			slowest_cpu = cpu;
		}
	}
	return slowest_cpu;
}

static void plug_in(int online_cpu_count){
	int cpu;

	if(online_cpu_count >= max_cpu_on)
		return;

	mutex_lock(&mutex);
	singlecore = false;
	for_each_possible_cpu(cpu){
		if(cpu != 0){
			if(!cpu_online(cpu)){
				cpu_up(cpu);
				break;
			}
		}
	}
	mutex_unlock(&mutex);
#ifdef DEBUG_ENABLED
	pr_info(HOTPLUG_INFO_TAG"Plugged in a core !");
#endif
}
static void plug_out(int online_cpu_count){

	if(online_cpu_count <= min_cpu_on)
		return;

	mutex_lock(&mutex);
	cpu_down(get_slowest_cpu());
	if(online_cpu_count <= 2){
		singlecore = true;
#ifdef DEBUG_ENABLED
		pr_info(HOTPLUG_INFO_TAG"Now running in single core");
#endif
	}
	mutex_unlock(&mutex);
#ifdef DEBUG_ENABLED
	pr_info(HOTPLUG_INFO_TAG"Plugged out a core !");
#endif
}

/*
 * Main function of the hotplug
 */
static void hotplug(struct work_struct *work){
	int online_cpu_count;
	unsigned long load = avg_nr_running();


	// We minimize the work when calculations are not
	// needed to spare resources when cpu is almost idle
	if(load < idle_threshold && singlecore)
		goto delay_work;

	online_cpu_count = num_online_cpus();
#ifdef DEBUG_ENABLED
	pr_info(HOTPLUG_INFO_TAG"The load is %lu, we have %d cpu online and the in threshold is : %lu. The delay is %d, out : %lu, %d and singlecore : %d", load, online_cpu_count, *plug_in_threshold[online_cpu_count], *plug_in_delay[online_cpu_count], *plug_out_threshold[online_cpu_count - 1], *plug_out_delay[online_cpu_count - 1], singlecore);
#endif
	
	// If boosted, we don't hesitate to plug in cores if there is some load
	if(is_boosted && load > threshold_to_boost && singlecore){
		plug_in(online_cpu_count);

	// If we have enough load, we decrement the delay until we can plug in
	} else if(load > *(plug_in_threshold[online_cpu_count])){
		delay_out = *(plug_out_delay[online_cpu_count]);
		if(delay_in > 0)
			delay_in--;
		else
			plug_in(online_cpu_count);
	} else if(!is_boosted && load < *(plug_out_threshold[online_cpu_count - 1])){
		delay_in = *(plug_in_delay[online_cpu_count -1]);
		if(delay_out > 0)
			delay_out--;
		else
			plug_out(online_cpu_count);

	// Nothing special happens, we just reset the delays
	} else {
		delay_out = *(plug_out_delay[online_cpu_count]);
		delay_in = *(plug_in_delay[online_cpu_count]);
	}

delay_work:
	// If we don't lock and check for sleep here, we might queue a
	// new work just after the power suspend function flushed the work queue
	mutex_lock(&mutex);
	if(is_sleeping){
		mutex_unlock(&mutex);
		return;
	}
	queue_delayed_work_on(0, hotplug_wq, &hotplug_work, msecs_to_jiffies(refresh_rate));
	mutex_unlock(&mutex);
}


/*
 * Boost / Unboost cpu when the screen is touched
 */
static void unboost_cpu(unsigned long data){
	is_boosted = false;
	
	// This is a good, quiet occasion to change the idle threshold
	// in case the plug in frequency has been changed
	idle_threshold = *(plug_in_threshold[1]);
#ifdef DEBUG_ENABLED
	pr_info(HOTPLUG_INFO_TAG"Cpu unboosted !\n");
#endif
}

static void hotplug_input_event(struct input_handle *handle,
		unsigned int type, unsigned int code, int value)
{
	if(is_boosted){
		mod_timer(&unboost_timer, jiffies + msecs_to_jiffies(boost_duration));
		return;
	}
	is_boosted = true;
#ifdef DEBUG_ENABLED
	pr_info(HOTPLUG_INFO_TAG"Cpu boosted !\n");
#endif
	mod_timer(&unboost_timer, jiffies + msecs_to_jiffies(boost_duration));

}

/* 
 * This part of the code is about managing touchscreen event
 */
static int hotplug_input_connect(struct input_handler *handler,
		struct input_dev *dev, const struct input_device_id *id)
{
	struct input_handle *handle;
	int error;

	handle = kzalloc(sizeof(struct input_handle), GFP_KERNEL);
	if (!handle)
		return -ENOMEM;

	handle->dev = dev;
	handle->handler = handler;
	handle->name = "hotplug";

	error = input_register_handle(handle);
	if (error)
		goto err2;

	error = input_open_device(handle);
	if (error)
		goto err1;
	return 0;
err1:
	input_unregister_handle(handle);
err2:
	kfree(handle);
	return error;
}

static void hotplug_input_disconnect(struct input_handle *handle)
{
	input_close_device(handle);
	input_unregister_handle(handle);
	kfree(handle);
}

static const struct input_device_id hotplug_ids[] = {
	{
		.flags = INPUT_DEVICE_ID_MATCH_EVBIT |
			 INPUT_DEVICE_ID_MATCH_ABSBIT,
		.evbit = { BIT_MASK(EV_ABS) },
		.absbit = { [BIT_WORD(ABS_MT_POSITION_X)] =
			    BIT_MASK(ABS_MT_POSITION_X) |
			    BIT_MASK(ABS_MT_POSITION_Y) },
	}, /* multi-touch touchscreen */
	{
		.flags = INPUT_DEVICE_ID_MATCH_KEYBIT |
			 INPUT_DEVICE_ID_MATCH_ABSBIT,
		.keybit = { [BIT_WORD(BTN_TOUCH)] = BIT_MASK(BTN_TOUCH) },
		.absbit = { [BIT_WORD(ABS_X)] =
			    BIT_MASK(ABS_X) | BIT_MASK(ABS_Y) },
	}, /* touchpad */
	{ },
};

static struct input_handler hotplug_input_handler = {
	.event          = hotplug_input_event,
	.connect        = hotplug_input_connect,
	.disconnect     = hotplug_input_disconnect,
	.name           = "hotplug_handler",
	.id_table       = hotplug_ids,
};

static int enable_fast_hotplug(const char *val, const struct kernel_param *kp){
	int cpu;
	int ret = param_set_bool(val, kp);
	int rc;
	
	if(!fast_hotplug_enabled){
		pr_info(HOTPLUG_INFO_TAG"Fast hotplug disabled\n");
		mutex_lock(&mutex);
		flush_workqueue(hotplug_wq);
		
		// stop the boosting as well
		input_unregister_handler(&hotplug_input_handler);
		del_timer_sync(&unboost_timer);

		for_each_possible_cpu(cpu){
			if(cpu == 0)
				continue;
			cpu_up(cpu);
		}
		is_sleeping = true;
		mutex_unlock(&mutex);
	} else {
		pr_info(HOTPLUG_INFO_TAG"Fast hotplug enabled\n");
		mutex_lock(&mutex);
		is_sleeping = false;
		is_boosted = true;

		flush_workqueue(hotplug_wq);
		
		rc = input_register_handler(&hotplug_input_handler);
		init_timer(&unboost_timer);

		mod_timer(&unboost_timer, jiffies + msecs_to_jiffies(boost_duration));
		queue_delayed_work_on(0, hotplug_wq, &hotplug_work, msecs_to_jiffies(refresh_rate));
		mutex_unlock(&mutex);
	}

	return ret;
}

/*
 * Suspend / Resume
 */


static void hotplug_power_suspend(struct power_suspend *h) {
	int cpu;
	if(fast_hotplug_enabled && screen_off_singlecore){
		mutex_lock(&mutex);
		flush_workqueue(hotplug_wq);
#ifdef DEBUG_ENABLED
		pr_info(HOTPLUG_INFO_TAG"Screen off\n");
#endif
		for_each_online_cpu(cpu){
			if(cpu == 0)
				continue;
#ifdef DEBUG_ENABLED
			pr_info(HOTPLUG_INFO_TAG"Bringing cpu %d down\n", cpu);
#endif
			cpu_down(cpu);
		}
		singlecore = true;
		is_sleeping = true;
		mutex_unlock(&mutex);
	}
}

static void hotplug_late_resume(struct power_suspend *h) {
	if(fast_hotplug_enabled){
#ifdef DEBUG_ENABLED
		pr_info(HOTPLUG_INFO_TAG"Screen on, let's boost the cpu !");
#endif
		is_sleeping = false;
		is_boosted = true;
		plug_in(num_online_cpus());
		mutex_lock(&mutex);
		mod_timer(&unboost_timer, jiffies + msecs_to_jiffies(boost_duration));
		queue_delayed_work_on(0, hotplug_wq, &hotplug_work, msecs_to_jiffies(1));
		mutex_unlock(&mutex);
	}
}

static struct power_suspend hotplug_power_suspend_handler = {
	.suspend = hotplug_power_suspend,
	.resume = hotplug_late_resume,
};
/*
 * Initialization of the module
 */
static int __init hotplug_init(void)
{

	unboost_timer.function = unboost_cpu;
	unboost_timer.expires = jiffies;

	hotplug_wq = alloc_workqueue("hotplug", WQ_HIGHPRI | WQ_UNBOUND, 1);

	INIT_DELAYED_WORK(&hotplug_work, hotplug);

	register_power_suspend(&hotplug_power_suspend_handler);

	//queue_delayed_work_on(0, hotplug_wq, &hotplug_work, msecs_to_jiffies(refresh_rate));

	pr_info(HOTPLUG_INFO_TAG"Fast hotplug succesfully initialized !");
	return 0;
}


late_initcall(hotplug_init);
