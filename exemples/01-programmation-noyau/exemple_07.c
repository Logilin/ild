/************************************************************************\
  exemple_07 - Chapitre "Programmer pour le noyau Linux"

  Ecriture au chargement et dechargement des heures precises.

  Exemples de la formation "Programmation Noyau sous Linux"

  (c) 2005-2014 Christophe Blaess
  http://www.blaess.fr/christophe/

\************************************************************************/

#include <linux/jiffies.h>
#include <linux/module.h>

static int __init exemple_07_init (void)
{
	struct timeval  tv_jif, tv_tod;
	struct timespec ts_ckt, ts_tod;

	jiffies_to_timeval(jiffies, & tv_jif);
	do_gettimeofday(& tv_tod);
	ts_ckt = current_kernel_time();
	getnstimeofday(& ts_tod);
	printk(KERN_INFO  "%s - Chargement :\n", THIS_MODULE->name);
	printk(KERN_INFO "tv_jif.tv_sec = %ld, tv_jif.tv_usec = %ld\n",
	                  tv_jif.tv_sec, tv_jif.tv_usec);
	printk(KERN_INFO "tv_tod.tv_sec = %ld, tv_tod.tv_usec = %ld\n",
	                  tv_tod.tv_sec, tv_tod.tv_usec);
	printk(KERN_INFO "ts_ckt.tv_sec = %ld, ts_ckt.tv_nsec = %ld\n",
	                  ts_ckt.tv_sec, ts_ckt.tv_nsec);
	printk(KERN_INFO "ts_tod.tv_sec = %ld, ts_tod.tv_nsec = %ld\n",
	                  ts_tod.tv_sec, ts_tod.tv_nsec);
	return 0;
}
 
static void __exit exemple_07_exit (void)
{
	struct timeval tv_jif, tv_tod;
	struct timespec ts_ckt, ts_tod;

	jiffies_to_timeval(jiffies, & tv_jif);
	do_gettimeofday(& tv_tod);
	ts_ckt = current_kernel_time();
	getnstimeofday(& ts_tod);

	printk(KERN_INFO  "%s - Dechargement :\n", THIS_MODULE->name);
	printk(KERN_INFO "tv_jif.tv_sec = %ld, tv_jif.tv_usec = %ld\n",
	                  tv_jif.tv_sec, tv_jif.tv_usec);
	printk(KERN_INFO "tv_tod.tv_sec = %ld, tv_tod.tv_usec = %ld\n",
	                  tv_tod.tv_sec, tv_tod.tv_usec);
	printk(KERN_INFO "ts_ckt.tv_sec = %ld, ts_ckt.tv_nsec = %ld\n",
	                  ts_ckt.tv_sec, ts_ckt.tv_nsec);
	printk(KERN_INFO "ts_tod.tv_sec = %ld, ts_tod.tv_nsec = %ld\n",
	                  ts_tod.tv_sec, ts_tod.tv_nsec);
}

module_init(exemple_07_init);
module_exit(exemple_07_exit);

MODULE_LICENSE("GPL");

