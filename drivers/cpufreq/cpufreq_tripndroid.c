/*
 * drivers/cpufreq/cpufreq_tripndroid.c
 *
 * Copyright (c) 2013, TripNDroid Mobile Engineering
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include <linux/cpu.h>
#include <linux/cpumask.h>
#include <linux/cpufreq.h>
#include <linux/mutex.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/tick.h>
#include <linux/time.h>
#include <linux/timer.h>
#include <linux/workqueue.h>
#include <linux/kthread.h>
#include <linux/mutex.h>
#include <linux/earlysuspend.h>

#ifdef CONFIG_TRIPNDROID_FRAMEWORK
#include <linux/td_framework.h>
#endif

#include <asm/cputime.h>

#ifdef CONFIG_TRIPNDROID_FRAMEWORK
extern unsigned int tdf_suspend_state;
extern unsigned int tdf_cpu_load;
extern unsigned int powersaving_active;
#endif

static atomic_t active_count = ATOMIC_INIT(0);

struct cpufreq_tripndroid_cpuinfo {
	struct timer_list cpu_timer;
	int timer_idlecancel;
	u64 time_in_idle;
	u64 idle_exit_time;
	u64 timer_run_time;
	int idling;
	u64 freq_change_time;
	u64 freq_change_time_in_idle;
	struct cpufreq_policy *policy;
	struct cpufreq_frequency_table *freq_table;
	unsigned int target_freq;
	unsigned int freq_table_size;
	int governor_enabled;
}; 

static DEFINE_PER_CPU(struct cpufreq_tripndroid_cpuinfo, cpuinfo);

/* workqueues handle */
static struct task_struct *up_task;
static struct workqueue_struct *down_wq;
static struct work_struct freq_scale_down_work;

static cpumask_t up_cpumask;
static spinlock_t up_cpumask_lock;
static cpumask_t down_cpumask;
static spinlock_t down_cpumask_lock;

static struct mutex set_speed_lock;

/* hispeed to jump to from lowspeed at load burst */
static u64 hispeed_freq; 

/* governor settings */
#define DEFAULT_GO_HISPEED_LOAD 80
static unsigned long go_hispeed_load;

#define DEFAULT_DOWN_SAMPLE_TIME 20 * USEC_PER_MSEC
static unsigned long down_sample_time;

#define DEFAULT_UP_SAMPLE_TIME 10 * USEC_PER_MSEC
static unsigned long up_sample_time;

#define DEFAULT_TIMER_RATE 20 * USEC_PER_MSEC
static unsigned long timer_rate;

static int cpufreq_governor_tripndroid(struct cpufreq_policy *policy, unsigned int event);

#ifndef CONFIG_CPU_FREQ_DEFAULT_GOV_TRIPNDROID
static
#endif
struct cpufreq_governor cpufreq_gov_tripndroid = {
	.name = "tripndroid",
	.governor = cpufreq_governor_tripndroid,
	.max_transition_latency = 10000000,
	.owner = THIS_MODULE,
};

static unsigned int get_freq_table_size(struct cpufreq_frequency_table *freq_table) {

	unsigned int size = 0;

	while (freq_table[++size].frequency != CPUFREQ_TABLE_END);
		return size;
}

static void cpufreq_tripndroid_timer(unsigned long data)
{
	unsigned int delta_idle;
	unsigned int delta_time;
	int cpu_load;
	int load_since_change;
	u64 time_in_idle;
	u64 idle_exit_time;
	struct cpufreq_tripndroid_cpuinfo *pcpu = &per_cpu(cpuinfo, data);
	u64 now_idle;
	unsigned long flags;
	unsigned int new_freq;
	int index;
	int ret;

	smp_rmb();

	if (!pcpu->governor_enabled)
		goto exit;

	time_in_idle = pcpu->time_in_idle;
	idle_exit_time = pcpu->idle_exit_time;
	now_idle = get_cpu_idle_time_us(data, &pcpu->timer_run_time);
	smp_wmb();

	/* If we raced with cancelling a timer, skip. */
	if (!idle_exit_time) {
		goto exit;
	}

	delta_idle = (unsigned int) (now_idle - time_in_idle);
	delta_time = (unsigned int) (pcpu->timer_run_time - idle_exit_time);

	/*
	 * when timer running less than 1ms after short-term sample started, retry it
	 */
	if (delta_time < 1000)
		goto rearm;

	if (delta_idle > delta_time)
		cpu_load = 0;
	else
		cpu_load = 100 * (delta_time - delta_idle) / delta_time;

#ifdef CONFIG_TRIPNDROID_FRAMEWORK
	tdf_cpu_load = cpu_load;
#endif

	delta_idle = (unsigned int) (now_idle - pcpu->freq_change_time_in_idle);
	delta_time = (unsigned int) (pcpu->timer_run_time - pcpu->freq_change_time);

	if ((delta_time == 0) || (delta_idle > delta_time))
		load_since_change = 0;
	else
		load_since_change = 100 * (delta_time - delta_idle) / delta_time;

	/*
	 * Choose greater of short-term load (since last idle timer
	 * started or timer function re-armed itself) or long-term load
	 * (since last frequency change).
	 */
	if (load_since_change > cpu_load)
		cpu_load = load_since_change;

#ifdef CONFIG_TRIPNDROID_FRAMEWORK
	if (tdf_suspend_state == 1 && pcpu->policy->max != TDF_FREQ_SLEEP_MAX) {
	pcpu->policy->max = TDF_FREQ_SLEEP_MAX;
	new_freq = pcpu->policy->min;
	pcpu->target_freq = pcpu->policy->cur;
	}
	else {
	pcpu->policy->max = hispeed_freq;
	}
#endif

	if (cpu_load >= go_hispeed_load) {

		ret = cpufreq_frequency_table_target(
				pcpu->policy, pcpu->freq_table,
				pcpu->policy->cur, CPUFREQ_RELATION_H,
				&index);

			if (pcpu->policy->cur < pcpu->policy->max)

			if (ret < 0)
				goto rearm;

			index += 1;

			if (index >= pcpu->freq_table_size)
				index = pcpu->freq_table_size - 1;

			new_freq = pcpu->freq_table[index].frequency;
	}
	else {

			new_freq = pcpu->policy->max * cpu_load / 100;
			ret = cpufreq_frequency_table_target(pcpu->policy, pcpu->freq_table,
                                                 new_freq, CPUFREQ_RELATION_H,
                                                 &index);
			if (ret < 0)
				goto rearm;

			new_freq = pcpu->freq_table[index].frequency;
	}

	if (cpufreq_frequency_table_target(pcpu->policy, pcpu->freq_table, new_freq, CPUFREQ_RELATION_H, &index)) {
		pr_warn_once("timer %d: cpufreq_frequency_table_target error\n", (int) data);
			goto rearm;
	}

	if (pcpu->target_freq == new_freq) {
		goto rearm_if_notmax;
	}

	/* scale only down if we have been at this frequency for the minimum sample time */
	if (new_freq < pcpu->target_freq) {
		if ((pcpu->timer_run_time - pcpu->freq_change_time) < down_sample_time) {
			goto rearm;
		}
	}
	else {
		if ((pcpu->timer_run_time - pcpu->freq_change_time) < up_sample_time) {
			goto rearm;
		}
	}

	if (new_freq < pcpu->target_freq) {
		pcpu->target_freq = new_freq;
		spin_lock_irqsave(&down_cpumask_lock, flags);
		cpumask_set_cpu(data, &down_cpumask);
		spin_unlock_irqrestore(&down_cpumask_lock, flags);
		queue_work(down_wq, &freq_scale_down_work);
	}
	else {
		pcpu->target_freq = new_freq;
		spin_lock_irqsave(&up_cpumask_lock, flags);
		cpumask_set_cpu(data, &up_cpumask);
		spin_unlock_irqrestore(&up_cpumask_lock, flags);
		wake_up_process(up_task);
	}

rearm_if_notmax:
	/* dont set policy max when running policy max */
	if (pcpu->target_freq == pcpu->policy->max)
		goto exit;

rearm:
	if (!timer_pending(&pcpu->cpu_timer)) {

		if (pcpu->target_freq == pcpu->policy->min) {
			smp_rmb();

			if (pcpu->idling) {
				goto exit;
			}
			pcpu->timer_idlecancel = 1;
		}

		pcpu->time_in_idle = get_cpu_idle_time_us(data, &pcpu->idle_exit_time);
		mod_timer(&pcpu->cpu_timer, jiffies + 2);
	}

exit:
	return;
}

static void cpufreq_tripndroid_idle_start(void)
{
	struct cpufreq_tripndroid_cpuinfo *pcpu =
		&per_cpu(cpuinfo, smp_processor_id());
	int pending;

	if (!pcpu->governor_enabled)
		return;

	pcpu->idling = 1;
	smp_wmb();
	pending = timer_pending(&pcpu->cpu_timer);

	if (pcpu->target_freq != pcpu->policy->min) {

		if (!pending) {
			pcpu->time_in_idle = get_cpu_idle_time_us(
				smp_processor_id(), &pcpu->idle_exit_time);
			pcpu->timer_idlecancel = 0;
			mod_timer(&pcpu->cpu_timer,
				  jiffies + usecs_to_jiffies(timer_rate));
		}
	}
	else {

		if (pending && pcpu->timer_idlecancel) {
			del_timer(&pcpu->cpu_timer);
			/* ensure last timer run time is after current idle sample start time */
			pcpu->idle_exit_time = 0;
			pcpu->timer_idlecancel = 0;
		}
	}
}

static void cpufreq_tripndroid_idle_end(void)
{
	struct cpufreq_tripndroid_cpuinfo *pcpu = &per_cpu(cpuinfo, smp_processor_id());

	pcpu->idling = 0;
	smp_wmb();

	if (timer_pending(&pcpu->cpu_timer) == 0 &&
	    pcpu->timer_run_time >= pcpu->idle_exit_time &&
	    pcpu->governor_enabled) {
		pcpu->time_in_idle = get_cpu_idle_time_us(smp_processor_id(), &pcpu->idle_exit_time);
		pcpu->timer_idlecancel = 0;

		mod_timer(&pcpu->cpu_timer, jiffies + usecs_to_jiffies(timer_rate));
	}

}

static int cpufreq_tripndroid_up_task(void *data)
{
	unsigned int cpu;
	cpumask_t tmp_mask;
	unsigned long flags;
	struct cpufreq_tripndroid_cpuinfo *pcpu;

	while (1) {
		set_current_state(TASK_INTERRUPTIBLE);
		spin_lock_irqsave(&up_cpumask_lock, flags);

		if (cpumask_empty(&up_cpumask)) {
			spin_unlock_irqrestore(&up_cpumask_lock, flags);
			schedule();

			if (kthread_should_stop())
				break;

			spin_lock_irqsave(&up_cpumask_lock, flags);
		}

		set_current_state(TASK_RUNNING);
		tmp_mask = up_cpumask;
		cpumask_clear(&up_cpumask);
		spin_unlock_irqrestore(&up_cpumask_lock, flags);

		for_each_cpu(cpu, &tmp_mask) {
			unsigned int j;
			unsigned int max_freq = 0;

			pcpu = &per_cpu(cpuinfo, cpu);
			smp_rmb();

			if (!pcpu->governor_enabled)
				continue;

			mutex_lock(&set_speed_lock);

			for_each_cpu(j, pcpu->policy->cpus) {
				struct cpufreq_tripndroid_cpuinfo *pjcpu =
					&per_cpu(cpuinfo, j);

#ifdef CONFIG_TRIPNDROID_FRAMEWORK
				if (tdf_suspend_state == 1) {
					if (pjcpu->target_freq > TDF_FREQ_SLEEP_MAX)
						pjcpu->target_freq = TDF_FREQ_SLEEP_MAX;
				}

				if ((powersaving_active == 1) &&
						(tdf_suspend_state == 0)) {
					if (pjcpu->target_freq > TDF_FREQ_PWRSAVE_MAX)
						pjcpu->target_freq = TDF_FREQ_PWRSAVE_MAX;
				}
#endif
				if (pjcpu->target_freq > max_freq)
					max_freq = pjcpu->target_freq;

			}

			if (max_freq != pcpu->policy->cur)
				__cpufreq_driver_target(pcpu->policy,
							max_freq,
							CPUFREQ_RELATION_H);
			mutex_unlock(&set_speed_lock);

			pcpu->freq_change_time_in_idle =
				get_cpu_idle_time_us(cpu,
						     &pcpu->freq_change_time);
		}
	}

	return 0;
}

static void cpufreq_tripndroid_freq_down(struct work_struct *work)
{
	unsigned int cpu;
	cpumask_t tmp_mask;
	unsigned long flags;
	struct cpufreq_tripndroid_cpuinfo *pcpu;

	spin_lock_irqsave(&down_cpumask_lock, flags);
	tmp_mask = down_cpumask;
	cpumask_clear(&down_cpumask);
	spin_unlock_irqrestore(&down_cpumask_lock, flags);

	for_each_cpu(cpu, &tmp_mask) {
		unsigned int j;
		unsigned int max_freq = 0;

		pcpu = &per_cpu(cpuinfo, cpu);
		smp_rmb();

		if (!pcpu->governor_enabled)
			continue;

		mutex_lock(&set_speed_lock);

		for_each_cpu(j, pcpu->policy->cpus) {
			struct cpufreq_tripndroid_cpuinfo *pjcpu =
				&per_cpu(cpuinfo, j);

#ifdef CONFIG_TRIPNDROID_FRAMEWORK
			if (tdf_suspend_state == 1) {
				if (pjcpu->target_freq > TDF_FREQ_SLEEP_MAX)
					pjcpu->target_freq = TDF_FREQ_SLEEP_MAX;
			}

			if ((powersaving_active == 1) &&
					(tdf_suspend_state == 0)) {
				if (pjcpu->target_freq > TDF_FREQ_PWRSAVE_MAX)
					pjcpu->target_freq = TDF_FREQ_PWRSAVE_MAX;
			}
#endif
			if (pjcpu->target_freq > max_freq)
				max_freq = pjcpu->target_freq;
		}

		if (max_freq != pcpu->policy->cur)
			__cpufreq_driver_target(pcpu->policy, max_freq,
						CPUFREQ_RELATION_H);

		mutex_unlock(&set_speed_lock);
		pcpu->freq_change_time_in_idle =
			get_cpu_idle_time_us(cpu,
					     &pcpu->freq_change_time);
	}
}

static ssize_t show_hispeed_freq(struct kobject *kobj,
				 struct attribute *attr, char *buf)
{
	return sprintf(buf, "%llu\n", hispeed_freq);
}

static ssize_t store_hispeed_freq(struct kobject *kobj,
				  struct attribute *attr, const char *buf,
				  size_t count)
{
	int ret;
	u64 val;

	ret = strict_strtoull(buf, 0, &val);
	if (ret < 0)
		return ret;
	hispeed_freq = val;
	return count;
}

static struct global_attr hispeed_freq_attr = __ATTR(hispeed_freq, 0644,
		show_hispeed_freq, store_hispeed_freq);


static ssize_t show_go_hispeed_load(struct kobject *kobj,
				     struct attribute *attr, char *buf)
{
	return sprintf(buf, "%lu\n", go_hispeed_load);
}

static ssize_t store_go_hispeed_load(struct kobject *kobj,
			struct attribute *attr, const char *buf, size_t count)
{
	int ret;
	unsigned long val;

	ret = strict_strtoul(buf, 0, &val);
	if (ret < 0)
		return ret;
	go_hispeed_load = val;
	return count;
}

static struct global_attr go_hispeed_load_attr = __ATTR(go_hispeed_load, 0644,
		show_go_hispeed_load, store_go_hispeed_load);

static ssize_t show_down_sample_time(struct kobject *kobj,
				struct attribute *attr, char *buf)
{
	return sprintf(buf, "%lu\n", down_sample_time);
}

static ssize_t store_down_sample_time(struct kobject *kobj,
			struct attribute *attr, const char *buf, size_t count)
{
	int ret;
	unsigned long val;

	ret = strict_strtoul(buf, 0, &val);
	if (ret < 0)
		return ret;
	down_sample_time = val;
	return count;
}

static struct global_attr down_sample_time_attr = __ATTR(down_sample_time, 0644,
		show_down_sample_time, store_down_sample_time);

static ssize_t show_up_sample_time(struct kobject *kobj,
				struct attribute *attr, char *buf)
{
	return sprintf(buf, "%lu\n", up_sample_time);
}

static ssize_t store_up_sample_time(struct kobject *kobj,
			struct attribute *attr, const char *buf, size_t count)
{
	int ret;
	unsigned long val;

	ret = strict_strtoul(buf, 0, &val);
	if (ret < 0)
		return ret;
	up_sample_time = val;
	return count;
}

static struct global_attr up_sample_time_attr = __ATTR(up_sample_time, 0644,
		show_up_sample_time, store_up_sample_time);

static ssize_t show_timer_rate(struct kobject *kobj,
			struct attribute *attr, char *buf)
{
	return sprintf(buf, "%lu\n", timer_rate);
}

static ssize_t store_timer_rate(struct kobject *kobj,
			struct attribute *attr, const char *buf, size_t count)
{
	int ret;
	unsigned long val;

	ret = strict_strtoul(buf, 0, &val);
	if (ret < 0)
		return ret;
	timer_rate = val;
	return count;
}

static struct global_attr timer_rate_attr = __ATTR(timer_rate, 0644,
		show_timer_rate, store_timer_rate);

static struct attribute *tripndroid_attributes[] = {
	&hispeed_freq_attr.attr,
	&go_hispeed_load_attr.attr,
	&down_sample_time_attr.attr,
	&up_sample_time_attr.attr,
	&timer_rate_attr.attr,
	NULL,
};

static struct attribute_group tripndroid_attr_group = {
	.attrs = tripndroid_attributes,
	.name = "tripndroid",
};

static int cpufreq_governor_tripndroid(struct cpufreq_policy *policy,
		unsigned int event)
{
	int rc;
	unsigned int j;
	struct cpufreq_tripndroid_cpuinfo *pcpu;
	struct cpufreq_frequency_table *freq_table;

	switch (event) {
	case CPUFREQ_GOV_START:
		if (!cpu_online(policy->cpu))
			return -EINVAL;

		freq_table = cpufreq_frequency_get_table(policy->cpu);

		for_each_cpu(j, policy->cpus) {
			pcpu = &per_cpu(cpuinfo, j);
			pcpu->policy = policy;
			pcpu->target_freq = policy->cur;
			pcpu->freq_table = freq_table;
			pcpu->freq_table_size = get_freq_table_size(pcpu->freq_table);
			pcpu->freq_change_time_in_idle =
				get_cpu_idle_time_us(j, &pcpu->freq_change_time);

			pcpu->governor_enabled = 1;
			smp_wmb();
		}

		if (!hispeed_freq)
			hispeed_freq = policy->max;

		/*
		 * Do not register the idle hook and create sysfs
		 * entries if we have already done so.
		 */
		if (atomic_inc_return(&active_count) > 1)
			return 0;

		rc = sysfs_create_group(cpufreq_global_kobject,
				&tripndroid_attr_group);
		if (rc)
			return rc;

		break;

	case CPUFREQ_GOV_STOP:
		for_each_cpu(j, policy->cpus) {
			pcpu = &per_cpu(cpuinfo, j);
			pcpu->governor_enabled = 0;
			smp_wmb();
			del_timer_sync(&pcpu->cpu_timer);

			/*
			 * Reset idle exit time since we may cancel the timer
			 * before it can run after the last idle exit time,
			 * to avoid tripping the check in idle exit for a timer
			 * that is trying to run.
			 */
			pcpu->idle_exit_time = 0;
		}

		flush_work(&freq_scale_down_work);
		if (atomic_dec_return(&active_count) > 0)
			return 0;

		sysfs_remove_group(cpufreq_global_kobject,
				&tripndroid_attr_group);

		break;

	case CPUFREQ_GOV_LIMITS:
		if (policy->max < policy->cur)
			__cpufreq_driver_target(policy,
					policy->max, CPUFREQ_RELATION_H);
		else if (policy->min > policy->cur)
			__cpufreq_driver_target(policy,
					policy->min, CPUFREQ_RELATION_L);
		break;
	}
	return 0;
}

static int cpufreq_tripndroid_idle_notifier(struct notifier_block *nb, unsigned long val, void *data)
{
	switch (val) {
	case IDLE_START:
		cpufreq_tripndroid_idle_start();
		break;
	case IDLE_END:
		cpufreq_tripndroid_idle_end();
		break;
	}

	return 0;
}

static struct notifier_block cpufreq_tripndroid_idle_nb = {
	.notifier_call = cpufreq_tripndroid_idle_notifier,
};

static int __init cpufreq_tripndroid_init(void)
{
	unsigned int i;
	struct cpufreq_tripndroid_cpuinfo *pcpu;
	struct sched_param param = { .sched_priority = MAX_RT_PRIO-1 };

	go_hispeed_load = DEFAULT_GO_HISPEED_LOAD;
	down_sample_time = DEFAULT_DOWN_SAMPLE_TIME;
	up_sample_time = DEFAULT_UP_SAMPLE_TIME;
	timer_rate = DEFAULT_TIMER_RATE;

	/* per-cpu timers */
	for_each_possible_cpu(i) {
		pcpu = &per_cpu(cpuinfo, i);
		init_timer(&pcpu->cpu_timer);
		pcpu->cpu_timer.function = cpufreq_tripndroid_timer;
		pcpu->cpu_timer.data = i;
	}

	up_task = kthread_create(cpufreq_tripndroid_up_task, NULL,
				 "ktripndroidup");
	if (IS_ERR(up_task))
		return PTR_ERR(up_task);

	sched_setscheduler_nocheck(up_task, SCHED_FIFO, &param);
	get_task_struct(up_task);

	down_wq = alloc_workqueue("ktripndroiddown", 0, 1);

	if (!down_wq)
		goto err_freeuptask;

	INIT_WORK(&freq_scale_down_work,
		  cpufreq_tripndroid_freq_down);

	spin_lock_init(&up_cpumask_lock);
	spin_lock_init(&down_cpumask_lock);
	mutex_init(&set_speed_lock);

	idle_notifier_register(&cpufreq_tripndroid_idle_nb);

	return cpufreq_register_governor(&cpufreq_gov_tripndroid);

err_freeuptask:
	put_task_struct(up_task);
	return -ENOMEM;
}

#ifdef CONFIG_CPU_FREQ_DEFAULT_GOV_TRIPNDROID
fs_initcall(cpufreq_tripndroid_init);
#else
module_init(cpufreq_tripndroid_init);
#endif

static void __exit cpufreq_tripndroid_exit(void)
{
	cpufreq_unregister_governor(&cpufreq_gov_tripndroid);
	kthread_stop(up_task);
	put_task_struct(up_task);
	destroy_workqueue(down_wq);
}

module_exit(cpufreq_tripndroid_exit);

MODULE_AUTHOR("TripNDroid <info@tripndroid.com>");
MODULE_DESCRIPTION("'cpufreq_tripndroid' - cpufreq governor for the tripndroid framework");
MODULE_LICENSE("GPL");
