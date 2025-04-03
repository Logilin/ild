// SPDX-License-Identifier: GPL-2.0
//
// Exemples de la formation
//  "Ecriture de drivers et programmation noyau Linux"
// Chapitre "Le noyau Linux et ses modules"
//
// (c) 2001-2025 Christophe Blaess
//
//    https://www.logilin.fr/
//


#include <linux/module.h>


int __init example_init(void)
{
	printk(KERN_INFO "%s: %s()\n", THIS_MODULE->name, __FUNCTION__);
	return 0;
}


void __exit example_exit(void)
{
	printk(KERN_INFO "%s: %s()\n", THIS_MODULE->name, __FUNCTION__);
}


module_init(example_init);
module_exit(example_exit);


MODULE_DESCRIPTION("Exercise skeleton");
MODULE_AUTHOR("My Name");
MODULE_LICENSE("GPL v2");

