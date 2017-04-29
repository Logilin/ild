/************************************************************************\
  Exemples de la formation
    "Ecriture de drivers et programmation noyau Linux"
  Chapitre "Le noyau Linux et ses modules"

  (c) 2001-2017 Christophe Blaess
  http://www.blaess.fr/christophe/

\************************************************************************/

	#include <linux/module.h>


static int __init exemple_init (void)
{
	printk(KERN_INFO "%s: Hello...\n", THIS_MODULE->name);

	return 0; 
}


static void __exit exemple_exit (void)
{
	printk(KERN_INFO "%s: Bye!\n", THIS_MODULE->name);
}


	module_init(exemple_init);
	module_exit(exemple_exit);

	MODULE_DESCRIPTION("Simple 'Hello World' style kernel module");
	MODULE_AUTHOR("Christophe Blaess <Christophe.Blaess@Logilin.fr>");
	MODULE_LICENSE("GPL");

