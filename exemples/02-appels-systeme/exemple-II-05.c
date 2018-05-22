/************************************************************************\
  Exemples de la formation
    "Ecriture de drivers et programmation noyau Linux"
  Chapitre "Appels-systeme"

  (c) 2005-2017 Christophe Blaess
  http://www.blaess.fr/christophe/

\************************************************************************/

	#include <linux/module.h>
	#include <linux/proc_fs.h>
	#include <linux/sched.h>
	#include <linux/version.h>

	#include <asm/uaccess.h>
	#include <linux/uaccess.h>


	static ssize_t exemple_read  (struct file *, char __user *, size_t, loff_t *);
	static ssize_t exemple_write (struct file *, const char __user *, size_t, loff_t *);

	static int exemple_value = 0;

	static const struct file_operations exemple_proc_fops = {
		.read   = exemple_read,
		.write  = exemple_write,
	};


static int __init exemple_init (void)
{
	if (proc_create(THIS_MODULE->name, S_IFREG | 0644, NULL, & exemple_proc_fops) == NULL)
		return -EBUSY;

	return 0;
}


static void __exit exemple_exit (void)
{
	remove_proc_entry(THIS_MODULE->name, NULL);
}


static ssize_t exemple_read(struct file * filp, char __user * u_buffer, size_t max, loff_t * offset)
{
	char buffer[128];
	int  nb;

	snprintf(buffer, 128, "PID=%u, PPID=%u, Name=%s, Value=%d\n",
	         current->pid, 
	         current->real_parent->pid,
	         current->comm,
	         exemple_value);

	nb = strlen(buffer) - (*offset);
	if (nb <= 0)
		return 0;
	if (nb > max)
		nb = max;
	if (copy_to_user(u_buffer, & (buffer[*offset]), nb) != 0)
		return -EFAULT;
	(*offset) += nb;

	return nb;
}


static ssize_t exemple_write(struct file * filp, const char __user * u_buffer, size_t nb, loff_t * unused)
{
	char buffer[128];

	if (nb >= 128)
		return -ENOMEM;
	if (copy_from_user(buffer, u_buffer, nb) != 0)
		return -EFAULT;
	if (sscanf(buffer, "%d", &exemple_value) != 1)
		return -EINVAL;

	return nb;
}


	module_init(exemple_init);
	module_exit(exemple_exit);

	MODULE_DESCRIPTION("/proc write callback.");
	MODULE_AUTHOR("Christophe Blaess <Christophe.Blaess@Logilin.fr>");
	MODULE_LICENSE("GPL");

