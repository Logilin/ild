// SPDX-License-Identifier: GPL-2.0
//
// Exemples de la formation
//  "Ecriture de drivers et programmation noyau Linux"
// Chapitre "Driver en mode caracteres"
//
// (c) 2001-2022 Christophe Blaess
//
//    https://www.logilin.fr/
//

#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/uaccess.h>
#include <linux/version.h>

#include <asm/uaccess.h>


static ssize_t example_read(struct file *filp, char *u_buffer, size_t length, loff_t *offset)
{
	char k_buffer[128];
	int lg;

	snprintf(k_buffer, 128, "PID=%u, PPID=%u\n",
		current->pid,
		current->real_parent->pid);

	lg = strlen(k_buffer) - (*offset);

	if (lg <= 0)
		return 0;

	if (length < lg)
		lg = length;

	if (copy_to_user(u_buffer, &k_buffer[*offset], lg) != 0)
		return -EFAULT;

	*offset += lg;

	return lg;
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


#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 9, 0)
	module_misc_device(example_misc_driver);
#else
	module_driver(example_misc_driver, misc_register, misc_deregister)
#endif

MODULE_DESCRIPTION("read() system call implementation.");
MODULE_AUTHOR("Christophe Blaess <Christophe.Blaess@Logilin.fr>");
MODULE_LICENSE("GPL v2");
