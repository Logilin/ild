// SPDX-License-Identifier: GPL-2.0
//
// Exemples de la formation
//  "Ecriture de drivers et programmation noyau Linux"
// Chapitre "A.P.I du noyau"
//
// (c) 2001-2021 Christophe Blaess
//
//    https://www.logilin.fr/
//

#include <linux/hrtimer.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/version.h>


static enum hrtimer_restart example_htimer_function(struct hrtimer *);
static struct hrtimer example_htimer;

static int period_us = 1000;
module_param(period_us, int, 0644);

static ktime_t example_period_kt;


static int __init example_init(void)
{
	example_period_kt = ktime_set(0, 1000 * period_us);

	hrtimer_init(&example_htimer, CLOCK_REALTIME, HRTIMER_MODE_REL);
	example_htimer.function = example_htimer_function;

	hrtimer_start(&example_htimer, example_period_kt, HRTIMER_MODE_REL);

	return 0;
}


static void __exit example_exit(void)
{
	hrtimer_cancel(&example_htimer);
}


#if LINUX_VERSION_CODE >= KERNEL_VERSION(3, 17, 0)
static enum hrtimer_restart example_htimer_function(struct hrtimer *unused)
{
	ktime_t now;
	static ktime_t previous = -1;
	ktime_t elapsed;
	static ktime_t elapsed_min = -1;
	static ktime_t elapsed_max = -1;

	hrtimer_forward_now(&example_htimer, example_period_kt);

	now = ktime_get_real_ns();

	if (previous > 0) {
		elapsed  = ktime_sub(now, previous);
		if ((elapsed_min < 0) || (ktime_compare(elapsed, elapsed_min) < 0)) {
			elapsed_min = elapsed;
			pr_info("%s: min=%lld  max=%lld\n",
			       THIS_MODULE->name, elapsed_min, elapsed_max);
		}
		if ((elapsed_max < 0) || (ktime_compare(elapsed, elapsed_max) > 0)) {
			elapsed_max = elapsed;
			pr_info("%s: min=%lld  max=%lld\n",
			       THIS_MODULE->name, elapsed_min, elapsed_max);
		}
	}
	previous = now;

	return HRTIMER_RESTART;
}

#else

static enum hrtimer_restart example_htimer_function(struct hrtimer *unused)
{
	struct timespec now;
	static struct timespec previous = { 0, 0 };
	long long elapsed;
	static long long  elapsed_min = -1;
	static long long elapsed_max = -1;

	hrtimer_forward_now(&example_htimer, example_period_kt);

	getnstimeofday(&now);

	if (previous.tv_sec > 0) {
		elapsed  = now.tv_sec - previous.tv_sec;
		elapsed *= 1000000000;
		elapsed += now.tv_nsec - previous.tv_nsec;
		if ((elapsed_min < 0) || (elapsed < elapsed_min)) {
			elapsed_min = elapsed;
			pr_info("%s: min=%lld  max=%lld\n",
			       THIS_MODULE->name, elapsed_min, elapsed_max);
		}
		if ((elapsed_max < 0) || (elapsed > elapsed_max)) {
			elapsed_max = elapsed;
			pr_info("%s: min=%lld  max=%lld\n",
			       THIS_MODULE->name, elapsed_min, elapsed_max);
		}
	}
	previous = now;

	return HRTIMER_RESTART;
}
#endif


module_init(example_init);
module_exit(example_exit);

MODULE_DESCRIPTION("Jitter of a precise timer.");
MODULE_AUTHOR("Christophe Blaess <Christophe.Blaess@Logilin.fr>");
MODULE_LICENSE("GPL v2");

