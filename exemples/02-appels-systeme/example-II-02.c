/************************************************************************\
  Exemples de la formation
    "Ecriture de drivers et programmation noyau Linux"
  Chapitre "Appels-systeme"

  (c) 2005-2019 Christophe Blaess
  http://www.blaess.fr/christophe/

\************************************************************************/

	#include <linux/module.h>
	#include <linux/proc_fs.h>
	#include <linux/version.h>


	static const struct file_operations example_fops = {
	};


static int __init example_init (void)
{
	struct proc_dir_entry * entry;

	entry = proc_create(THIS_MODULE->name, S_IFREG | 0644, NULL, & example_fops);
	if (entry == NULL)
		return -EBUSY;

	return 0;
}


static void __exit example_exit (void)
{
	remove_proc_entry(THIS_MODULE->name, NULL);
}


	module_init(example_init);
	module_exit(example_exit);

	MODULE_DESCRIPTION("/proc entry creation.");
	MODULE_AUTHOR("Christophe Blaess <Christophe.Blaess@Logilin.fr>");
	MODULE_LICENSE("GPL");

