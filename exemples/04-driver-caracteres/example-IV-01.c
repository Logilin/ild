// SPDX-License-Identifier: GPL-2.0
//
// Exemples de la formation
//  "Ecriture de drivers et programmation noyau Linux"
// Chapitre "Driver en mode caracteres"
//
// (c) 2001-2021 Christophe Blaess
//
//    https://www.logilin.fr/
//


#include <linux/fs.h>
#include <linux/module.h>

	static dev_t example_dev = MKDEV(0, 0);

	static int example_major;

	module_param_named(major, example_major, int, 0644);


static int __init example_init(void)
{
	int err;

	if (example_major == 0) {
		err = alloc_chrdev_region(&example_dev, 0, 1, THIS_MODULE->name);
	} else {
		example_dev = MKDEV(example_major, 0);
		err = register_chrdev_region(example_dev, 1, THIS_MODULE->name);
	}

	if (err < 0)
		return err;

	return 0;
}


static void __exit example_exit(void)
{
	unregister_chrdev_region(example_dev, 1);
}


module_init(example_init);
module_exit(example_exit);

MODULE_DESCRIPTION("Major number and minor range reservation");
MODULE_AUTHOR("Christophe Blaess <Christophe.Blaess@Logilin.fr>");
MODULE_LICENSE("GPL v2");

