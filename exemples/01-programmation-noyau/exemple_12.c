/************************************************************************\
  Exemples de la formation
    "Ecriture de drivers et programmation noyau Linux"
  Chapitre "Programmer pour le noyau Linux"

  (c) 2005-2015 Christophe Blaess
  http://www.blaess.fr/christophe/

\************************************************************************/

	#include <linux/module.h>
	#include <linux/proc_fs.h>
	#include <linux/sched.h>
	#include <linux/version.h>

	#include <asm/uaccess.h>


	static ssize_t exemple_read (struct file *, char __user *, size_t, loff_t *);

	static const struct file_operations exemple_fops = {
		.read   = exemple_read,
	};


static int __init exemple_init (void)
{
	struct proc_dir_entry * entry;

	entry = proc_create(THIS_MODULE->name, S_IFREG | 0644, NULL, & exemple_fops);
	if (entry == NULL)
		return -EBUSY;

	return 0; 
}


static void __exit exemple_exit (void)
{
	remove_proc_entry(THIS_MODULE->name, NULL);
}


static ssize_t exemple_read (struct file * filp, char __user * u_buffer, size_t max, loff_t * offset)
{
	char buffer[128];
	int  nb;

	printk(KERN_INFO "%s - %s(%p, %p, %u, %lld)", THIS_MODULE->name, __FUNCTION__, filp, u_buffer, (unsigned int) max, *offset);

	snprintf(buffer, 128, "PID=%u, PPID=%u, Name=%s\n",
	         current->pid, 
	         current->real_parent->pid,
	         current->comm);

	nb = strlen(buffer) - (*offset);
	if (nb <= 0) {
		printk(" -> 0\n");
		return 0;
	}

	if (nb > max)
		nb = max;

	if (copy_to_user(u_buffer, & (buffer[*offset]), nb) != 0) {
		printk(" -> error\n");
		return -EFAULT;
	}
	(*offset) += nb;
	printk(" -> %d\n", nb);
	return nb;
}


	module_init(exemple_init);
	module_exit(exemple_exit);

	MODULE_DESCRIPTION("/proc entry read callback improvement.");
	MODULE_AUTHOR("Christophe Blaess <Christophe.Blaess@Logilin.fr>");
	MODULE_LICENSE("GPL");
