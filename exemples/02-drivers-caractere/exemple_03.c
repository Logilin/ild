/************************************************************************\
  Exemples de la formation
    "Ecriture de drivers et programmation noyau Linux"
  Chapitre "Ecriture de driver en mode caractere"

  (c) 2005-2015 Christophe Blaess
  http://www.blaess.fr/christophe/

\************************************************************************/

	#include <linux/cdev.h>
	#include <linux/device.h>
	#include <linux/fs.h>
	#include <linux/module.h>


	static dev_t exemple_dev = MKDEV(0, 0);

	static int exemple_major = 0;
	module_param_named(major, exemple_major, int, 0644);

	static struct cdev exemple_cdev;

	static int exemple_open    (struct inode * ind, struct file * filp);
	static int exemple_release (struct inode * ind, struct file * filp);

	static struct file_operations fops_exemple = {
		.owner   =  THIS_MODULE,
		.open    =  exemple_open,
		.release =  exemple_release,
	};

	static struct class * exemple_class = NULL;


static int __init exemple_init (void)
{
	int err;

	if (exemple_major == 0) {
		err = alloc_chrdev_region(& exemple_dev, 0, 1, THIS_MODULE->name);
	} else {
		exemple_dev = MKDEV(exemple_major, 0);
		err = register_chrdev_region(exemple_dev, 1, THIS_MODULE->name);
	}

	if (err < 0)
		return err;

	exemple_class = class_create(THIS_MODULE, "exemple_class");
	if (IS_ERR(exemple_class)) {
		unregister_chrdev_region(exemple_dev, 1);
		return -EINVAL;
	}

	device_create(exemple_class, NULL, exemple_dev,NULL,THIS_MODULE->name);

	cdev_init(& exemple_cdev, & fops_exemple);

	err = cdev_add(& exemple_cdev, exemple_dev, 1);
	if (err != 0) {
		device_destroy(exemple_class, exemple_dev);
		class_destroy(exemple_class);
		unregister_chrdev_region(exemple_dev, 1);
		return err;
	}
	return 0; 
}


static void __exit exemple_exit (void)
{
	device_destroy(exemple_class, exemple_dev);
	class_destroy(exemple_class);
	cdev_del(& exemple_cdev);
	unregister_chrdev_region(exemple_dev, 1);
}


static int exemple_open(struct inode * ind, struct file * filp)
{
	printk(KERN_INFO "%s - %s()\n", THIS_MODULE->name, __FUNCTION__);
	return 0;
}


static int exemple_release(struct inode * ind, struct file * filp)
{
	printk(KERN_INFO "%s - %s()\n", THIS_MODULE->name, __FUNCTION__);
	return 0;
}


	module_init(exemple_init);
	module_exit(exemple_exit);

	MODULE_DESCRIPTION("Device class creation.");
	MODULE_AUTHOR("Christophe Blaess <Christophe.Blaess@Logilin.fr>");
	MODULE_LICENSE("GPL");

