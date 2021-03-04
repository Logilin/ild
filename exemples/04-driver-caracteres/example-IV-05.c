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
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/module.h>
#include <linux/sched.h>

#include <asm/uaccess.h>
#include <linux/uaccess.h>


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


static int __init example_init(void)
{
	return misc_register(&example_misc_driver);
}


static void __exit example_exit(void)
{
	misc_deregister(&example_misc_driver);
}


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


module_init(example_init);
module_exit(example_exit);

MODULE_DESCRIPTION("read() system call implementation.");
MODULE_AUTHOR("Christophe Blaess <Christophe.Blaess@Logilin.fr>");
MODULE_LICENSE("GPL v2");

