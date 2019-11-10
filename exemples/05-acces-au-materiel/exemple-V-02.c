/************************************************************************\
  Exemples de la formation
    "Ecriture de drivers et programmation noyau Linux"
  Chapitre "Acces au materiel"

  (c) 2005-2019 Christophe Blaess
  http://www.blaess.fr/christophe/

\************************************************************************/

	#include <linux/completion.h>
	#include <linux/delay.h>
	#include <linux/fs.h>
	#include <linux/kthread.h>
	#include <linux/module.h>

	static DECLARE_COMPLETION(example_started);
	static DECLARE_COMPLETION(example_stopped);

	static int example_stop = 0;


int example_thread(void * arg)
{
	complete(& example_started);

	while (! example_stop) {
		printk(KERN_INFO "%s - %s(): Thread running, jiffies = %lu\n",
		                 THIS_MODULE->name, __FUNCTION__, jiffies);
		ssleep(1);
	}
	complete_and_exit(& example_stopped, 0);
}


static int __init example_init (void)
{
	struct task_struct * thread;

	thread = kthread_run(example_thread, NULL, THIS_MODULE->name);
	if (IS_ERR(thread))
              return -ENOMEM;
        wait_for_completion(& example_started);
	printk(KERN_INFO "%s - %s(): Thread started\n", THIS_MODULE->name, __FUNCTION__);

	return 0; 
}


static void __exit example_exit (void)
{
	example_stop = 1;
	wait_for_completion(& example_stopped);

	printk(KERN_INFO "%s - %s(): Thread terminated\n", THIS_MODULE->name, __FUNCTION__);
}


	module_init(example_init);
	module_exit(example_exit);

	MODULE_DESCRIPTION("Kernel thread implementation.");
	MODULE_AUTHOR("Christophe Blaess <Christophe.Blaess@Logilin.fr>");
	MODULE_LICENSE("GPL");

