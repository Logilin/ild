/************************************************************************\
  Exemples de la formation
    "Ecriture de drivers et programmation noyau Linux"
  Chapitre "A.P.I. du noyau"

  (c) 2005-2019 Christophe Blaess
  http://www.blaess.fr/christophe/

\************************************************************************/

	#include <linux/hrtimer.h>
	#include <linux/module.h>
	#include <linux/sched.h>


	static enum hrtimer_restart example_htimer_function(struct hrtimer *);
	static struct hrtimer example_htimer;

	static int period_us = 1000;
	module_param(period_us, int, 0644);

	static ktime_t example_period_kt;


static int __init example_init (void)
{
	example_period_kt = ktime_set(0, 1000 * period_us);

	hrtimer_init (& example_htimer, CLOCK_REALTIME, HRTIMER_MODE_REL);
	example_htimer.function = example_htimer_function;

	hrtimer_start(& example_htimer, example_period_kt, HRTIMER_MODE_REL);

	return 0;
}


static void __exit example_exit (void)
{
	hrtimer_cancel(& example_htimer);
}


static enum hrtimer_restart example_htimer_function(struct hrtimer * unused)
{
	ktime_t  now;
	static ktime_t  previous = 0;

	ktime_t elapsed;
	static ktime_t elapsed_min = -1;
	static ktime_t elapsed_max = -1;

	hrtimer_forward_now(& example_htimer, example_period_kt);

	now = ktime_get_real_ns();

	if (previous > 0) {
		elapsed  = ktime_sub(now, previous);
		if ((elapsed_min < 0) || (ktime_compare(elapsed, elapsed_min) < 0)) {
			elapsed_min = elapsed;
			printk(KERN_INFO "%s - %s: min=%lld  max=%lld\n",
			       THIS_MODULE->name, __FUNCTION__, elapsed_min, elapsed_max);
		}
		if ((elapsed_max < 0) || (ktime_compare(elapsed, elapsed_max) > 0)) {
			elapsed_max = elapsed;
			printk(KERN_INFO "%s - %s: min=%lld  max=%lld\n",
			       THIS_MODULE->name, __FUNCTION__, elapsed_min, elapsed_max);
		}
	}
	previous = now;

	return HRTIMER_RESTART;
}


	module_init(example_init);
	module_exit(example_exit);

	MODULE_DESCRIPTION("Jitter of a precise timer.");
	MODULE_AUTHOR("Christophe Blaess <Christophe.Blaess@Logilin.fr>");
	MODULE_LICENSE("GPL");

