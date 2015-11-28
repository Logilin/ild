/************************************************************************\
  Exemples de la formation
    "Ecriture de drivers et programmation noyau Linux"
  Chapitre "Ecriture de driver en mode caractere"

  (c) 2005-2015 Christophe Blaess
  http://www.blaess.fr/christophe/

\************************************************************************/

	#include <linux/fs.h>
	#include <linux/module.h>

	static dev_t exemple_dev = MKDEV(0, 0);

	static int exemple_major = 0;
	module_param_named(major, exemple_major, int, 0644);


static int __init exemple_init (void)
{
	int err;

	if (exemple_major == 0) {
		err = alloc_chrdev_region(& exemple_dev, 0, 1, THIS_MODULE->name);
	} else {
		exemple_dev = MKDEV(exemple_major, 0);
		err = register_chrdev_region(exemple_dev, 1, THIS_MODULE->name);
	}

	if (err < 0)
		return err;
	return 0; 
}


static void __exit exemple_exit (void)
{
	unregister_chrdev_region(exemple_dev, 1);
}


	module_init(exemple_init);
	module_exit(exemple_exit);

	MODULE_DESCRIPTION("Major number and minor range reservation");
	MODULE_AUTHOR("Christophe Blaess <Christophe.Blaess@Logilin.fr>");
	MODULE_LICENSE("GPL");

