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
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/uaccess.h>
#include <linux/version.h>

#include <asm/uaccess.h>

#include "example-IV-06.h"


static int example_ppid_flag = 1;


static ssize_t example_read(struct file *filp, char *u_buffer, size_t length, loff_t *offset)
{
	char k_buffer[128];

	if (example_ppid_flag)
		snprintf(k_buffer, 128, "PID= %u, PPID= %u\n",
			current->pid,
			current->real_parent->pid);
	else
		snprintf(k_buffer, 128, "PID= %u\n", current->pid);

	return simple_read_from_buffer(u_buffer, length, offset, k_buffer, strlen(k_buffer));
}


static long example_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	if (_IOC_TYPE(cmd) != EXAMPLE_IOCTL_MAGIC)
		return -ENOTTY;

	switch (_IOC_NR(cmd)) {
	case EXAMPLE_INTERNAL_GET_PPID_FLAG:
		if (copy_to_user((void *) arg, &example_ppid_flag, sizeof(example_ppid_flag)) != 0)
			return -EFAULT;
		break;
	case EXAMPLE_INTERNAL_SET_PPID_FLAG:
		if (copy_from_user(&example_ppid_flag, (void *) arg, sizeof(example_ppid_flag)) != 0)
			return -EFAULT;
		break;
	default:
		return -ENOTTY;
	}
	return 0;
}


static const struct file_operations example_fops = {
	.owner   =  THIS_MODULE,
	.read    =  example_read,
	.unlocked_ioctl   =  example_ioctl,
};


static struct miscdevice example_misc_driver = {
	.minor          = MISC_DYNAMIC_MINOR,
	.name           = THIS_MODULE->name,
	.fops           = &example_fops,
	.mode           = 0666,
};


	module_misc_device(example_misc_driver);

MODULE_DESCRIPTION("ioctl() system call implementation.");
MODULE_AUTHOR("Christophe Blaess <Christophe.Blaess@Logilin.fr>");
MODULE_LICENSE("GPL v2");

