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
#include <linux/delay.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/uaccess.h>
#include <linux/version.h>

#include <asm/uaccess.h>


	static ssize_t example_read(struct file *filp, char *buffer, size_t length, loff_t *offset);

	static const struct file_operations fops_example = {
		.owner   =  THIS_MODULE,
		.read    =  example_read,
	};

	static struct miscdevice example_misc_driver = {
		.minor          = MISC_DYNAMIC_MINOR,
		.name           = THIS_MODULE->name,
		.fops           = &fops_example,
		.mode           = 0444,
	};

	static volatile int current_pid;


static ssize_t example_read(struct file *filp, char *u_buffer, size_t length, loff_t *offset)
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
	if (copy_to_user(u_buffer, k_buffer, 2) != 0)
		return -EFAULT;
	return 1;
}


#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 9, 0)
	module_misc_device(example_misc_driver);
#else
	module_driver(example_misc_driver, misc_register, misc_deregister)
#endif

MODULE_DESCRIPTION("Unprotected access on a shared variable.");
MODULE_AUTHOR("Christophe Blaess <Christophe.Blaess@Logilin.fr>");
MODULE_LICENSE("GPL v2");

