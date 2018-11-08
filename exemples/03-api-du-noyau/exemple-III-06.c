/************************************************************************\
  Exemples de la formation
    "Ecriture de drivers et programmation noyau Linux"
  Chapitre "A.P.I. du noyau"

  (c) 2005-2017 Christophe Blaess
  http://www.blaess.fr/christophe/

\************************************************************************/

	#include <linux/fs.h>
	#include <linux/mm.h>
	#include <linux/module.h>
	#include <linux/slab.h>

	#include <asm/io.h>


static int __init exemple_init (void)
{
	char * buffer;
	struct page * pg;
	unsigned int pfn;

	buffer = kmalloc(256, GFP_KERNEL);
	if (buffer == NULL)
		return -ENOMEM;

	printk("%s: kmalloc()      -> %pK\n",
	       THIS_MODULE->name, buffer);

	printk("%s: virt-to-phys() -> %llx\n",
	       THIS_MODULE->name, (long long unsigned int) virt_to_phys(buffer));

	pfn = virt_to_phys(buffer) >> PAGE_SHIFT;
	printk("%s: pfn            -> %x\n",
	       THIS_MODULE->name, pfn);

	pg = pfn_to_page(pfn);
	if (pg != NULL)
		printk("%s: page_address   -> %pK\n",
		       THIS_MODULE->name, page_address(pg));

	kfree(buffer);
	return 0;
}


static void __exit exemple_exit (void)
{
}


	module_init(exemple_init);
	module_exit(exemple_exit);

	MODULE_DESCRIPTION("Virtual and physical addresses.");
	MODULE_AUTHOR("Christophe Blaess <Christophe.Blaess@Logilin.fr>");
	MODULE_LICENSE("GPL");

