/************************************************************************\
  TP de la formation
    "Ecriture de drivers et programmation noyau Linux"
  Chapitre "Programmer pour le noyau Linux"

  (c) 2005-2015 Christophe Blaess
  http://www.blaess.fr/christophe/

\************************************************************************/

	#include <linux/module.h>


#define NB_SAMPLES 50

static int __init exemple_init (void)
{
	int i;
	static struct timeval tv [NB_SAMPLES];

	for (i = 0; i < NB_SAMPLES; i ++)
		do_gettimeofday(&(tv[i]));

	for (i = 0; i < NB_SAMPLES; i ++)
		printk(KERN_INFO "%2d : %ld.%06ld\n", i, tv[i].tv_sec, tv[i].tv_usec);

	return 0;
}


static void __exit exemple_exit (void)
{
}


	module_init(exemple_init);
	module_exit(exemple_exit);

	MODULE_DESCRIPTION("Exercise correction.");
	MODULE_AUTHOR("Christophe Blaess <Christophe.Blaess@Logilin.fr>");
	MODULE_LICENSE("GPL");

