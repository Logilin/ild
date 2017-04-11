/************************************************************************\
  Exemples de la formation
    "Ecriture de drivers et programmation noyau Linux"
  Chapitre "Le noyau Linux et ses modules"

  (c) 2001-2017 Christophe Blaess
  http://www.blaess.fr/christophe/

\************************************************************************/

	#include <linux/init.h>
	#include <linux/module.h>
	#include <linux/kernel.h>
	#include <linux/version.h>


int __init exemple_init(void)
{
	/* Module initialization. */

	return 0;
}


void __exit exemple_exit(void)
{
	/* Module cleanup. */

}


	module_init(exemple_init);
	module_exit(exemple_exit);

	MODULE_DESCRIPTION("Empty module skeleton");
	MODULE_AUTHOR("Christophe Blaess <Christophe.Blaess@Logilin.fr>");
	MODULE_LICENSE("GPL");

