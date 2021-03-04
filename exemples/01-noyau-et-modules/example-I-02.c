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

static int input = 1234;
module_param(input, int, 0644);
MODULE_PARM_DESC(input, "The input channel number.");


static char *example_label = THIS_MODULE->name;
module_param_named(label, example_label, charp, 0644);
MODULE_PARM_DESC(label, "The label to use during communication.");


static int param_table[8];
static int nb_items;
module_param_array(param_table, int, &nb_items, 0644);


static int __init example_02_init(void)
{
	int i;

	pr_info("%s: input = %d\n", THIS_MODULE->name, input);
	pr_info("%s: example_label = %s\n", THIS_MODULE->name, example_label);
	pr_info("%s: nb_items = %d\n", THIS_MODULE->name, nb_items);
	pr_info("%s: Table = ", THIS_MODULE->name);
	for (i = 0; i < nb_items; i++)
		pr_cont("%d ", param_table[i]);
	pr_cont("\n");

	return 0;
}


static void __exit example_02_exit(void)
{
}


module_init(example_02_init);
module_exit(example_02_exit);

MODULE_DESCRIPTION("Parameters on the insmod command line");
MODULE_AUTHOR("Christophe Blaess <Christophe.Blaess@Logilin.fr>");
MODULE_LICENSE("GPL v2");
