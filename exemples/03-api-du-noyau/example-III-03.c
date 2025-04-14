// SPDX-License-Identifier: GPL-2.0
//
// Exemples de la formation
//  "Ecriture de drivers et programmation noyau Linux"
// Chapitre "A.P.I du noyau"
//
// (c) 2001-2025 Christophe Blaess
//
//    https://www.logilin.fr/
//

#include <linux/module.h>
#include <linux/sched.h>
#include <linux/timer.h>
#include <linux/version.h>


static struct timer_list example_timer;


static void example_timer_function(struct timer_list *timer)
{
	pr_info("%s -%s: ktime_get_ns(): %lld\n",
		THIS_MODULE->name, __func__,
		ktime_get_ns());

	mod_timer(timer, jiffies + HZ);
}



static int __init example_init(void)
{
	timer_setup(&example_timer, example_timer_function, 0);
	example_timer.expires = jiffies + HZ;
	add_timer(&example_timer);

	return 0;
}


static void __exit example_exit(void)
{
	del_timer(&example_timer);
}


module_init(example_init);
module_exit(example_exit);

MODULE_DESCRIPTION("Periodic message (current time).");
MODULE_AUTHOR("Christophe Blaess <Christophe.Blaess@Logilin.fr>");
MODULE_LICENSE("GPL v2");

