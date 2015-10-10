/************************************************************************\
  exemple_03 - Chapitre "Programmer pour le noyau Linux"

  Exportation d'un symbole a destination des autres modules.

  Exemples de la formation "Programmation Noyau sous Linux"

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

	MODULE_LICENSE("GPL");
