/************************************************************************\
  exemple_05 - Chapitre "Programmer pour le noyau Linux"

  Ecriture au chargement et dechargement du nombre de jiffies ecoules
  depuis le boot.

  Exemples de la formation "Programmation Noyau sous Linux"

  (c) 2005-2014 Christophe Blaess
  http://www.blaess.fr/christophe/

\************************************************************************/

#include <linux/jiffies.h>
#include <linux/module.h>

static int __init exemple_06_init (void)
{
	printk(KERN_INFO "%s : HZ=%d, jiffies=%ld\n",
	       THIS_MODULE->name, HZ, jiffies);
	return 0;
}
 
static void __exit exemple_06_exit (void)
{
	printk(KERN_INFO "%s : HZ=%d, jiffies=%ld\n",
	       THIS_MODULE->name, HZ, jiffies);
}

module_init(exemple_06_init);
module_exit(exemple_06_exit);

MODULE_LICENSE("GPL");

