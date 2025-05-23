// SPDX-License-Identifier: GPL-2.0
//
// Exemples de la formation
//  "Ecriture de drivers et programmation noyau Linux"
// Chapitre "Appels-systeme"
//
// (c) 2001-2025 Christophe Blaess
//
//    https://www.logilin.fr/
//

#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/sched.h>
#include <linux/version.h>

#include <asm/uaccess.h>
#include <linux/uaccess.h>


static ssize_t example_read(struct file *filp, char __user *u_buffer, size_t max, loff_t *offset)
{
	char k_buffer[128];
	int  nb;

	pr_info("%s - %s(%016lx, %016lx, %u, %lld)\n", THIS_MODULE->name, __func__, (unsigned long)filp, (unsigned long)u_buffer, (unsigned int) max, *offset);

	snprintf(k_buffer, 128, "PID=%u, PPID=%u, Command=%s\n",
		current->pid,
		current->real_parent->pid,
		current->comm);

	nb = strlen(k_buffer);

	return simple_read_from_buffer(u_buffer, max, offset, k_buffer, nb);
}


#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 6, 0)
	static const struct proc_ops example_ops = {
		.proc_read = example_read,
		.proc_lseek = default_llseek,
	};
#else
	static const struct file_operations example_ops = {
		.read = example_read,
	};
#endif


static struct proc_dir_entry *example_entry;


static int __init example_init(void)
{
	example_entry = proc_create(THIS_MODULE->name, 0666, NULL, &example_ops);
	if (example_entry == NULL)
		return -EBUSY;

	return 0;
}


static void __exit example_exit(void)
{
	proc_remove(example_entry);
}


module_init(example_init);
module_exit(example_exit);

MODULE_DESCRIPTION("/proc entry read callback improvement.");
MODULE_AUTHOR("Christophe Blaess <Christophe.Blaess@Logilin.fr>");
MODULE_LICENSE("GPL v2");
