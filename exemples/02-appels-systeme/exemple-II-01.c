/************************************************************************\
  Exemples de la formation
    "Ecriture de drivers et programmation noyau Linux"
  Chapitre "Appels-systeme"

  (c) 2005-2019 Christophe Blaess
  http://www.blaess.fr/christophe/

\************************************************************************/

	#include <linux/module.h>
	#include <linux/sched.h>
	#include <linux/version.h>


static int __init example_init (void)
{
	int i;
	struct task_struct *task;

	task = current;
	i = 0;
	while (task != NULL) {
		printk(KERN_INFO "%s: (%d)  PID = %u, COMM = %s\n",
	          THIS_MODULE->name, i, task->pid, task->comm);
		if (task->real_parent == task) {
			printk(KERN_INFO "real_parent = self\n");
			break;
		}
		task = task->real_parent;
		i ++;
	}
	return 0;
}


static void __exit example_exit (void)
{
	printk(KERN_INFO "%s - %s(): PID = %u, PPID = %u\n",
	          THIS_MODULE->name, __FUNCTION__,
	          current->pid,
	          current->real_parent->pid);
}


	module_init(example_init);
	module_exit(example_exit);

	MODULE_DESCRIPTION("Informations about current process.");
	MODULE_AUTHOR("Christophe Blaess <Christophe.Blaess@Logilin.fr>");
	MODULE_LICENSE("GPL");

