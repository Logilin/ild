/************************************************************************\
  exemple_04 - Chapitre "Ecriture de driver - peripherique caractere"

  Inscription dans une classe de périphériques existante.

  Exemples de la formation "Programmation Noyau sous Linux"

  (c) 2005-2015 Christophe Blaess
  http://www.blaess.fr/christophe/

\************************************************************************/

	#include <linux/cdev.h>
	#include <linux/device.h>
	#include <linux/fs.h>
	#include <linux/miscdevice.h>
	#include <linux/module.h>


	static int exemple_open    (struct inode * ind, struct file * filp);
	static int exemple_release (struct inode * ind, struct file * filp);


	static struct file_operations fops_exemple = {
		.owner   =  THIS_MODULE,
		.open    =  exemple_open,
		.release =  exemple_release,
	};


	static struct miscdevice exemple_misc_driver = {
		    .minor          = MISC_DYNAMIC_MINOR,
		    .name           = THIS_MODULE->name,
		    .fops           = & fops_exemple,
	};



static int __init exemple_init (void)
{
	return misc_register(& exemple_misc_driver);
}



static void __exit exemple_exit (void)
{
	misc_deregister(& exemple_misc_driver);
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
	MODULE_LICENSE("GPL");
