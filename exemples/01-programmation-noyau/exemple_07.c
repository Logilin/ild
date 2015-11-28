/************************************************************************\
  Exemples de la formation
    "Ecriture de drivers et programmation noyau Linux"
  Chapitre "Programmer pour le noyau Linux"

  (c) 2005-2015 Christophe Blaess
  http://www.blaess.fr/christophe/

\************************************************************************/

	#include <linux/module.h>
	#include <linux/sched.h>


	static void exemple_timer_function(unsigned long);
	static struct timer_list exemple_timer;


static int __init exemple_init (void)
{
	init_timer (& exemple_timer);
	exemple_timer.function = exemple_timer_function;
	exemple_timer.data = (unsigned long) (& exemple_timer);
	exemple_timer.expires = jiffies + HZ;
	add_timer(& exemple_timer);

	return 0;
}


static void __exit exemple_exit (void)
{
	del_timer(& exemple_timer);
}


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


	module_init(exemple_init);
	module_exit(exemple_exit);

	MODULE_DESCRIPTION("Periodic message (current time).");
	MODULE_AUTHOR("Christophe Blaess <Christophe.Blaess@Logilin.fr>");
	MODULE_LICENSE("GPL");

