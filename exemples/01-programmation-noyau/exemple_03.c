/************************************************************************\
  exemple_03 - Chapitre "Programmer pour le noyau Linux"

  Exportation d'un symbole a destination des autres modules.

  Exemples de la formation "Programmation Noyau sous Linux"

  (c) 2005-2014 Christophe Blaess
  http://www.blaess.fr/christophe/

\************************************************************************/

#include <linux/module.h>
#include "exemple_03.h"

static int __init exemple_03_init (void)
{
	return 0; 
}

static void __exit exemple_03_exit (void)
{
}

void exemple_03_hello(int numero)
{
	printk (KERN_INFO "Hello, le numero est %d\n", numero);
}

EXPORT_SYMBOL(exemple_03_hello);


module_init(exemple_03_init);
module_exit(exemple_03_exit);

MODULE_LICENSE("GPL");

