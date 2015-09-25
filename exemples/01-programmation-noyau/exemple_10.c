/************************************************************************\
  exemple_10 - Chapitre "Programmer pour le noyau Linux"

  Creation et suppression d'une entree dans /proc

  Exemples de la formation "Programmation Noyau sous Linux"

  (c) 2005-2014 Christophe Blaess
  http://www.blaess.fr/christophe/

\************************************************************************/

#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/version.h>

static char * nom_entree = "exemple_11";

	static const struct file_operations exemple_11_proc_fops = {
		.owner	= THIS_MODULE,
	};

static int __init exemple_11_init (void)
{
	struct proc_dir_entry * entree;
	
	entree = proc_create(nom_entree, S_IFREG | 0644, NULL, & exemple_11_proc_fops);

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

