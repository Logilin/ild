/************************************************************************\
  exemple_21 - Chapitre - "Ecriture de driver - peripherique caractere"

  Implementation de l'appel systeme mmap()

  Exemples de la formation "Programmation Noyau sous Linux"
 
  (c) 2005-2014 Christophe Blaess
  http://www.blaess.fr/christophe/

\************************************************************************/

#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/timer.h>






	static int  exemple_mmap (struct file * filp, struct vm_area_struct * vm);

	static struct file_operations fops_exemple = {
		.owner   =  THIS_MODULE,
		.mmap    =  exemple_mmap,
	};


	static struct miscdevice exemple_misc_driver = {
		    .minor          = MISC_DYNAMIC_MINOR,
		    .name           = THIS_MODULE->name,
		    .fops           = & fops_exemple,
	};

	static void exemple_timer_func (unsigned long arg);
	struct timer_list exemple_timer;

	static char * exemple_buffer = NULL;

static int __init exemple_init (void)
{
	int err;
	struct page * pg = NULL;


	exemple_buffer = kmalloc(PAGE_SIZE, GFP_KERNEL);
	if (exemple_buffer == NULL)
		return -ENOMEM;
	exemple_buffer[0] = '\0';

	pg = virt_to_page(exemple_buffer);
	SetPageReserved(pg);

	err =  misc_register(& exemple_misc_driver);
	if (err != 0) {
		ClearPageReserved(pg);
		kfree(exemple_buffer);
		exemple_buffer = NULL;
		return err;
	}

	init_timer(& exemple_timer);
	exemple_timer.function = exemple_timer_func;
	exemple_timer.expires = jiffies + HZ;
	add_timer(& exemple_timer);

	return 0;
}


static void __exit exemple_exit (void)
{
	struct page * pg;

	del_timer(& exemple_timer);

	pg = virt_to_page(exemple_buffer);
	ClearPageReserved(pg);
	kfree(exemple_buffer);
	exemple_buffer = NULL;

	misc_deregister(& exemple_misc_driver);
}




static int exemple_mmap (struct file * filp, struct vm_area_struct * vma)
{
	int err;

	if ((unsigned long) (vma->vm_end - vma->vm_start) > PAGE_SIZE)
		return -EINVAL;
	err = remap_pfn_range(vma,
	                    (unsigned long) (vma->vm_start),
	                    virt_to_phys(exemple_buffer) >> PAGE_SHIFT,
	                    PAGE_SIZE,
	                    vma->vm_page_prot);
	if (err != 0)
		return -EAGAIN;
	return 0;
}


static void exemple_timer_func (unsigned long arg)
{
	sprintf(exemple_buffer, "\r%s - %lu", THIS_MODULE->name, jiffies);
	mod_timer(& exemple_timer, jiffies + HZ);
}


module_init(exemple_init);
module_exit(exemple_exit);
MODULE_LICENSE("GPL");

