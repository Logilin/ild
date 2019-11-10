/************************************************************************\
  Exemples de la formation
    "Ecriture de drivers et programmation noyau Linux"
  Chapitre "Le noyau Linux et ses modules"

  (c) 2001-2017 Christophe Blaess
  http://www.blaess.fr/christophe/

\************************************************************************/

	#include <linux/module.h>
	#include "exemple-I-03.h"


static int __init example_init (void)
{
	example_hello(10);
	return 0;
}


static void __exit example_exit (void)
{
	example_hello(20);
}


	module_init(example_init);
	module_exit(example_exit);

	MODULE_DESCRIPTION("External symbol importation.");
	MODULE_AUTHOR("Christophe Blaess <Christophe.Blaess@Logilin.fr>");
	MODULE_LICENSE("GPL");

