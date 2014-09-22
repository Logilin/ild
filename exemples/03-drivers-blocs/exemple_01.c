/************************************************************************\
  Chapitre "Peripheriques en mode bloc"
  exemple_01

  Enregistrement du peripherique bloc

  Exemples de la formation "Programmation Noyau sous Linux"

  (c) 2005-2014 Christophe Blaess
  http://www.blaess.fr/christophe/

\************************************************************************/

#include <linux/module.h>
#include <linux/fs.h>

	static int exemple_major = 0;
	module_param_named(major, exemple_major, int, 0444);

static int __init exemple_init (void)
{
	int ret;

	ret = register_blkdev(exemple_major, THIS_MODULE->name);
	if (ret < 0)
		return ret;

	if (exemple_major == 0)
		/* Memoriser le numero attribue par le noyau */
		exemple_major = ret;

	return 0; 
}


static void __exit exemple_exit (void)
{
	unregister_blkdev(exemple_major, THIS_MODULE->name);
}

module_init(exemple_init);
module_exit(exemple_exit);
MODULE_LICENSE("GPL");

