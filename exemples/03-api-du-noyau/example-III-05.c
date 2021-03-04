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

#include <linux/fs.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/slab.h>

#include <asm/io.h>


static int __init example_init(void)
{
	char *buffer;
	struct page *pg;
	unsigned int pfn;

	buffer = kmalloc(256, GFP_KERNEL);
	if (buffer == NULL)
		return -ENOMEM;

	pr_info("%s: kmalloc()      -> %pK\n",
		THIS_MODULE->name, buffer);

	pr_info("%s: virt-to-phys() -> %llx\n",
		THIS_MODULE->name, (unsigned long long) virt_to_phys(buffer));

	pfn = virt_to_phys(buffer) >> PAGE_SHIFT;
	pr_info("%s: pfn            -> %x\n",
		THIS_MODULE->name, pfn);

	pg = pfn_to_page(pfn);
	if (pg != NULL)
		pr_info("%s: page_address   -> %pK\n",
			THIS_MODULE->name, page_address(pg));

	kfree(buffer);
	return 0;
}


static void __exit example_exit(void)
{
}


module_init(example_init);
module_exit(example_exit);



MODULE_DESCRIPTION("Virtual and physical addresses.");
MODULE_AUTHOR("Christophe Blaess <Christophe.Blaess@Logilin.fr>");
MODULE_LICENSE("GPL v2");

