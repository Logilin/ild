/************************************************************************\
  exemple_14 - Chapitre "Programmer pour le noyau Linux"

  Callback d'ecriture dans /proc

  Exemples de la formation "Programmation Noyau sous Linux"

  (c) 2005-2014 Christophe Blaess
  http://www.blaess.fr/christophe/

\************************************************************************/

#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/sched.h>
#include <linux/version.h>

#include <asm/uaccess.h>

static char * nom_entree = "exemple_14";

static int valeur_exemple = 0;



#if LINUX_VERSION_CODE < KERNEL_VERSION (3,10,0)

static int lecture (char *, char **, off_t, int, int *, void *);
static int ecriture (struct file *, const char __user *, unsigned long, void *);

static int __init exemple_14_init (void)
{
	struct proc_dir_entry * entree;
	
	entree = create_proc_entry(nom_entree, S_IFREG | 0644, NULL);
	if (entree == NULL)
		return -EBUSY;
	entree->read_proc = lecture;
	entree->write_proc = ecriture;
	return 0; 
}


static void __exit exemple_14_exit (void)
{
	remove_proc_entry(nom_entree, NULL);
}


static int lecture (char * buffer, char **debut, off_t offset,
                    int max, int * eof, void * private)
{
	snprintf(buffer, max, "PID=%u, PPID=%u, valeur=%d\n",
	         current->pid,
	         current->real_parent->pid,
	         valeur_exemple);
	return strlen(buffer);
}


static int ecriture (struct file * filp, const char __user * u_buffer,
                     unsigned long nombre, void * data)
{
	char buffer[128];
	if (nombre >= 128)
		return -ENOMEM;
	if (copy_from_user(buffer, u_buffer, nombre) != 0)
		return -EFAULT;
	if (sscanf(buffer, "%d", & valeur_exemple) != 1)
		return -EINVAL;
	return nombre;
}


module_init(exemple_14_init);
module_exit(exemple_14_exit);

MODULE_LICENSE("GPL");


#else

	static ssize_t lecture  (struct file *, char __user *, size_t, loff_t *);
	static ssize_t ecriture (struct file *, const char __user *, size_t, loff_t *);

	static const struct file_operations exemple_14_proc_fops = {
		.owner	= THIS_MODULE,
		.read   = lecture,
		.write  = ecriture,
	};


static int __init exemple_14_init (void)
{
	struct proc_dir_entry * entree;
	entree = proc_create(nom_entree, S_IFREG | 0644, NULL, & exemple_14_proc_fops);
	if (entree == NULL)
		return -EBUSY;
	return 0; 
}


static void __exit exemple_14_exit (void)
{
	remove_proc_entry(nom_entree, NULL);
}


static ssize_t lecture (struct file * filp, char __user * u_buffer, size_t max, loff_t * offset)
{
	char buffer[128];
	int  nb;

	snprintf(buffer, max, "PID=%u, PPID=%u, valeur=%d\n",
	         current->pid,
	         current->real_parent->pid,
	         valeur_exemple);

	nb = strlen(buffer);
	if ((* offset) >= nb)
		return 0;
	if (copy_to_user(u_buffer, & (buffer[*offset]), nb - (*offset)) != 0)
		return -EFAULT;
	(*offset) += nb;
	return strlen_user(u_buffer);;
}


static ssize_t ecriture (struct file * filp, const char __user * u_buffer, size_t nb, loff_t * unused)
{
	char buffer[128];
	if (nb >= 128)
		return -ENOMEM;
	if (copy_from_user(buffer, u_buffer, nb) != 0)
		return -EFAULT;
	if (sscanf(buffer, "%d", & valeur_exemple) != 1)
		return -EINVAL;
	return nb;
}

module_init(exemple_14_init);
module_exit(exemple_14_exit);
MODULE_LICENSE("GPL");

#endif

