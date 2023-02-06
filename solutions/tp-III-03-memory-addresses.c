// SPDX-License-Identifier: GPL-2.0
//
// Exemples de la formation
//  "Ecriture de drivers et programmation noyau Linux"
//
// (c) 2001-2023 Christophe Blaess
//
//    https://www.logilin.fr/
//

#include <linux/fs.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/slab.h>

#include <asm/io.h>


static int __init memory_addresses_init(void)
{
	char *buffer;
	struct page *pg;
	unsigned int pfn;

	buffer = kmalloc(256, GFP_KERNEL);
	if (buffer == NULL)
		return -ENOMEM;

	pr_info("%s: kmalloc()      -> %016llX\n",
		THIS_MODULE->name, (unsigned long long)buffer);

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


static void __exit memory_addresses_exit(void)
{
}


module_init(memory_addresses_init);
module_exit(memory_addresses_exit);



MODULE_DESCRIPTION("Exercise III-03 solution.");
MODULE_AUTHOR("Christophe Blaess <Christophe.Blaess@Logilin.fr>");
MODULE_LICENSE("GPL v2");
