/************************************************************************\
  exemple_08 - Chapitre "Programmer pour le noyau Linux"

  Ecriture de l'heure precise toutes les secondes.

  Exemples de la formation "Programmation Noyau sous Linux"

  (c) 2005-2014 Christophe Blaess
  http://www.blaess.fr/christophe/

\************************************************************************/

#include <linux/module.h>
#include <linux/sched.h>

static void fonction_periodique (unsigned long);

static struct timer_list timer;

static int __init exemple_08_init (void)
{
	init_timer (& timer);
	timer.function = fonction_periodique;
	timer.data = 0; /* inutilise */
	timer.expires = jiffies + HZ;
	add_timer(& timer);

	return 0;
}
 
static void __exit exemple_08_exit (void)
{
	del_timer(& timer);
}

static void fonction_periodique(unsigned long inutile)
{
	struct timeval time_of_day;

	do_gettimeofday(& time_of_day);
	printk(KERN_INFO "%s - time_of_day: %ld.%06ld\n",
	       THIS_MODULE->name, time_of_day.tv_sec, time_of_day.tv_usec);

	mod_timer(& timer, jiffies+HZ);
}

module_init(exemple_08_init);
module_exit(exemple_08_exit);
MODULE_LICENSE("GPL");
