/************************************************************************\
  Exemples de la formation
    "Ecriture de drivers et programmation noyau Linux"
  Chapitre "Driver blocs et VFS"

  (c) 2005-2019 Christophe Blaess
  http://www.blaess.fr/christophe/

\************************************************************************/

	#include <linux/fs.h>
	#include <linux/genhd.h>
	#include <linux/blkdev.h>
	#include <linux/module.h>
	#include <linux/vmalloc.h>
	#include <linux/spinlock.h>


	static int example_major = 0;
	module_param_named(major, example_major, int, 0444);

	#define EXEMPLE_MINORS 9

	#define EXEMPLE_SECTOR_SIZE 512
	static int example_sectors = 4096;
	module_param_named(sectors, example_sectors, int, 0444);

	static char * example_data = NULL;
	static struct request_queue  * example_request_queue;
	static struct gendisk        * example_gendisk;
	static spinlock_t              example_spinlock;

	static int  example_open    (struct block_device *, fmode_t);
	static void example_release (struct gendisk *, fmode_t);

	static struct block_device_operations example_devops = {
		.owner   = THIS_MODULE,
		.open    = example_open,
		.release = example_release,
	};


static int example_open(struct block_device * blkdev, fmode_t mode)
{
	printk(KERN_INFO "%s - %s()\n", THIS_MODULE->name, __FUNCTION__);
	return 0;
}


static void example_release(struct gendisk * disk, fmode_t mode)
{
	printk(KERN_INFO "%s - %s()\n", THIS_MODULE->name, __FUNCTION__);
}


static void example_request(struct request_queue * rqueue)
{
	unsigned long start;
	unsigned long length;
	struct request * rq;

	rq = blk_fetch_request(rqueue);
	while (rq != NULL) {

		start  = blk_rq_pos(rq);
		length = blk_rq_cur_sectors(rq);

		if (rq_data_dir(rq)) { /* write */
			memcpy(& example_data[start * EXEMPLE_SECTOR_SIZE],
			       bio_data(rq->bio),
			       length * EXEMPLE_SECTOR_SIZE);
		} else /* read */ {
			memcpy(bio_data(rq->bio),
			       & example_data[start * EXEMPLE_SECTOR_SIZE],
			       length * EXEMPLE_SECTOR_SIZE);
		}

		if (__blk_end_request_cur(rq, 0) == 0)
			rq = blk_fetch_request(rqueue);
	}
}


static int __init example_init (void)
{
	int ret;

	if (example_sectors <= 0)
		return -EINVAL;

	example_data = vmalloc(example_sectors * EXEMPLE_SECTOR_SIZE);
	if (example_data == NULL)
		return -ENOMEM;
	memset(example_data, 0, example_sectors * EXEMPLE_SECTOR_SIZE);

	ret = register_blkdev(example_major, THIS_MODULE->name);
	if (ret < 0) {
		vfree(example_data);
		return ret;
	}

	if (example_major == 0)
		example_major = ret;

	spin_lock_init(& example_spinlock);

	example_request_queue = blk_init_queue(example_request,
	                                     & example_spinlock);
	if (example_request_queue == NULL) {
		unregister_blkdev(example_major, THIS_MODULE->name);
		vfree(example_data);
		return -ENOMEM;
	}

	example_gendisk = alloc_disk(EXEMPLE_MINORS);
	if (example_gendisk == NULL) {
		blk_cleanup_queue(example_request_queue);
		unregister_blkdev(example_major, THIS_MODULE->name);
		vfree(example_data);
		return -ENOMEM;
	}
	example_gendisk->major       = example_major;
	example_gendisk->first_minor = 0;
	example_gendisk->fops        = &example_devops;
	example_gendisk->queue       = example_request_queue;
	snprintf(example_gendisk->disk_name, 32, THIS_MODULE->name);
	set_capacity(example_gendisk, example_sectors);

	add_disk(example_gendisk);

	return 0;
}


static void __exit example_exit (void)
{
	del_gendisk(example_gendisk);
	blk_cleanup_queue(example_request_queue);
	unregister_blkdev(example_major, THIS_MODULE->name);
	vfree(example_data);
}


	module_init(example_init);
	module_exit(example_exit);

	MODULE_DESCRIPTION("Virtual block device implementation.");
	MODULE_AUTHOR("Christophe Blaess <Christophe.Blaess@Logilin.fr>");
	MODULE_LICENSE("GPL");

