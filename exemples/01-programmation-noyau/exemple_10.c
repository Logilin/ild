/************************************************************************\
  exemple_10 - Chapitre "Programmer pour le noyau Linux"

  Affichage au chargement et dechargement des PID et PPID de l'appelant

  Exemples de la formation "Programmation Noyau sous Linux"

  (c) 2005-2014 Christophe Blaess
  http://www.blaess.fr/christophe/

\************************************************************************/

#include <linux/module.h>
#include <linux/sched.h>
#include <linux/version.h>

static int __init exemple_10_init (void)
{
	printk(KERN_INFO "%s - Chargement par %u, son pere est %u\n",
	          THIS_MODULE->name,
	          current->pid,
	          current->real_parent->pid);
	return 0;
}
 
static void __exit exemple_10_exit (void)
{
	printk(KERN_INFO "%s - Dechargement par %u, son pere est %u\n",
	          THIS_MODULE->name,
	          current->pid,
	          current->real_parent->pid);
}

module_init(exemple_10_init);
module_exit(exemple_10_exit);
MODULE_LICENSE("GPL");

