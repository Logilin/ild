// SPDX-License-Identifier: GPL-2.0
//
// Exemples de la formation
//  "Ecriture de drivers et programmation noyau Linux"
// Chapitre "Driver en mode caracteres"
//
// (c) 2001-2025 Christophe Blaess
//
//    https://www.logilin.fr/
//

#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/module.h>


static dev_t example_dev = MKDEV(0, 0);

static struct cdev example_cdev;


static int example_open(struct inode *ind, struct file *filp)
{
	pr_info("%s - %s()\n", THIS_MODULE->name, __func__);
	return 0;
}


static int example_release(struct inode *ind, struct file *filp)
{
	pr_info("%s - %s()\n", THIS_MODULE->name, __func__);
	return 0;
}


static const struct file_operations example_fops = {
	.owner   =  THIS_MODULE,
	.open    =  example_open,
	.release =  example_release,
};


static int __init example_init(void)
{
	int err;

	err = alloc_chrdev_region(&example_dev, 0, 1, THIS_MODULE->name);
	if (err < 0)
		return err;

	cdev_init(&example_cdev, &example_fops);

	err = cdev_add(&example_cdev, example_dev, 1);
	if (err != 0) {
		unregister_chrdev_region(example_dev, 1);
		return err;
	}

	return 0;
}


static void __exit example_exit(void)
{
	cdev_del(&example_cdev);
	unregister_chrdev_region(example_dev, 1);
}


module_init(example_init);
module_exit(example_exit);

MODULE_DESCRIPTION("open() and release() system calls implementations");
MODULE_AUTHOR("Christophe Blaess <Christophe.Blaess@Logilin.fr>");
MODULE_LICENSE("GPL v2");
