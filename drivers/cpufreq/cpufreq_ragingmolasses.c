/*
 *  drivers/cpufreq/cpufreq_ragingmolasses.c
 *
 *  Copyright (C)  2001 Russell King
 *            (C)  2003 Venkatesh Pallipadi <venkatesh.pallipadi@intel.com>.
 *                      Jun Nakajima <jun.nakajima@intel.com>
 *            (C)  2009 Alexander Clouter <alex@digriz.org.uk>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/cpufreq.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/kernel_stat.h>
#include <linux/kobject.h>
#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/notifier.h>
#include <linux/percpu-defs.h>
#include <linux/slab.h>
#include <linux/sysfs.h>
#include <linux/types.h>


#define DEF_SAMPLING_RATE                       (40000)
#define DEF_FREQUENCY_UP_THRESHOLD		(70)
#define DEF_FREQUENCY_STEP			(20)

#define MAX_FREQUENCY_THRESHOLD 		(3091200)
#define MIN_FREQUENCY_THRESHOLD			(35800)


static DEFINE_PER_CPU(struct rm_cpu_dbs_info_s, rm_cpu_dbs_info);

#ifndef CONFIG_CPU_FREQ_DEFAULT_GOV_RAGINGMOLASSES
static struct cpufreq_governor cpufreq_gov_ragingmolasses;
#endif

static int dbs_cpufreq_notifier(struct notifier_block *nb, unsigned long val,
		void *data)
{
	struct cpufreq_freqs *freq = data;
	struct rm_cpu_dbs_info_s *dbs_info =
					&per_cpu(rm_cpu_dbs_info, freq->cpu);
	struct cpufreq_policy *policy;

	if (!dbs_info->enable)
		return 0;

	policy = dbs_info->cdbs.cur_policy;

	/*
	 * we only care if our internally tracked freq moves outside the 'valid'
	 * ranges of frequency available to us otherwise we do not change it
	*/
	if (dbs_info->requested_freq > policy->max
			|| dbs_info->requested_freq < policy->min)
		dbs_info->requested_freq = freq->new;

	return 0;
}

static struct notifier_block rm_cpufreq_notifier_block = {
	.notifier_call = dbs_cpufreq_notifier,
};

static struct common_dbs_data rm_dbs_cdata;

static ssize_t store_sampling_rate(struct dbs_data *dbs_data, const char *buf,
		size_t count)
{
	struct rm_dbs_tuners *rm_tuners = dbs_data->tuners;
	unsigned int input;
	int ret;
	ret = sscanf(buf, "%u", &input);

	if (ret != 1)
		return -EINVAL;

	rm_tuners->sampling_rate = max(input, dbs_data->min_sampling_rate);
	return count;
}

static ssize_t store_up_threshold(struct dbs_data *dbs_data, const char *buf,
		size_t count)
{
	struct rm_dbs_tuners *rm_tuners = dbs_data->tuners;
	unsigned int input;
	int ret;
	ret = sscanf(buf, "%u", &input);

	if (ret != 1 || input > 100)
		return -EINVAL;

	rm_tuners->up_threshold = input;
	return count;
}

static ssize_t store_ignore_nice_load(struct dbs_data *dbs_data,
		const char *buf, size_t count)
{
	struct rm_dbs_tuners *rm_tuners = dbs_data->tuners;
	unsigned int input, j;
	int ret;

	ret = sscanf(buf, "%u", &input);
	if (ret != 1)
		return -EINVAL;

	if (input > 1)
		input = 1;

	if (input == rm_tuners->ignore_nice_load) /* nothing to do */
		return count;

	rm_tuners->ignore_nice_load = input;

	/* we need to re-evaluate prev_cpu_idle */
	for_each_online_cpu(j) {
		struct rm_cpu_dbs_info_s *dbs_info;
		dbs_info = &per_cpu(rm_cpu_dbs_info, j);
		dbs_info->cdbs.prev_cpu_idle = get_cpu_idle_time(j,
					&dbs_info->cdbs.prev_cpu_wall, 0);
		if (rm_tuners->ignore_nice_load)
			dbs_info->cdbs.prev_cpu_nice =
				kcpustat_cpu(j).cpustat[CPUTIME_NICE];
	}
	return count;
}

static ssize_t store_freq_step(struct dbs_data *dbs_data, const char *buf,
		size_t count)
{
	struct rm_dbs_tuners *rm_tuners = dbs_data->tuners;
	unsigned int input;
	int ret;
	ret = sscanf(buf, "%u", &input);

	if (ret != 1)
		return -EINVAL;

	if (input > 100)
		input = 100;

	/*
	 * no need to test here if freq_step is zero as the user might actually
	 * want this, they would be crazy though :)
	 */
	rm_tuners->freq_step = input;
	return count;
}

show_store_one(rm, sampling_rate);
show_store_one(rm, up_threshold);
show_store_one(rm, ignore_nice_load);
show_store_one(rm, freq_step);
declare_show_sampling_rate_min(rm);

gov_sys_pol_attr_rw(sampling_rate);
gov_sys_pol_attr_rw(up_threshold);
gov_sys_pol_attr_rw(ignore_nice_load);
gov_sys_pol_attr_rw(freq_step);
gov_sys_pol_attr_ro(sampling_rate_min);

static struct attribute *dbs_attributes_gov_sys[] = {
	&sampling_rate_min_gov_sys.attr,
	&sampling_rate_gov_sys.attr,
	&up_threshold_gov_sys.attr,
	&ignore_nice_load_gov_sys.attr,
	&freq_step_gov_sys.attr,
	NULL
};

static struct attribute_group rm_attr_group_gov_sys = {
	.attrs = dbs_attributes_gov_sys,
	.name = "ragingmolasses",
};

static struct attribute *dbs_attributes_gov_pol[] = {
	&sampling_rate_min_gov_pol.attr,
	&sampling_rate_gov_pol.attr,
	&up_threshold_gov_pol.attr,
	&ignore_nice_load_gov_pol.attr,
	&freq_step_gov_pol.attr,
	NULL
};

static struct attribute_group rm_attr_group_gov_pol = {
	.attrs = dbs_attributes_gov_pol,
	.name = "ragingmolasses",
};

static void rm_check_cpu(int cpu, unsigned int load)
{
	struct rm_cpu_dbs_info_s *dbs_info = &per_cpu(rm_cpu_dbs_info, cpu);
	struct cpufreq_policy *policy = dbs_info->cdbs.cur_policy;
	struct dbs_data *dbs_data = policy->governor_data;
	struct rm_dbs_tuners *rm_tuners = dbs_data->tuners;
	unsigned int freq_next;

	
	if (rm_tuners->freq_step == 0)
		return;

	if (load < rm_tuners->up_threshold) {
	/*if load is low then bias to lower speeds*/

		freq_next = (rm_tuners->freq_step * policy->max) / 100;

		dbs_info->requested_freq -= freq_next;
		if (dbs_info->requested_freq < policy->min)
			dbs_info->requested_freq = policy->min;

		if (freq_next < policy->min)
			freq_next = policy->min;
		__cpufreq_driver_target(policy, dbs_info->requested_freq,
				CPUFREQ_RELATION_L);
		return;

	}

	if (load > rm_tuners->up_threshold){
		dbs_info->down_skip = 0;
		/* Calculate the next frequency proportional to load */
		freq_next = load * policy->max / 100;

		if (freq_next > policy->max)
			freq_next = policy->max;

		__cpufreq_driver_target(policy, freq_next,
				CPUFREQ_RELATION_L);
		return;

	} 
}

static void rm_dbs_timer(struct work_struct *work)
{
	struct rm_cpu_dbs_info_s *dbs_info = container_of(work,
			struct rm_cpu_dbs_info_s, cdbs.work.work);
	unsigned int cpu = dbs_info->cdbs.cur_policy->cpu;
	struct rm_cpu_dbs_info_s *core_dbs_info = &per_cpu(rm_cpu_dbs_info,
			cpu);
	struct dbs_data *dbs_data = dbs_info->cdbs.cur_policy->governor_data;
	struct rm_dbs_tuners *rm_tuners = dbs_data->tuners;
	int delay = delay_for_sampling_rate(rm_tuners->sampling_rate);
	bool modify_all = true;

	mutex_lock(&core_dbs_info->cdbs.timer_mutex);
	if (!need_load_eval(&core_dbs_info->cdbs, rm_tuners->sampling_rate))
		modify_all = false;
	else
		dbs_check_cpu(dbs_data, cpu);

	gov_queue_work(dbs_data, dbs_info->cdbs.cur_policy, delay, modify_all);
	mutex_unlock(&core_dbs_info->cdbs.timer_mutex);
}

static int rm_init(struct dbs_data *dbs_data)
{
	struct rm_dbs_tuners *tuners;

	tuners = kzalloc(sizeof(struct rm_dbs_tuners), GFP_KERNEL);
	if (!tuners) {
		pr_err("%s: kzalloc failed\n", __func__);
		return -ENOMEM;
	}

	tuners->up_threshold = DEF_FREQUENCY_UP_THRESHOLD;
	tuners->ignore_nice_load = 0;
	tuners->freq_step = DEF_FREQUENCY_STEP;

	dbs_data->tuners = tuners;
	dbs_data->min_sampling_rate = MIN_SAMPLING_RATE_RATIO *
		jiffies_to_usecs(10);
	mutex_init(&dbs_data->mutex);
	return 0;
}

static void rm_exit(struct dbs_data *dbs_data)
{
	kfree(dbs_data->tuners);
}

define_get_cpu_dbs_routines(rm_cpu_dbs_info);

static struct rm_ops rm_ops = {
	.notifier_block = &rm_cpufreq_notifier_block,
};


static struct common_dbs_data rm_dbs_cdata = {
	.governor = GOV_CONSERVATIVE,
	.attr_group_gov_sys = &rm_attr_group_gov_sys,
	.attr_group_gov_pol = &rm_attr_group_gov_pol,
	.get_cpu_cdbs = get_cpu_cdbs,
	.get_cpu_dbs_info_s = get_cpu_dbs_info_s,
	.gov_dbs_timer = rm_dbs_timer,
	.gov_check_cpu = rm_check_cpu,
	.gov_ops = &rm_ops,
	.init = rm_init,
	.exit = rm_exit,
};

static int rm_cpufreq_governor_dbs(struct cpufreq_policy *policy,
				   unsigned int event)
{
	return cpufreq_governor_dbs(policy, &rm_dbs_cdata, event);
}

#ifndef CONFIG_CPU_FREQ_DEFAULT_GOV_RAGINGMOLASSES
static
#endif

struct cpufreq_governor cpufreq_gov_ragingmolasses = {
	.name			= "ragingmolasses",
	.governor		= rm_cpufreq_governor_dbs,
	.max_transition_latency	= TRANSITION_LATENCY_LIMIT,
	.owner			= THIS_MODULE,
};

static int __init cpufreq_gov_dbs_init(void)
{
	return cpufreq_register_governor(&cpufreq_gov_ragingmolasses);
}

static void __exit cpufreq_gov_dbs_exit(void)
{
	cpufreq_unregister_governor(&cpufreq_gov_ragingmolasses);
}


MODULE_AUTHOR("Alexander Clouter <alex@digriz.org.uk>");
MODULE_DESCRIPTION("'cpufreq_ragingmolasses' - A dynamic cpufreq governor for "
		"Low Latency Frequency Transition capable processors "
		"optimised for use in a battery environment");
MODULE_LICENSE("GPL");

#ifdef CONFIG_CPU_FREQ_DEFAULT_GOV_RAGINGMOLASSES
fs_initcall(cpufreq_gov_dbs_init);
#else
module_init(cpufreq_gov_dbs_init);
#endif
module_exit(cpufreq_gov_dbs_exit);
