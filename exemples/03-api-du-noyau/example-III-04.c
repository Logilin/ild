// SPDX-License-Identifier: GPL-2.0
//
// Exemples de la formation
//  "Ecriture de drivers et programmation noyau Linux"
// Chapitre "A.P.I du noyau"
//
// (c) 2001-2024 Christophe Blaess
//
//    https://www.logilin.fr/
//

#include <linux/hrtimer.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/version.h>


static struct hrtimer example_htimer;

static int period_us = 1000;
module_param(period_us, int, 0644);

static ktime_t example_period_kt;


static enum hrtimer_restart example_htimer_function(struct hrtimer *unused)
{
	time64_t now;
	static time64_t previous = -1;
	time64_t elapsed;
	static time64_t elapsed_min = -1;
	static time64_t elapsed_max = -1;

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


module_init(example_init);
module_exit(example_exit);

MODULE_DESCRIPTION("Jitter of a precise timer.");
MODULE_AUTHOR("Christophe Blaess <Christophe.Blaess@Logilin.fr>");
MODULE_LICENSE("GPL v2");
