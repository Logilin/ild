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


static int __init example_03_init(void)
{
	example_02_hello(10);
	return 0;
}


static void __exit example_03_exit(void)
{
	example_02_hello(20);
}


module_init(example_03_init);
module_exit(example_03_exit);

MODULE_DESCRIPTION("External symbol importation.");
MODULE_AUTHOR("Christophe Blaess <Christophe.Blaess@Logilin.fr>");
MODULE_LICENSE("GPL v2");

