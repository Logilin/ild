/************************************************************************\
  Exemples de la formation
    "Ecriture de drivers et programmation noyau Linux"
  Chapitre "A.P.I. du noyau"

  (c) 2005-2019 Christophe Blaess
  http://www.blaess.fr/christophe/

\************************************************************************/

	#include <linux/jiffies.h>
	#include <linux/module.h>
	#include <linux/version.h>


static void display_time_values(void)
{

#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,17,0)

	ktime_t   kgs =  ktime_get_seconds();
	ktime_t   kgrs = ktime_get_real_seconds();
	ktime_t   kgn =  ktime_get_ns();
	ktime_t   kgrn = ktime_get_real_ns();

	printk(KERN_INFO "[%s] %s\n", THIS_MODULE->name, __FUNCTION__);
	printk(KERN_INFO "ktime_get_seconds(): %lld\n", kgs);
	printk(KERN_INFO "ktime_get_real_seconds(): %lld\n", kgrs);
	printk(KERN_INFO "ktime_get_ns(): %lld\n", kgn);
	printk(KERN_INFO "ktime_get_real_ns(): %lld\n", kgrn);
#else
	struct timespec gnst;
	int gs = get_seconds();

	getnstimeofday(&gnst);

	printk(KERN_INFO "[%s] %s\n", THIS_MODULE->name, __FUNCTION__);
	printk(KERN_INFO "get_seconds(): %d\n", gs);
	printk(KERN_INFO "getnstimeofday(): %ld.%09ld\n", gnst.tv_sec, gnst.tv_nsec);

#endif

}


static int __init example_init (void)
{
	display_time_values();

	return 0;
}


static void __exit example_exit (void)
{
	display_time_values();
}


	module_init(example_init);
	module_exit(example_exit);

	MODULE_DESCRIPTION("Different representations of current time.");
	MODULE_AUTHOR("Christophe Blaess <Christophe.Blaess@Logilin.fr>");
	MODULE_LICENSE("GPL");

