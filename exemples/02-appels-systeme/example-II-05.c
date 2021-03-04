// SPDX-License-Identifier: GPL-2.0
//
// Exemples de la formation
//  "Ecriture de drivers et programmation noyau Linux"
// Chapitre "Appels-systeme"
//
// (c) 2001-2021 Christophe Blaess
//
//    https://www.logilin.fr/
//

#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/sched.h>
#include <linux/version.h>

#include <asm/uaccess.h>
#include <linux/uaccess.h>

static ssize_t example_read(struct file *, char __user *, size_t, loff_t *);
static ssize_t example_write(struct file *, const char __user *, size_t, loff_t *);

static int example_value = 0;

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 6, 0)
	static const struct proc_ops example_ops = {
		.proc_read  = example_read,
		.proc_write = example_write,
	};
#else
	static const struct file_operations example_ops = {
		.read  = example_read,
		.write = example_write,
	};
#endif


static struct proc_dir_entry *example_entry;

static int __init example_init(void)
{
	example_entry = proc_create(THIS_MODULE->name, S_IFREG | 0644, NULL, &example_ops);
	if (example_entry == NULL)
		return -EBUSY;

	return 0;
}


static void __exit example_exit(void)
{
	proc_remove(example_entry);
}


static ssize_t example_read(struct file *filp, char __user *u_buffer, size_t max, loff_t *offset)
{
	char k_buffer[128];
	int  nb;

	snprintf(k_buffer, 128, "PID=%u, PPID=%u, Name=%s, Value=%d\n",
		 current->pid,
		 current->real_parent->pid,
		 current->comm,
		 example_value);

	nb = strlen(k_buffer) - (*offset);
	if (nb <= 0)
		return 0;
	if (nb > max)
		nb = max;
	if (copy_to_user(u_buffer, &(k_buffer[*offset]), nb) != 0)
		return -EFAULT;
	(*offset) += nb;

	return nb;
}


static ssize_t example_write(struct file * filp, const char __user * u_buffer, size_t nb, loff_t * unused)
{
	char k_buffer[128];

	if (nb >= 128)
		return -ENOMEM;
	if (copy_from_user(k_buffer, u_buffer, nb) != 0)
		return -EFAULT;
	if (sscanf(k_buffer, "%d", &example_value) != 1)
		return -EINVAL;

	return nb;
}


module_init(example_init);
module_exit(example_exit);

MODULE_DESCRIPTION("/proc write callback.");
MODULE_AUTHOR("Christophe Blaess <Christophe.Blaess@Logilin.fr>");
MODULE_LICENSE("GPL v2");
