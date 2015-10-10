/************************************************************************\
  exemple_06 - Chapitre "Ecriture de driver - peripherique caractere"

  Fonction d'ioctl

  Exemples de la formation "Programmation Noyau sous Linux"

  (c) 2005-2015 Christophe Blaess
  http://www.blaess.fr/christophe/

\************************************************************************/

	#include <linux/cdev.h>
	#include <linux/device.h>
	#include <linux/fs.h>
	#include <linux/miscdevice.h>
	#include <linux/module.h>
	#include <linux/sched.h>

	#include <asm/uaccess.h>

	#include "exemple_06.h"


	static ssize_t exemple_read  (struct file * filp, char * buffer,
	                              size_t length, loff_t * offset);

	static long    exemple_ioctl (struct file * filp,
	                              unsigned int cmd, unsigned long arg);


	static struct file_operations fops_exemple = {
		.owner   =  THIS_MODULE,
		.read    =  exemple_read,
		.unlocked_ioctl   =  exemple_ioctl,
	};


	static struct miscdevice exemple_misc_driver = {
		    .minor          = MISC_DYNAMIC_MINOR,
		    .name           = THIS_MODULE->name,
		    .fops           = & fops_exemple,
	};


	static int exemple_ppid_flag = 1;


static int __init exemple_init (void)
{
	return misc_register(& exemple_misc_driver);
}



static void __exit exemple_exit (void)
{
	misc_deregister(& exemple_misc_driver);
}



static ssize_t exemple_read(struct file * filp, char * buffer,
                            size_t length, loff_t * offset)
{
	char chaine[128];
	int l;

	if (exemple_ppid_flag) 
		snprintf(chaine, 128, "PID= %u, PPID= %u\n",
		                current->pid,
	                        current->real_parent->pid);
	else
		snprintf(chaine, 128, "PID= %u\n", current->pid);

	l = strlen(chaine) - (*offset);
	if (l <= 0)
		return 0;

	if (length < l)
		l = length;

	if (copy_to_user(buffer, & chaine[* offset], l) != 0)
		return -EFAULT;

	*offset += l;

	return l;
}



static long exemple_ioctl (struct file * filp,
                           unsigned int cmd,
                           unsigned long arg)
{
	if (_IOC_TYPE(cmd) != EXEMPLE_IOCTL_MAGIC)
		return -ENOTTY;

	switch(_IOC_NR(cmd)) {
		case EXEMPLE_GET_PPID_FLAG :
			if (copy_to_user((void *) arg, & exemple_ppid_flag, sizeof(exemple_ppid_flag)) != 0)
				return -EFAULT;
			break;
		case EXEMPLE_SET_PPID_FLAG :
			if (copy_from_user(& exemple_ppid_flag, (void *) arg, sizeof(exemple_ppid_flag)) != 0)
				return -EFAULT;
			break;
		default :
			return -ENOTTY; 
	}
	return 0;
}


	module_init(exemple_init);
	module_exit(exemple_exit);
	MODULE_LICENSE("GPL");

