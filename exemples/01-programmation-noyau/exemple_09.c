/************************************************************************\
  exemple_09 - Chapitre "Programmer pour le noyau Linux"

  Affichage au chargement et dechargement des PID et PPID de l'appelant

  Exemples de la formation "Programmation Noyau sous Linux"

  (c) 2005-2015 Christophe Blaess
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

	MODULE_LICENSE("GPL");
