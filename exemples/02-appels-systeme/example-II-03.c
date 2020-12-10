/************************************************************************\
  Exemples de la formation
    "Ecriture de drivers et programmation noyau Linux"
  Chapitre "Appels-systeme"

  (c) 2005-2019 Christophe Blaess
  http://www.blaess.fr/christophe/

\************************************************************************/

	#include <linux/module.h>
	#include <linux/proc_fs.h>
	#include <linux/sched.h>
	#include <linux/version.h>

	#include <asm/uaccess.h>
	#include <linux/uaccess.h>


	static ssize_t example_read (struct file *, char __user *, size_t, loff_t *);

	static const struct file_operations example_fops = {
		.read   = example_read,
	};

	static struct proc_dir_entry * example_entry;

static int __init example_init (void)
{
	example_entry = proc_create(THIS_MODULE->name, S_IFREG | 0644, NULL, & example_fops);
	if (example_entry == NULL)
		return -EBUSY;

	return 0;
}


static void __exit example_exit (void)
{
	proc_remove(example_entry);
}


static ssize_t example_read(struct file * filp, char __user * u_buffer, size_t max, loff_t * offset)
{
	char k_buffer[128];
	int  nb;

	snprintf(k_buffer, 128, "PID=%u, PPID=%u, Name=%s\n",
	         current->pid,
	         current->real_parent->pid,
	         current->comm);

	nb = strlen(k_buffer);
	if (nb > max)
		return -ENOMEM;
	if (copy_to_user(u_buffer, k_buffer, nb) != 0)
		return -EFAULT;
	return nb;
}

	module_init(example_init);
	module_exit(example_exit);

	MODULE_DESCRIPTION("/proc read callback function.");
	MODULE_AUTHOR("Christophe Blaess <Christophe.Blaess@Logilin.fr>");
	MODULE_LICENSE("GPL");
