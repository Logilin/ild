/************************************************************************\
  Exemples de la formation
    "Ecriture de drivers et programmation noyau Linux"
  Chapitre "Appels-systeme"

  (c) 2005-2017 Christophe Blaess
  http://www.blaess.fr/christophe/

\************************************************************************/

	#include <linux/module.h>
	#include <linux/sched.h>
	#include <linux/version.h>


static int __init exemple_init (void)
{
	printk(KERN_INFO "%s - %s(): PID = %u  PPID = %u\n",
	          THIS_MODULE->name, __FUNCTION__,
	          current->pid,
	          current->real_parent->pid);
	return 0;
}


static void __exit exemple_exit (void)
{
	printk(KERN_INFO "%s - %s(): PID = %u, PPID = %u\n",
	          THIS_MODULE->name, __FUNCTION__,
	          current->pid,
	          current->real_parent->pid);
}


	module_init(exemple_init);
	module_exit(exemple_exit);

	MODULE_DESCRIPTION("Informations about current process.");
	MODULE_AUTHOR("Christophe Blaess <Christophe.Blaess@Logilin.fr>");
	MODULE_LICENSE("GPL");

