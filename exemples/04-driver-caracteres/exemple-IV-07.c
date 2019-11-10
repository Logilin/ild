/************************************************************************\
  Exemples de la formation
    "Ecriture de drivers et programmation noyau Linux"
  Chapitre "Driver en mode caracteres"

  (c) 2005-2019 Christophe Blaess
  http://www.blaess.fr/christophe/

\************************************************************************/

	#include <linux/cdev.h>
	#include <linux/delay.h>
	#include <linux/device.h>
	#include <linux/fs.h>
	#include <linux/miscdevice.h>
	#include <linux/module.h>
	#include <linux/sched.h>
	#include <linux/uaccess.h>

	#include <asm/uaccess.h>


	static ssize_t example_read  (struct file * filp, char * buffer,
	                              size_t length, loff_t * offset);

	static struct file_operations fops_example = {
		.owner   =  THIS_MODULE,
		.read    =  example_read,
	};

	static struct miscdevice example_misc_driver = {
		    .minor          = MISC_DYNAMIC_MINOR,
		    .name           = THIS_MODULE->name,
		    .fops           = & fops_example,
	};

	static volatile int current_pid;


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
	char k_buffer[2];
	unsigned long delay;

	current_pid = current->pid;

	// 10 ticks loop to force collision between simultaneous system calls.
	delay = jiffies + 10;
	while (time_before(jiffies, delay))
		schedule(); // On a preemptible system, cpu_relax() works as well.

	if (current_pid == current->pid)
		strcpy(k_buffer, ".");
	else
		strcpy(k_buffer, "#");

	if (length < 2)
		return -ENOMEM;
	if (copy_to_user(buffer, k_buffer, 2) != 0)
		return -EFAULT;
	return 1;
}


	module_init(example_init);
	module_exit(example_exit);

	MODULE_DESCRIPTION("Unprotected access on a shared variable.");
	MODULE_AUTHOR("Christophe Blaess <Christophe.Blaess@Logilin.fr>");
	MODULE_LICENSE("GPL");

