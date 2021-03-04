// SPDX-License-Identifier: GPL-2.0
//
// Exemples de la formation
//  "Ecriture de drivers et programmation noyau Linux"
// Chapitre "A.P.I du noyau"
//
// (c) 2001-2021 Christophe Blaess
//
//    https://www.logilin.fr/
//

#include <linux/module.h>
#include <linux/sched.h>
#include <linux/timer.h>
#include <linux/version.h>


static struct timer_list example_timer;


#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 15, 0)
static void example_timer_function(struct timer_list *timer)
{
	pr_info("%s -%s: ktime_get_ns(): %lld\n",
		THIS_MODULE->name, __func__,
		ktime_get_ns());

	mod_timer(timer, jiffies + HZ);
}
#else
static void example_timer_function(unsigned long arg)
{
	struct timer_list *timer = (struct timer_list *) arg;
	struct timeval time_of_day;

	do_gettimeofday(&time_of_day);
	pr_info("%s - %s: time_of_day=%ld.%06ld\n",
		THIS_MODULE->name, __func__,
		time_of_day.tv_sec, time_of_day.tv_usec);

	mod_timer(timer, jiffies + HZ);
}
#endif



static int __init example_init(void)
{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 15, 0)
	timer_setup(&example_timer, example_timer_function, 0);
#else
	init_timer(&example_timer);
	example_timer.function = example_timer_function;
	example_timer.data = (unsigned long) (&example_timer);
#endif
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

