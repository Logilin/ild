/************************************************************************\
  exemple_10 - Chapitre "Ecriture de driver - peripherique caractere"

  Execution d'un thread noyau

  Exemples de la formation "Programmation Noyau sous Linux"

  (c) 2005-2014 Christophe Blaess
  http://www.blaess.fr/christophe/

\************************************************************************/

#include <linux/completion.h>
#include <linux/fs.h>
#include <linux/kthread.h>
#include <linux/module.h>
#include <linux/sched.h>


static DECLARE_COMPLETION(exemple_started);
static DECLARE_COMPLETION(exemple_stopped);

static int exemple_stop = 0;

int exemple_thread(void * arg)
{
	// struct sched_param param;
	// param.sched_priority = 20;
	// sched_setscheduler(current, SCHED_FIFO, &param);

	complete(& exemple_started);
	while (! exemple_stop) {
		printk(KERN_INFO "%s - Thread running, jiffies = %lu\n",
		                 THIS_MODULE->name, jiffies);
		// Mise en sommeil pour 1 seconde
		set_current_state(TASK_INTERRUPTIBLE);
		schedule_timeout(HZ);
	}
	complete_and_exit(& exemple_stopped, 0);
}


static int __init exemple_init (void)
{
	struct task_struct * thread;

	thread = kthread_run(exemple_thread, NULL, "ExempleThread");
	if (IS_ERR(thread))
              return -ENOMEM;
        wait_for_completion(& exemple_started);
	printk(KERN_INFO "%s - Thread lance\n", THIS_MODULE->name);
	return 0; 
}


static void __exit exemple_exit (void)
{
	exemple_stop = 1;
	wait_for_completion(& exemple_stopped);
	printk(KERN_INFO "%s - Thread fini\n", THIS_MODULE->name);
}


module_init(exemple_init);
module_exit(exemple_exit);
MODULE_LICENSE("GPL");

