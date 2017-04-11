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
	exemple_03_hello(10);
	return 0;
}


static void __exit exemple_exit (void)
{
	exemple_03_hello(20);
}


	module_init(exemple_init);
	module_exit(exemple_exit);

	MODULE_DESCRIPTION("External symbol importation.");
	MODULE_AUTHOR("Christophe Blaess <Christophe.Blaess@Logilin.fr>");
	MODULE_LICENSE("GPL");

