/************************************************************************\
  exemple_08 - Chapitre "Programmer pour le noyau Linux"

  Utilisation des timers de precision.

  Exemples de la formation "Programmation Noyau sous Linux"

  (c) 2005-2015 Christophe Blaess
  http://www.blaess.fr/christophe/

\************************************************************************/

	#include <linux/hrtimer.h>
	#include <linux/module.h>
	#include <linux/sched.h>


	static enum hrtimer_restart exemple_htimer_function(struct hrtimer *);
	static struct hrtimer exemple_htimer;

	static int period_us = 1000;
	module_param(period_us, int, 0644);

	static ktime_t exemple_period_kt;



static int __init exemple_init (void)
{
	exemple_period_kt = ktime_set(0, 1000 * period_us);

	hrtimer_init (& exemple_htimer, CLOCK_REALTIME, HRTIMER_MODE_REL);
	exemple_htimer.function = exemple_htimer_function;

	hrtimer_start(& exemple_htimer, exemple_period_kt, HRTIMER_MODE_REL);

	return 0;
}



static void __exit exemple_exit (void)
{
	hrtimer_cancel(& exemple_htimer);
}



static enum hrtimer_restart exemple_htimer_function(struct hrtimer * unused)
{
	struct timeval tv;
	static struct timeval tv_prev = {0, 0};

	long long int elapsed_us;
	static long long int elapsed_min = -1;
	static long long int elapsed_max = -1;

	hrtimer_forward_now(& exemple_htimer, exemple_period_kt);

	do_gettimeofday(& tv);

	if (tv_prev.tv_sec > 0) {
		elapsed_us  = tv.tv_sec - tv_prev.tv_sec;
		elapsed_us *= 1000000;
		elapsed_us += tv.tv_usec - tv_prev.tv_usec;
		if ((elapsed_min < 0) || (elapsed_us < elapsed_min)) {
			elapsed_min = elapsed_us;
			printk(KERN_INFO "%s - %s: min=%lld  max=%lld\n",
			       THIS_MODULE->name, __FUNCTION__, elapsed_min, elapsed_max);
		}
		if ((elapsed_max < 0) || (elapsed_us > elapsed_max)) {
			elapsed_max = elapsed_us;
			printk(KERN_INFO "%s - %s: min=%lld  max=%lld\n",
			       THIS_MODULE->name, __FUNCTION__, elapsed_min, elapsed_max);
		}
	}
	tv_prev = tv;

	return HRTIMER_RESTART;
}


	module_init(exemple_init);
	module_exit(exemple_exit);
	MODULE_LICENSE("GPL");
