// SPDX-License-Identifier: GPL-2.0
//
// Exemples de la formation
//  "Ecriture de drivers et programmation noyau Linux"
// Chapitre "Le noyau Linux et ses modules"
//
// (c) 2001-2024 Christophe Blaess
//
//    https://www.logilin.fr/
//


#include <linux/module.h>


int __init example_00_init(void)
{
	/* Module initialization. */

	return 0;
}


void __exit example_00_exit(void)
{
	/* Module cleanup. */

}


module_init(example_00_init);
module_exit(example_00_exit);


MODULE_DESCRIPTION("Empty module skeleton");
MODULE_AUTHOR("");
MODULE_LICENSE("GPL v2");

