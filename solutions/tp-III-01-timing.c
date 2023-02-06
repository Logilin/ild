// SPDX-License-Identifier: GPL-2.0
//
// Exemples de la formation
//  "Ecriture de drivers et programmation noyau Linux"
//
// (c) 2001-2023 Christophe Blaess
//
//    https://www.logilin.fr/
//

#include <linux/jiffies.h>
#include <linux/module.h>
#include <linux/version.h>

#define NB_SAMPLES    50

static int __init timing_init(void)
{
	int i;

#if LINUX_VERSION_CODE >= KERNEL_VERSION(3, 17, 0)
	time64_t  sample[NB_SAMPLES];

	for (i = 0; i < NB_SAMPLES; i++)
		sample[i] = ktime_get_real_ns();
	for (i = 0; i < NB_SAMPLES; i++)
		pr_info("%02d: %lld\n", i, sample[i]);

#else
	struct timespec sample[NB_SAMPLES];
	for (i = 0; i < NB_SAMPLES; i++)
		getnstimeofday(&(sample[i]));
	for (i = 0; i < NB_SAMPLES; i++)
		pr_info("%02d: %ld%09ld\n", i, sample[i].tv_sec, sample[i].tv_nsec);
#endif

	return 0;
}


static void __exit timing_exit(void)
{
}


module_init(timing_init);
module_exit(timing_exit);

MODULE_DESCRIPTION("Exercise III-01 solution.");
MODULE_AUTHOR("Christophe Blaess <Christophe.Blaess@Logilin.fr>");
MODULE_LICENSE("GPL v2");
