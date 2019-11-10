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
	ktime_t   kgs =  ktime_get_seconds();
	ktime_t   kgrs = ktime_get_real_seconds();
	ktime_t   kgn =  ktime_get_ns();
	ktime_t   kgrn = ktime_get_real_ns();

	printk(KERN_INFO "[%s] %s\n", THIS_MODULE->name, __FUNCTION__);
	printk(KERN_INFO "ktime_get_seconds(): %lld\n", kgs);
	printk(KERN_INFO "ktime_get_real_seconds(): %lld\n", kgrs);
	printk(KERN_INFO "ktime_get_ns(): %lld\n", kgn);
	printk(KERN_INFO "ktime_get_real_ns(): %lld\n", kgrn);

	return 0;
}


static void __exit exemple_exit (void)
{
	ktime_t   kgs =  ktime_get_seconds();
	ktime_t   kgrs = ktime_get_real_seconds();
	ktime_t   kgn =  ktime_get_ns();
	ktime_t   kgrn = ktime_get_real_ns();

	printk(KERN_INFO "[%s] %s\n", THIS_MODULE->name, __FUNCTION__);
	printk(KERN_INFO "ktime_get_seconds(): %lld\n", kgs);
	printk(KERN_INFO "ktime_get_real_seconds(): %lld\n", kgrs);
	printk(KERN_INFO "ktime_get_ns(): %lld\n", kgn);
	printk(KERN_INFO "ktime_get_real_ns(): %lld\n", kgrn);
}


	module_init(exemple_init);
	module_exit(exemple_exit);

	MODULE_DESCRIPTION("Different representations of current time.");
	MODULE_AUTHOR("Christophe Blaess <Christophe.Blaess@Logilin.fr>");
	MODULE_LICENSE("GPL");

