/************************************************************************\
  Exemples de la formation
    "Ecriture de drivers et programmation noyau Linux"
  Chapitre "Acces au materiel"

  (c) 2005-2017 Christophe Blaess
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
	#include <linux/version.h>

	#include <asm/io.h>


	static int  exemple_mmap (struct file * filp, struct vm_area_struct * vm);

	static struct file_operations exemple_fops = {
		.owner   =  THIS_MODULE,
		.mmap    =  exemple_mmap,
	};

	static struct miscdevice exemple_misc_driver = {
		    .minor          = MISC_DYNAMIC_MINOR,
		    .name           = THIS_MODULE->name,
		    .fops           = & exemple_fops,
	};

	struct timer_list exemple_timer;

	static char * exemple_buffer = NULL;

#if LINUX_VERSION_CODE < KERNEL_VERSION(4,15,0)
static void exemple_timer_function(unsigned long unused)
#else
static void exemple_timer_function(struct timer_list *unused)
#endif
{
	sprintf(exemple_buffer, "\r%s - %s(): %lu", THIS_MODULE->name, __FUNCTION__, jiffies);
	mod_timer(& exemple_timer, jiffies + HZ);
}



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

#if LINUX_VERSION_CODE < KERNEL_VERSION(4,15,0)
	init_timer (& exemple_timer);
	exemple_timer.function = exemple_timer_function;
#else
	timer_setup (& exemple_timer, exemple_timer_function, 0);
#endif
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
	                    vma->vm_end - vma->vm_start,
	                    vma->vm_page_prot);
	if (err != 0)
		return -EAGAIN;

	return 0;
}



	module_init(exemple_init);
	module_exit(exemple_exit);

	MODULE_DESCRIPTION("mmap() system call installation");
	MODULE_AUTHOR("Christophe Blaess <Christophe.Blaess@Logilin.fr>");
	MODULE_LICENSE("GPL");

