/************************************************************************\
  Exemples de la formation
    "Ecriture de drivers et programmation noyau Linux"
  Chapitre "A.P.I. du noyau"

  (c) 2005-2017 Christophe Blaess
  http://www.blaess.fr/christophe/

\************************************************************************/

	#include <linux/debugfs.h>
	#include <linux/module.h>

	static u8   exemple_u8  = 123;
	static u16  exemple_u16 = 12345;
	static u32  exemple_u32 = 1234567890;

	struct {
		struct dentry * directory;
		struct dentry * u8;
		struct dentry * u16;
		struct dentry * u32;

	} exemple_debugfs;



static int __init exemple_init (void)
{
	exemple_debugfs.directory = debugfs_create_dir(THIS_MODULE->name, NULL);
	if (IS_ERR(exemple_debugfs.directory))
		return -EINVAL;

	exemple_debugfs.u8 = debugfs_create_u8("u8", 0644, exemple_debugfs.directory, & exemple_u8);
	if (IS_ERR(exemple_debugfs.u8)) {
		debugfs_remove(exemple_debugfs.directory);
		return -EINVAL;
	}

	exemple_debugfs.u16 = debugfs_create_u16("u16", 0644, exemple_debugfs.directory, & exemple_u16);
	if (IS_ERR(exemple_debugfs.u8)) {
		debugfs_remove(exemple_debugfs.u8);
		debugfs_remove(exemple_debugfs.directory);
		return -EINVAL;
	}

	exemple_debugfs.u32 = debugfs_create_u32("u32", 0644, exemple_debugfs.directory, & exemple_u32);
	if (IS_ERR(exemple_debugfs.u32)) {
		debugfs_remove(exemple_debugfs.u8);
		debugfs_remove(exemple_debugfs.u16);
		debugfs_remove(exemple_debugfs.directory);
		return -EINVAL;
	}

	printk(KERN_INFO "%s: exemple_u8  = %u\n", THIS_MODULE->name, exemple_u8);
	printk(KERN_INFO "%s: exemple_u16 = %u\n", THIS_MODULE->name, exemple_u16);
	printk(KERN_INFO "%s: exemple_u32 = %u\n", THIS_MODULE->name, exemple_u32);

	return 0;
}


static void __exit exemple_exit (void)
{
	printk(KERN_INFO "%s: exemple_u8  = %u\n", THIS_MODULE->name, exemple_u8);
	printk(KERN_INFO "%s: exemple_u16 = %u\n", THIS_MODULE->name, exemple_u16);
	printk(KERN_INFO "%s: exemple_u32 = %u\n", THIS_MODULE->name, exemple_u32);


	debugfs_remove(exemple_debugfs.u8);
	debugfs_remove(exemple_debugfs.u16);
	debugfs_remove(exemple_debugfs.u32);
	debugfs_remove(exemple_debugfs.directory);
}



	module_init(exemple_init);
	module_exit(exemple_exit);

	MODULE_DESCRIPTION("Debugfs usage example.");
	MODULE_AUTHOR("Christophe Blaess <Christophe.Blaess@Logilin.fr>");
	MODULE_LICENSE("GPL");

