// SPDX-License-Identifier: GPL-2.0
//
// Exemples de la formation
//  "Ecriture de drivers et programmation noyau Linux"
//
// (c) 2001-2023 Christophe Blaess
//
//    https://www.logilin.fr/
//

#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/sched.h>
#include <linux/version.h>
#include <linux/uaccess.h>
#include <asm/uaccess.h>

int _Sum = 0;

static ssize_t sum_read(struct file *filp, char __user *u_buffer, size_t max, loff_t *offset)
{
	char string[32];
	int  nb;

	snprintf(string, 32, "%d\n", _Sum);
	nb = strlen(string) - (*offset);
	if (nb <= 0)
		return 0;
	if (nb > max)
		nb = max;
	if (copy_to_user(u_buffer, &(string[*offset]), nb) != 0)
		return -EFAULT;
	(*offset) += nb;

	return nb;
}



static ssize_t sum_write(struct file * filp, const char __user * u_buffer, size_t nb, loff_t * unused)
{
	char k_buffer[128];
	int value;

	if (nb >= 128)
		return -ENOMEM;
	if (copy_from_user(k_buffer, u_buffer, nb) != 0)
		return -EFAULT;
	k_buffer[nb] = '\0';

	if (sscanf(k_buffer, "%d", &value) != 1)
		return -EINVAL;

	_Sum += value;

	return nb;
}


#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 6, 0)
	static const struct proc_ops sum_fops = {
		.owner	= THIS_MODULE,
		.proc_read   = sum_read,
		.proc_write  = sum_write,
	};
#else
	static const struct file_operations sum_fops = {
		.owner	= THIS_MODULE,
		.read   = sum_read,
		.write  = sum_write,
	};
#endif



static int __init sum_init(void)
{
	if (proc_create("sum", 0666, NULL, &sum_fops) == NULL)
		return -EBUSY;

	return 0;
}


static void __exit sum_exit(void)
{
	remove_proc_entry("sum", NULL);
}


module_init(sum_init);
module_exit(sum_exit);

MODULE_DESCRIPTION("Exercise II-03 solution.");
MODULE_AUTHOR("Christophe Blaess <Christophe.Blaess@Logilin.fr>");
MODULE_LICENSE("GPL v2");
