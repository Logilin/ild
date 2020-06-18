/************************************************************************\
  Exemples de la formation
    "Ecriture de drivers et programmation noyau Linux"
  Chapitre "Driver en mode caracteres"

  (c) 2005-2019 Christophe Blaess
  http://www.blaess.fr/christophe/

\************************************************************************/

	#include <linux/cdev.h>
	#include <linux/device.h>
	#include <linux/fs.h>
	#include <linux/miscdevice.h>
	#include <linux/module.h>


	static int example_open    (struct inode * ind, struct file * filp);
	static int example_release (struct inode * ind, struct file * filp);


	static struct file_operations fops_example = {
		.owner   =  THIS_MODULE,
		.open    =  example_open,
		.release =  example_release,
	};

	static struct miscdevice example_misc_driver = {
		    .minor          = MISC_DYNAMIC_MINOR,
		    .name           = THIS_MODULE->name,
		    .fops           = & fops_example,
	};


static int __init example_init (void)
{
	return misc_register(& example_misc_driver);
}


static void __exit example_exit (void)
{
	misc_deregister(& example_misc_driver);
}


static int example_open(struct inode * ind, struct file * filp)
{
	printk(KERN_INFO "%s - %s()\n", THIS_MODULE->name, __FUNCTION__);
	return 0;
}


static int example_release(struct inode * ind, struct file * filp)
{
	printk(KERN_INFO "%s - %s()\n", THIS_MODULE->name, __FUNCTION__);
	return 0;
}


	module_init(example_init);
	module_exit(example_exit);

	MODULE_DESCRIPTION("Registration into an existing class.");
	MODULE_AUTHOR("Christophe Blaess <Christophe.Blaess@Logilin.fr>");
	MODULE_LICENSE("GPL");

