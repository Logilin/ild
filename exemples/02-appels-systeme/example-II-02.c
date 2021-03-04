// SPDX-License-Identifier: GPL-2.0
//
// Exemples de la formation
//  "Ecriture de drivers et programmation noyau Linux"
// Chapitre "Appels-systeme"
//
// (c) 2001-2021 Christophe Blaess
//
//    https://www.logilin.fr/
//

#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/version.h>


#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 6, 0)
	static const struct proc_ops example_ops = {
	};
#else
	static const struct file_operations example_ops = {
	};
#endif


static struct proc_dir_entry *example_entry;


static int __init example_init(void)
{
	example_entry = proc_create(THIS_MODULE->name, S_IFREG | 0644, NULL, &example_ops);
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

MODULE_DESCRIPTION("/proc entry creation.");
MODULE_AUTHOR("Christophe Blaess <Christophe.Blaess@Logilin.fr>");
MODULE_LICENSE("GPL v2");

