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
#include "example-I-02.h"


static int __init example_02_init(void)
{
	return 0;
}


static void __exit example_02_exit(void)
{
}


void example_02_hello(int number)
{
	pr_info("Hello, the number is %d\n", number);
}
EXPORT_SYMBOL(example_02_hello);


module_init(example_02_init);
module_exit(example_02_exit);

MODULE_DESCRIPTION("Simple library module.");
MODULE_AUTHOR("Christophe Blaess <Christophe.Blaess@Logilin.fr>");
MODULE_LICENSE("GPL v2");

