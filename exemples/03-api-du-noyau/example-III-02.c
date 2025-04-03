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

#include <linux/jiffies.h>
#include <linux/module.h>
#include <linux/version.h>


static void display_time_values(void)
{

	time64_t   kgs =  ktime_get_seconds();
	time64_t   kgrs = ktime_get_real_seconds();
	time64_t   kgn =  ktime_get_ns();
	time64_t   kgrn = ktime_get_real_ns();

	pr_info("[%s] %s\n", THIS_MODULE->name, __func__);
	pr_info("ktime_get_seconds(): %lld\n", kgs);
	pr_info("ktime_get_real_seconds(): %lld\n", kgrs);
	pr_info("ktime_get_ns(): %lld\n", kgn);
	pr_info("ktime_get_real_ns(): %lld\n", kgrn);
}


static int __init example_init(void)
{
	display_time_values();

	return 0;
}


static void __exit example_exit(void)
{
	display_time_values();
}


module_init(example_init);
module_exit(example_exit);

MODULE_DESCRIPTION("Different representations of current time.");
MODULE_AUTHOR("Christophe Blaess <Christophe.Blaess@Logilin.fr>");
MODULE_LICENSE("GPL v2");

