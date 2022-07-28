// SPDX-License-Identifier: GPL-2.0
//
// Exemples de la formation
//  "Ecriture de drivers et programmation noyau Linux"
// Chapitre "Le noyau Linux et ses modules"
//
// (c) 2001-2022 Christophe Blaess
//
//    https://www.logilin.fr/
//


#include <linux/module.h>


int __init example_00_init(void)
{
	/* Module initialization. */
	printk(KERN_INFO "Hello from my module\n");
	return 0;
}


void __exit example_00_exit(void)
{
	/* Module cleanup. */
	printk(KERN_INFO "Bye bye!\n");
}


module_init(example_00_init);
module_exit(example_00_exit);


MODULE_DESCRIPTION("Exercise I-05 solution");
MODULE_AUTHOR("My Name");
MODULE_LICENSE("GPL v2");

