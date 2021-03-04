// SPDX-License-Identifier: GPL-2.0
//
// Exemples de la formation
//  "Ecriture de drivers et programmation noyau Linux"
// Chapitre "Le noyau Linux et ses modules"
//
// (c) 2001-2021 Christophe Blaess
//
//    https://www.logilin.fr/
//


#include <linux/module.h>


static int __init example_01_init(void)
{
	printk(KERN_INFO "%s: Hello...\n", THIS_MODULE->name);

	return 0;
}


static void __exit example_01_exit(void)
{
	printk(KERN_INFO "%s: Bye!\n", THIS_MODULE->name);
}


module_init(example_01_init);
module_exit(example_01_exit);


MODULE_DESCRIPTION("Simple 'Hello World' style kernel module");
MODULE_AUTHOR("Christophe Blaess <Christophe.Blaess@Logilin.fr>");
MODULE_LICENSE("GPL v2");

