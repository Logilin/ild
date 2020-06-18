/************************************************************************\
  Exemples de la formation
    "Ecriture de drivers et programmation noyau Linux"
  Chapitre "Acces au materiel"

  (c) 2005-2019 Christophe Blaess
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


	static int  example_mmap (struct file * filp, struct vm_area_struct * vm);

	static struct file_operations example_fops = {
		.owner   =  THIS_MODULE,
		.mmap    =  example_mmap,
	};

	static struct miscdevice example_misc_driver = {
		    .minor          = MISC_DYNAMIC_MINOR,
		    .name           = THIS_MODULE->name,
		    .fops           = & example_fops,
	};

	struct timer_list example_timer;

	static char * example_buffer = NULL;

#if LINUX_VERSION_CODE < KERNEL_VERSION(4,15,0)
static void example_timer_function(unsigned long unused)
#else
static void example_timer_function(struct timer_list *unused)
#endif
{
	sprintf(example_buffer, "\r%s - %s(): %lu", THIS_MODULE->name, __FUNCTION__, jiffies);
	mod_timer(& example_timer, jiffies + HZ);
}



static int __init example_init (void)
{
	int err;
	struct page * pg = NULL;

	example_buffer = kzalloc(PAGE_SIZE, GFP_KERNEL);
	if (example_buffer == NULL)
		return -ENOMEM;

	example_buffer[0] = '\0';

	pg = virt_to_page(example_buffer);
	SetPageReserved(pg);

	err =  misc_register(& example_misc_driver);
	if (err != 0) {
		ClearPageReserved(pg);
		kzfree(example_buffer);
		example_buffer = NULL;
		return err;
	}

#if LINUX_VERSION_CODE < KERNEL_VERSION(4,15,0)
	init_timer (& example_timer);
	example_timer.function = example_timer_function;
#else
	timer_setup (& example_timer, example_timer_function, 0);
#endif
	example_timer.expires = jiffies + HZ;
	add_timer(& example_timer);

	return 0;
}


static void __exit example_exit (void)
{
	struct page * pg;

	del_timer(& example_timer);

	pg = virt_to_page(example_buffer);
	ClearPageReserved(pg);
	kzfree(example_buffer);
	example_buffer = NULL;

	misc_deregister(& example_misc_driver);
}


static int example_mmap (struct file * filp, struct vm_area_struct * vma)
{
	int err;

	if ((unsigned long) (vma->vm_end - vma->vm_start) > PAGE_SIZE)
		return -EINVAL;

	err = remap_pfn_range(vma,
	                    (unsigned long) (vma->vm_start),
	                    virt_to_phys(example_buffer) >> PAGE_SHIFT,
	                    vma->vm_end - vma->vm_start,
	                    vma->vm_page_prot);
	if (err != 0)
		return -EAGAIN;

	return 0;
}



	module_init(example_init);
	module_exit(example_exit);

	MODULE_DESCRIPTION("mmap() system call installation");
	MODULE_AUTHOR("Christophe Blaess <Christophe.Blaess@Logilin.fr>");
	MODULE_LICENSE("GPL");
