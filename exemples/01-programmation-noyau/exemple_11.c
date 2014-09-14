/************************************************************************\
  exemple_11 - Chapitre "Programmer pour le noyau Linux"

  Creation et suppression d'une entree dans /proc

  Exemples de la formation "Programmation Noyau sous Linux"

  (c) 2005-2014 Christophe Blaess
  http://www.blaess.fr/christophe/

\************************************************************************/

#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/version.h>

static char * nom_entree = "exemple_11";

#if LINUX_VERSION_CODE >= KERNEL_VERSION (3,10,0)
	static const struct file_operations exemple_11_proc_fops = {
		.owner	= THIS_MODULE,
	};
#endif

static int __init exemple_11_init (void)
{
	struct proc_dir_entry * entree;
	

#if LINUX_VERSION_CODE < KERNEL_VERSION (3,10,0)
	entree = create_proc_entry(nom_entree, S_IFREG | 0644, NULL);
#else
	entree = proc_create(nom_entree, S_IFREG | 0644, NULL, & exemple_11_proc_fops);
#endif
	if (entree == NULL)
		return -EBUSY;
	return 0; 
}

static void __exit exemple_11_exit (void)
{
	remove_proc_entry(nom_entree, NULL);
}

module_init(exemple_11_init);
module_exit(exemple_11_exit);

MODULE_LICENSE("GPL");

