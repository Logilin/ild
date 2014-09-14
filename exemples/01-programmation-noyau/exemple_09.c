/************************************************************************\
  exemple_09 - Chapitre "Programmer pour le noyau Linux"

  Utilisation des timers de precision.

  Exemples de la formation "Programmation Noyau sous Linux"

  (c) 2005-2014 Christophe Blaess
  http://www.blaess.fr/christophe/

\************************************************************************/

#include <linux/hrtimer.h>
#include <linux/module.h>
#include <linux/sched.h>


static enum hrtimer_restart fonction_timer(struct hrtimer *);
static struct hrtimer htimer;

static int periode_us = 1000;
module_param(periode_us, int, 0644);

static ktime_t kt_periode;


static int __init exemple_09_init (void)
{
	kt_periode = ktime_set(0, 1000 * periode_us);
	hrtimer_init (& htimer, CLOCK_REALTIME, HRTIMER_MODE_REL);
	htimer.function = fonction_timer;
	hrtimer_start(& htimer, kt_periode, HRTIMER_MODE_REL);

	return 0;
}

 
static void __exit exemple_09_exit (void)
{
	hrtimer_cancel(& htimer);
}


static enum hrtimer_restart fonction_timer(struct hrtimer * unused)
{
	static struct timeval tv_prec = {0, 0};
	struct timeval tv;
	long long int ecart_us;
	static long long int ecart_min = -1;
	static long long int ecart_max = -1;
	
	hrtimer_forward_now(& htimer, kt_periode);
	do_gettimeofday(& tv);

	if (tv_prec.tv_sec > 0) {
		ecart_us  = tv.tv_sec - tv_prec.tv_sec;
		ecart_us *= 1000000;
		ecart_us += tv.tv_usec - tv_prec.tv_usec;
		if ((ecart_min < 0) || (ecart_min > ecart_us)) {
			ecart_min = ecart_us;
			printk(KERN_INFO "%s: min=%lld max=%lld\n",
			                 THIS_MODULE->name, ecart_min, ecart_max);
		}
		if ((ecart_max < 0) || (ecart_max < ecart_us)) {
			ecart_max = ecart_us;
			printk(KERN_INFO "%s: min=%lld max=%lld\n",
			                 THIS_MODULE->name, ecart_min, ecart_max);
		}
	}
	tv_prec = tv;
	
	return HRTIMER_RESTART;
}


module_init(exemple_09_init);
module_exit(exemple_09_exit);
MODULE_LICENSE("GPL");

