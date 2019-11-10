/************************************************************************\
  Exemples de la formation
    "Ecriture de drivers et programmation noyau Linux"
  Chapitre "Driver blocs et VFS"

  (c) 2005-2019 Christophe Blaess
  http://www.blaess.fr/christophe/

\************************************************************************/

	#include <linux/module.h>
	#include <linux/fs.h>


	static int example_major = 0;
	module_param_named(major, example_major, int, 0444);


static int __init example_init (void)
{
	int ret;

	ret = register_blkdev(example_major, THIS_MODULE->name);
	if (ret < 0)
		return ret;

	if (example_major == 0)
		example_major = ret;

	return 0;
}


static void __exit example_exit (void)
{
	unregister_blkdev(example_major, THIS_MODULE->name);
}


	module_init(example_init);
	module_exit(example_exit);

	MODULE_DESCRIPTION("Major number reservation.");
	MODULE_AUTHOR("Christophe Blaess <Christophe.Blaess@Logilin.fr>");
	MODULE_LICENSE("GPL");

