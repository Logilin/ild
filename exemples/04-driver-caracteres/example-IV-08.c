// SPDX-License-Identifier: GPL-2.0
//
// Exemples de la formation
//  "Ecriture de drivers et programmation noyau Linux"
// Chapitre "Driver en mode caracteres"
//
// (c) 2001-2025 Christophe Blaess
//
//    https://www.logilin.fr/
//

#include <linux/cdev.h>
#include <linux/delay.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/sched.h>
#include <linux/uaccess.h>
#include <linux/version.h>

#include <asm/uaccess.h>


static volatile int current_pid;
DEFINE_MUTEX(mtx_current_pid);


static ssize_t example_read(struct file *filp, char *u_buffer, size_t length, loff_t *offset)
{
	char k_buffer[2];
	unsigned long delay;

	if (mutex_lock_interruptible(&mtx_current_pid) != 0)
		return -ERESTARTSYS;

	current_pid = current->pid;

	delay = jiffies + 10;
	while (time_before(jiffies, delay))
		schedule();

	if (current_pid == current->pid)
		strcpy(k_buffer, ".");
	else
		strcpy(k_buffer, "#");

	mutex_unlock(&mtx_current_pid);

	if (length < 2)
		return -ENOMEM;
	if (copy_to_user(u_buffer, k_buffer, 2) != 0)
		return -EFAULT;

	return 1;
}


static const struct file_operations example_fops = {
	.owner   =  THIS_MODULE,
	.read    =  example_read,
};


static struct miscdevice example_misc_driver = {
	.minor          = MISC_DYNAMIC_MINOR,
	.name           = THIS_MODULE->name,
	.fops           = &example_fops,
	.mode           = 0666,
};


	module_misc_device(example_misc_driver);

MODULE_DESCRIPTION("Use of a mutex for shared variable protection.");
MODULE_AUTHOR("Christophe Blaess <Christophe.Blaess@Logilin.fr>");
MODULE_LICENSE("GPL v2");
