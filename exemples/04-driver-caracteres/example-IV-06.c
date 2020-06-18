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
	#include <linux/sched.h>

	#include <asm/uaccess.h>
	#include <linux/uaccess.h>

	#include "example-IV-06.h"


	static ssize_t example_read  (struct file * filp, char * buffer,
	                              size_t length, loff_t * offset);

	static long    example_ioctl (struct file * filp,
	                              unsigned int cmd, unsigned long arg);

	static struct file_operations fops_example = {
		.owner   =  THIS_MODULE,
		.read    =  example_read,
		.unlocked_ioctl   =  example_ioctl,
	};

	static struct miscdevice example_misc_driver = {
		    .minor          = MISC_DYNAMIC_MINOR,
		    .name           = THIS_MODULE->name,
		    .fops           = & fops_example,
		    .mode           = S_IRUGO,
	};

	static int example_ppid_flag = 1;


static int __init example_init (void)
{
	return misc_register(& example_misc_driver);
}


static void __exit example_exit (void)
{
	misc_deregister(& example_misc_driver);
}


static ssize_t example_read(struct file * filp, char * buffer,
                            size_t length, loff_t * offset)
{
	char chaine[128];
	int l;

	if (example_ppid_flag) 
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


static long example_ioctl (struct file * filp,
                           unsigned int cmd,
                           unsigned long arg)
{
	if (_IOC_TYPE(cmd) != EXAMPLE_IOCTL_MAGIC)
		return -ENOTTY;

	switch(_IOC_NR(cmd)) {
		case EXAMPLE_GET_PPID_FLAG :
			if (copy_to_user((void *) arg, & example_ppid_flag, sizeof(example_ppid_flag)) != 0)
				return -EFAULT;
			break;
		case EXAMPLE_SET_PPID_FLAG :
			if (copy_from_user(& example_ppid_flag, (void *) arg, sizeof(example_ppid_flag)) != 0)
				return -EFAULT;
			break;
		default :
			return -ENOTTY; 
	}
	return 0;
}


	module_init(example_init);
	module_exit(example_exit);

	MODULE_DESCRIPTION("ioctl() system call implementation.");
	MODULE_AUTHOR("Christophe Blaess <Christophe.Blaess@Logilin.fr>");
	MODULE_LICENSE("GPL");

