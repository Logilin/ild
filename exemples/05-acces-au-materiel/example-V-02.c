// SPDX-License-Identifier: GPL-2.0
//
// Exemples de la formation
//  "Ecriture de drivers et programmation noyau Linux"
// Chapitre "Drive ren mode caracteres"
//
// (c) 2001-2021 Christophe Blaess
//
//    https://www.logilin.fr/
//

#include <linux/completion.h>
#include <linux/delay.h>
#include <linux/fs.h>
#include <linux/kthread.h>
#include <linux/module.h>

static DECLARE_COMPLETION(example_started);
static DECLARE_COMPLETION(example_stopped);

static int example_stop;


int example_thread(void *arg)
{
	(void) arg;

	complete(&example_started);

	while (!example_stop) {
		pr_info("%s(): Thread running, jiffies = %lu\n",
			THIS_MODULE->name, jiffies);
		ssleep(1);
	}
	complete_and_exit(&example_stopped, 0);
}


static int __init example_init(void)
{
	struct task_struct *thread;

	thread = kthread_run(example_thread, NULL, THIS_MODULE->name);
	if (IS_ERR(thread))
		return -ENOMEM;
	wait_for_completion(&example_started);
	pr_info("%s: Thread started\n", THIS_MODULE->name);

	return 0;
}


static void __exit example_exit(void)
{
	example_stop = 1;
	wait_for_completion(&example_stopped);

	pr_info("%s: Thread terminated\n", THIS_MODULE->name);
}


module_init(example_init);
module_exit(example_exit);

MODULE_DESCRIPTION("Kernel thread implementation.");
MODULE_AUTHOR("Christophe Blaess <Christophe.Blaess@Logilin.fr>");
MODULE_LICENSE("GPL v2");

