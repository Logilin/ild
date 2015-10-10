/************************************************************************\

  Squelette minimal de module du noyau

  (c) 2005-2015 Christophe Blaess
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

	MODULE_AUTHOR("Christophe Blaess <Christophe.Blaess@Logilin.fr>");
	MODULE_DESCRIPTION("This is an empty module skeleton");
	MODULE_LICENSE("GPL");
