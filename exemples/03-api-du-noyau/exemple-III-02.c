/************************************************************************\
  Exemples de la formation
    "Ecriture de drivers et programmation noyau Linux"
  Chapitre "A.P.I. du noyau"

  (c) 2005-2017 Christophe Blaess
  http://www.blaess.fr/christophe/

\************************************************************************/

	#include <linux/jiffies.h>
	#include <linux/module.h>


static int __init exemple_init (void)
{
	struct timeval  tv_jif, tv_tod;
	struct timespec ts_ckt, ts_tod;

	jiffies_to_timeval(jiffies, & tv_jif);
	do_gettimeofday(& tv_tod);
	ts_ckt = current_kernel_time();
	getnstimeofday(& ts_tod);

	printk(KERN_INFO  "%s - %s():\n", THIS_MODULE->name, __FUNCTION__);
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


static void __exit exemple_exit (void)
{
	struct timeval tv_jif, tv_tod;
	struct timespec ts_ckt, ts_tod;

	jiffies_to_timeval(jiffies, & tv_jif);
	do_gettimeofday(& tv_tod);
	ts_ckt = current_kernel_time();
	getnstimeofday(& ts_tod);

	printk(KERN_INFO  "%s - %s():\n", THIS_MODULE->name, __FUNCTION__);
	printk(KERN_INFO "tv_jif.tv_sec = %ld, tv_jif.tv_usec = %ld\n",
	                  tv_jif.tv_sec, tv_jif.tv_usec);
	printk(KERN_INFO "tv_tod.tv_sec = %ld, tv_tod.tv_usec = %ld\n",
	                  tv_tod.tv_sec, tv_tod.tv_usec);
	printk(KERN_INFO "ts_ckt.tv_sec = %ld, ts_ckt.tv_nsec = %ld\n",
	                  ts_ckt.tv_sec, ts_ckt.tv_nsec);
	printk(KERN_INFO "ts_tod.tv_sec = %ld, ts_tod.tv_nsec = %ld\n",
	                  ts_tod.tv_sec, ts_tod.tv_nsec);
}


	module_init(exemple_init);
	module_exit(exemple_exit);

	MODULE_DESCRIPTION("Different representations of current time.");
	MODULE_AUTHOR("Christophe Blaess <Christophe.Blaess@Logilin.fr>");
	MODULE_LICENSE("GPL");

