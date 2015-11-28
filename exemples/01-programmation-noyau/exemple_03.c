/************************************************************************\
  Exemples de la formation
    "Ecriture de drivers et programmation noyau Linux"
  Chapitre "Programmer pour le noyau Linux"

  (c) 2005-2015 Christophe Blaess
  http://www.blaess.fr/christophe/

\************************************************************************/

	#include <linux/module.h>
	#include "exemple_03.h"


static int __init exemple_init (void)
{
	return 0; 
}


static void __exit exemple_exit (void)
{
}


void exemple_03_hello(int number)
{
	printk (KERN_INFO "Hello, the number is %d\n", number);
}

EXPORT_SYMBOL(exemple_03_hello);


	module_init(exemple_init);
	module_exit(exemple_exit);

	MODULE_DESCRIPTION("Simple library module.");
	MODULE_AUTHOR("Christophe Blaess <Christophe.Blaess@Logilin.fr>");
	MODULE_LICENSE("GPL");

