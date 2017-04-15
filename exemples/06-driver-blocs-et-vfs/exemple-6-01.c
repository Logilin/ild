/************************************************************************\
  Exemples de la formation
    "Ecriture de drivers et programmation noyau Linux"
  Chapitre "Driver blocs et VFS"

  (c) 2005-2017 Christophe Blaess
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
		exemple_major = ret;

	return 0;
}


static void __exit exemple_exit (void)
{
	unregister_blkdev(exemple_major, THIS_MODULE->name);
}


	module_init(exemple_init);
	module_exit(exemple_exit);

	MODULE_DESCRIPTION("Major number reservation.");
	MODULE_AUTHOR("Christophe Blaess <Christophe.Blaess@Logilin.fr>");
	MODULE_LICENSE("GPL");

