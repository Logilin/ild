/************************************************************************\
  exemple_05 - Chapitre "Ecriture de driver - peripherique caractere"

  Fonction de lecture.

  Exemples de la formation "Programmation Noyau sous Linux"

  (c) 2005-2014 Christophe Blaess
  http://www.blaess.fr/christophe/

\************************************************************************/

#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/module.h>
#include <linux/sched.h>

#include <asm/uaccess.h>


	static ssize_t exemple_read  (struct file * filp, char * buffer,
	                              size_t length, loff_t * offset);

	static struct file_operations fops_exemple = {
		.owner   =  THIS_MODULE,
		.read    =  exemple_read,
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


static ssize_t exemple_read(struct file * filp, char * buffer,
                            size_t length, loff_t * offset)
{
	char chaine[128];
	int lg;

	printk(KERN_INFO "%s - exemple_read(.., %lu, %lld)...",
	                 THIS_MODULE->name, length, *offset);

	snprintf(chaine, 128, "PID=%u, PPID=%u\n",
	                current->pid,
	                current->real_parent->pid);

	lg = strlen(chaine);
	lg -= *offset;
	if (lg <= 0) {
		printk("-> 0\n");
		return 0;
	}
	if (length < lg)
		lg = length;

	if (copy_to_user(buffer, & chaine[* offset], lg) != 0) {
		printk("-> Error\n");
		return -EFAULT;
	}
	*offset += lg;
	printk("-> %d\n", lg);
	return lg;
}

module_init(exemple_init);
module_exit(exemple_exit);
MODULE_LICENSE("GPL");

