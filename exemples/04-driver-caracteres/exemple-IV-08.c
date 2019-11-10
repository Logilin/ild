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
	#include <linux/mutex.h>
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

	static volatile int current_pid = 0;
	DEFINE_MUTEX(mtx_current_pid);


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

	if (mutex_lock_interruptible(& mtx_current_pid) != 0)
		return -ERESTARTSYS;

	current_pid = current->pid;

	delay = jiffies + 10;
	while (time_before(jiffies, delay))
		schedule();

	if (current_pid == current->pid)
		strcpy(k_buffer, ".");
	else
		strcpy(k_buffer, "#");

	mutex_unlock(& mtx_current_pid);

	if (length < 2)
		return -ENOMEM;
	if (copy_to_user(buffer, k_buffer, 2) != 0)
		return -EFAULT;

	return 1;
}


	module_init(example_init);
	module_exit(example_exit);

	MODULE_DESCRIPTION("Use of a mutex for shared variable protection.");
	MODULE_AUTHOR("Christophe Blaess <Christophe.Blaess@Logilin.fr>");
	MODULE_LICENSE("GPL");

