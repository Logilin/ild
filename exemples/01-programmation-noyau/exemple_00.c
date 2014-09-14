/************************************************************************\

  Squelette minimal de module du noyau

  (c) 2005-2014 Christophe Blaess
  http://www.blaess.fr/christophe/

\************************************************************************/

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/version.h>

MODULE_AUTHOR("Christophe Blaess");
MODULE_DESCRIPTION("Ceci est un squelette de module");
MODULE_LICENSE("GPL");

int __init initialisation_module(void)
{
	/* initialisation */

	return 0;
}


void __exit suppression_module(void)
{
	/* liberation */

}


module_init(initialisation_module);
module_exit(suppression_module);

