// SPDX-License-Identifier: GPL-2.0
//
// Exemples de la formation
//  "Ecriture de drivers et programmation noyau Linux"
// Chapitre "Driver en mode caracteres"
//
// (c) 2001-2023 Christophe Blaess
//
//    https://www.logilin.fr/
//

#include <linux/delay.h>
#include <linux/fs.h>
#include <linux/kthread.h>
#include <linux/module.h>
#include <linux/version.h>


struct task_struct *example_thread_id;

int example_thread_function(void *arg)
{
	(void) arg;

	while (! kthread_should_stop()) {

		pr_info("%s(): Thread running, jiffies = %lu\n",
			THIS_MODULE->name, jiffies);

		ssleep(1);
	}
	do_exit(0);
}


static int __init example_init(void)
{
	example_thread_id = kthread_run(example_thread_function, NULL, THIS_MODULE->name);
	if (IS_ERR(example_thread_id))
		return -ENOMEM;

	pr_info("%s: Thread started\n", THIS_MODULE->name);

	return 0;
}


static void __exit example_exit(void)
{
	kthread_stop(example_thread_id);
	pr_info("%s: Thread terminated\n", THIS_MODULE->name);
}


module_init(example_init);
module_exit(example_exit);

MODULE_DESCRIPTION("Kernel thread implementation.");
MODULE_AUTHOR("Christophe Blaess <Christophe.Blaess@Logilin.fr>");
MODULE_LICENSE("GPL v2");

