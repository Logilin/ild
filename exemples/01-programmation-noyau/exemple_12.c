/************************************************************************\
  exemple_12 - Chapitre "Programmer pour le noyau Linux"

  Callback de lecture depuis /proc

  Exemples de la formation "Programmation Noyau sous Linux"

  (c) 2005-2014 Christophe Blaess
  http://www.blaess.fr/christophe/

\************************************************************************/

#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/sched.h>
#include <linux/version.h>
#include <asm/uaccess.h>

static char * nom_entree = "exemple_12";

static ssize_t lecture (struct file *, char __user *, size_t, loff_t *);

static const struct file_operations exemple_12_proc_fops = {
	.owner	= THIS_MODULE,
	.read   = lecture,
};

static int __init exemple_12_init (void)
{
	struct proc_dir_entry * entree;
	entree = proc_create(nom_entree, S_IFREG | 0644, NULL, & exemple_12_proc_fops);
	if (entree == NULL)
		return -EBUSY;
	return 0; 
}


static void __exit exemple_12_exit (void)
{
	remove_proc_entry(nom_entree, NULL);
}

static ssize_t lecture (struct file * filp, char __user * u_buffer, size_t max, loff_t * offset)
{
	char buffer[128];
	int  nb;

	snprintf(buffer, 128, "PID=%u, PPID=%u, Nom=%s\n",
	         current->pid, 
	         current->real_parent->pid,
	         current->comm);

	nb = strlen(buffer);
	if (nb > max)
		return -ENOMEM;
	if (copy_to_user(u_buffer, buffer, nb) != 0)
		return -EFAULT;
	return nb;
}

module_init(exemple_12_init);
module_exit(exemple_12_exit);
MODULE_LICENSE("GPL");



