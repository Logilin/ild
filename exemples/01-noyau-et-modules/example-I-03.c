/************************************************************************\
  Exemples de la formation
    "Ecriture de drivers et programmation noyau Linux"
  Chapitre "Le noyau Linux et ses modules"

  (c) 2001-2019 Christophe Blaess
  http://www.blaess.fr/christophe/

\************************************************************************/

	#include <linux/module.h>
	#include "example-I-03.h"


static int __init example_init (void)
{
	return 0;
}


static void __exit example_exit (void)
{
}


void example_hello(int number)
{
	printk (KERN_INFO "Hello, the number is %d\n", number);
}

EXPORT_SYMBOL(example_hello);


	module_init(example_init);
	module_exit(example_exit);

	MODULE_DESCRIPTION("Simple library module.");
	MODULE_AUTHOR("Christophe Blaess <Christophe.Blaess@Logilin.fr>");
	MODULE_LICENSE("GPL");

