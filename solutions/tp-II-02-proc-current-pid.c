// SPDX-License-Identifier: GPL-2.0
//
// Exemples de la formation
//  "Ecriture de drivers et programmation noyau Linux"
//
// (c) 2001-2025 Christophe Blaess
//
//    https://www.logilin.fr/
//

#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/sched.h>
#include <linux/version.h>
#include <linux/uaccess.h>
#include <asm/uaccess.h>



static ssize_t pid_read(struct file *filp, char __user *u_buffer, size_t max, loff_t *offset)
{
	char string[32];
	int  nb;

	snprintf(string, 32, "%u\n", current->pid);
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


	static const struct file_operations pid_fops = {
		.owner	= THIS_MODULE,
		.read   = pid_read,
	};


	static struct proc_dir_entry *current_entry;


static int __init current_pid_init(void)
{
	current_entry = proc_mkdir("current", NULL);
	if (current_entry == NULL)
		return -EBUSY;

	if (proc_create("pid", 0644, current_entry, &pid_fops) == NULL) {
		remove_proc_entry("current", NULL);
		return -EBUSY;
	}

	return 0;
}


static void __exit current_pid_exit(void)
{
	remove_proc_entry("pid", current_entry);
	remove_proc_entry("current", NULL);
}


module_init(current_pid_init);
module_exit(current_pid_exit);

MODULE_DESCRIPTION("Exercise II-02 solution.");
MODULE_AUTHOR("Christophe Blaess <Christophe.Blaess@Logilin.fr>");
MODULE_LICENSE("GPL v2");
