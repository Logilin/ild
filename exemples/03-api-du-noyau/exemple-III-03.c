/************************************************************************\
  Exemples de la formation
    "Ecriture de drivers et programmation noyau Linux"
  Chapitre "A.P.I. du noyau"

  (c) 2005-2017 Christophe Blaess
  http://www.blaess.fr/christophe/

\************************************************************************/

	#include <linux/module.h>
	#include <linux/sched.h>
	#include <linux/timer.h>
	#include <linux/version.h>

	static struct timer_list exemple_timer;


#if LINUX_VERSION_CODE < KERNEL_VERSION(4,15,0)
static void exemple_timer_function(unsigned long arg)
{
	struct timer_list * timer = (struct timer_list *) arg;
	struct timeval time_of_day;

	do_gettimeofday(& time_of_day);
	printk(KERN_INFO "%s - %s: time_of_day=%ld.%06ld\n",
	       THIS_MODULE->name, __FUNCTION__, 
	       time_of_day.tv_sec, time_of_day.tv_usec);

	mod_timer(timer, jiffies + HZ);
}
#else
static void exemple_timer_function(struct timer_list *timer)
{
	struct timeval time_of_day;

	do_gettimeofday(& time_of_day);
	printk(KERN_INFO "%s - %s: time_of_day=%ld.%06ld\n",
	       THIS_MODULE->name, __FUNCTION__, 
	       time_of_day.tv_sec, time_of_day.tv_usec);

	mod_timer(timer, jiffies + HZ);
}
#endif



static int __init exemple_init (void)
{
#if LINUX_VERSION_CODE < KERNEL_VERSION(4,15,0)
	init_timer (& exemple_timer);
	exemple_timer.function = exemple_timer_function;
	exemple_timer.data = (unsigned long) (& exemple_timer);
#else
	timer_setup (& exemple_timer, exemple_timer_function, 0);
#endif
	exemple_timer.expires = jiffies + HZ;
	add_timer(& exemple_timer);

	return 0;
}


static void __exit exemple_exit (void)
{
	del_timer(& exemple_timer);
}


	module_init(exemple_init);
	module_exit(exemple_exit);

	MODULE_DESCRIPTION("Periodic message (current time).");
	MODULE_AUTHOR("Christophe Blaess <Christophe.Blaess@Logilin.fr>");
	MODULE_LICENSE("GPL");

