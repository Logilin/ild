/************************************************************************\
  exemple_12 - Chapitre "Programmer pour le noyau Linux"

  Callback de lecture depuis /proc

  Exemples de la formation "Programmation Noyau sous Linux"

  (c) 2005-2014 Christophe Blaess
  http://www.blaess.fr/christophe/

\************************************************************************/

#include <linux/cred.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/sched.h>
#include <linux/version.h>

#if LINUX_VERSION_CODE < KERNEL_VERSION (2,6,27)
	#define uid_task(t) (t->uid)
#else
	#define uid_task(t) __kuid_val(task_uid(t))
#endif

static char * nom_entree = "exemple_12";

#if LINUX_VERSION_CODE < KERNEL_VERSION (3,10,0)

static int lecture (char *, char **, off_t, int, int *, void *);


static int __init exemple_12_init (void)
{
	struct proc_dir_entry * entree;

	entree = create_proc_entry(nom_entree, S_IFREG | 0644, NULL);
	if (entree == NULL)
		return -EBUSY;
	entree->read_proc = lecture;

	return 0; 
}


static void __exit exemple_12_exit (void)
{
	remove_proc_entry(nom_entree, NULL);
}


static int lecture (char * buffer, char **debut, off_t offset,
                    int max, int * eof, void * private)
{
	snprintf(buffer, max, "PID=%u, PPID=%u, UID=%u, Nom=%s\n",
	         current->pid, 
	         current->real_parent->pid,
	         uid_task(current), current->comm);
	return strlen(buffer);
}


module_init(exemple_12_init);
module_exit(exemple_12_exit);
MODULE_LICENSE("GPL");

#else 

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

	snprintf(buffer, max, "PID=%u, PPID=%u, UID=%u, Nom=%s\n",
	         current->pid, 
	         current->real_parent->pid,
	         uid_task(current), current->comm);

	nb = strlen(buffer);
	if (copy_to_user(u_buffer, buffer, nb) != 0)
		return -EFAULT;
	return nb;
}

module_init(exemple_12_init);
module_exit(exemple_12_exit);
MODULE_LICENSE("GPL");

#endif


