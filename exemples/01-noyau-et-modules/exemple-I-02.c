/************************************************************************\
  Exemples de la formation
    "Ecriture de drivers et programmation noyau Linux"
  Chapitre "Le noyau Linux et ses modules"

  (c) 2001-2019 Christophe Blaess
  http://www.blaess.fr/christophe/

\************************************************************************/

	#include <linux/module.h>

	static int input = 0;
	module_param(input, int, 0644);
	MODULE_PARM_DESC(input, "The input channel number.");


	static char * example_label = THIS_MODULE->name;
	module_param_named(label, example_label, charp, 0644);
	MODULE_PARM_DESC(label, "The label to use during communication.");


	static int param_table[8];
	static int nb_items;
	module_param_array(param_table, int, & nb_items, 0644);


static int __init example_init (void)
{
	int i;

	printk(KERN_INFO "%s: input = %d\n", THIS_MODULE->name, input);
	printk(KERN_INFO "%s: example_label = %s\n", THIS_MODULE->name, example_label);
	printk(KERN_INFO "%s: nb_items = %d\n", THIS_MODULE->name, nb_items);
	printk(KERN_INFO "%s: Table = ", THIS_MODULE->name);
	for (i = 0; i < nb_items; i ++)
		printk(KERN_CONT "%d ", param_table[i]);
	printk(KERN_CONT "\n");

	return 0;
}



static void __exit example_exit (void)
{
}


	module_init(example_init);
	module_exit(example_exit);

	MODULE_DESCRIPTION("Parameters on the insmod command line");
	MODULE_AUTHOR("Christophe Blaess <Christophe.Blaess@Logilin.fr>");
	MODULE_LICENSE("GPL");

