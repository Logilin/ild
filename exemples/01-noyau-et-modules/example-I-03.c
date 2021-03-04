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
#include "example-I-03.h"


static int __init example_03_init(void)
{
	return 0;
}


static void __exit example_03_exit(void)
{
}


void example_03_hello(int number)
{
	pr_info("Hello, the number is %d\n", number);
}
EXPORT_SYMBOL(example_03_hello);


module_init(example_03_init);
module_exit(example_03_exit);

MODULE_DESCRIPTION("Simple library module.");
MODULE_AUTHOR("Christophe Blaess <Christophe.Blaess@Logilin.fr>");
MODULE_LICENSE("GPL v2");

