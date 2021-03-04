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

#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/module.h>


	static dev_t example_dev = MKDEV(0, 0);

	static int example_major;

	module_param_named(major, example_major, int, 0644);

	static struct cdev example_cdev;

	static int example_open(struct inode *ind, struct file *filp);
	static int example_release(struct inode *ind, struct file *filp);

	static const struct file_operations fops_example = {
		.owner   =  THIS_MODULE,
		.open    =  example_open,
		.release =  example_release,
	};

	static struct class *example_class;


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

	example_class = class_create(THIS_MODULE, "example_class");
	if (IS_ERR(example_class)) {
		unregister_chrdev_region(example_dev, 1);
		return -EINVAL;
	}

	device_create(example_class, NULL, example_dev, NULL, THIS_MODULE->name);

	cdev_init(&example_cdev, &fops_example);

	err = cdev_add(&example_cdev, example_dev, 1);
	if (err != 0) {
		device_destroy(example_class, example_dev);
		class_destroy(example_class);
		unregister_chrdev_region(example_dev, 1);
		return err;
	}

	return 0;
}


static void __exit example_exit(void)
{
	device_destroy(example_class, example_dev);
	class_destroy(example_class);
	cdev_del(&example_cdev);
	unregister_chrdev_region(example_dev, 1);
}


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


module_init(example_init);
module_exit(example_exit);

MODULE_DESCRIPTION("Device class creation.");
MODULE_AUTHOR("Christophe Blaess <Christophe.Blaess@Logilin.fr>");
MODULE_LICENSE("GPL");

