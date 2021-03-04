// SPDX-License-Identifier: GPL-2.0
//
// Exemples de la formation
//  "Ecriture de drivers et programmation noyau Linux"
// Chapitre "A.P.I du noyau"
//
// (c) 2001-2021 Christophe Blaess
//
//    https://www.logilin.fr/
//

#include <linux/jiffies.h>
#include <linux/module.h>


static int __init example_init(void)
{
	pr_info("%s: HZ=%d, jiffies=%ld\n",
		THIS_MODULE->name, HZ, jiffies);
	return 0;
}


static void __exit example_exit(void)
{
	pr_info("%s: HZ=%d, jiffies=%ld\n",
		THIS_MODULE->name, HZ, jiffies);
}


module_init(example_init);
module_exit(example_exit);

MODULE_DESCRIPTION("Current jiffies values at loading and cleanup.");
MODULE_AUTHOR("Christophe Blaess <Christophe.Blaess@Logilin.fr>");
MODULE_LICENSE("GPL v2");

