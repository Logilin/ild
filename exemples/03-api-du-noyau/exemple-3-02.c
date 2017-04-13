/************************************************************************\
  Exemples de la formation
    "Ecriture de drivers et programmation noyau Linux"
  Chapitre "Programmer pour le noyau Linux"

  (c) 2005-2015 Christophe Blaess
  http://www.blaess.fr/christophe/

\************************************************************************/

	#include <linux/jiffies.h>
	#include <linux/module.h>


static int __init exemple_init (void)
{
	printk(KERN_INFO "%s: HZ=%d, jiffies=%ld\n",
	       THIS_MODULE->name, HZ, jiffies);
	return 0;
}


static void __exit exemple_exit (void)
{
	printk(KERN_INFO "%s: HZ=%d, jiffies=%ld\n",
	       THIS_MODULE->name, HZ, jiffies);
}


	module_init(exemple_init);
	module_exit(exemple_exit);

	MODULE_DESCRIPTION("Current jiffies values at loading and cleanup.");
	MODULE_AUTHOR("Christophe Blaess <Christophe.Blaess@Logilin.fr>");
	MODULE_LICENSE("GPL");

