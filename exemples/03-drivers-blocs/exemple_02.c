/************************************************************************\
  Chapitre "Peripheriques en mode bloc"
  exemple_02

  Appels-systeme d'acces au disque

  Exemples de la formation "Programmation Noyau sous Linux"

  (c) 2005-2015 Christophe Blaess
  http://www.blaess.fr/christophe/

\************************************************************************/

	#include <linux/fs.h>
	#include <linux/genhd.h>
	#include <linux/blkdev.h>
	#include <linux/module.h>
	#include <linux/vmalloc.h>
	#include <linux/spinlock.h>

	static int exemple_major = 0;
	module_param_named(major, exemple_major, int, 0444);

	/* Le numero mineur 0 correspond au disque dans son entier.
	 * Les numeros 1, 2, etc. correspondent aux partitions creees sur le disque.
	 * Le nombre maximal de mineurs influe sur le nombre maximal de partitions.
	 */
	#define EXEMPLE_MINORS 7

	#define EXEMPLE_SECTOR_SIZE 512
	static int exemple_sectors = 4096;
	module_param_named(sectors, exemple_sectors, int, 0444);

	static char * exemple_data = NULL;
	static struct request_queue  * exemple_request_queue;
	static struct gendisk        * exemple_gendisk;
	static spinlock_t              exemple_spinlock;

	static int  exemple_open    (struct block_device *, fmode_t);
	static void exemple_release (struct gendisk *, fmode_t);

	static struct block_device_operations exemple_devops = {
		.owner   = THIS_MODULE,
		.open    = exemple_open,
		.release = exemple_release,
	};



static int exemple_open(struct block_device * blkdev, fmode_t mode)
{
	printk(KERN_INFO "%s - %s()\n", THIS_MODULE->name, __FUNCTION__);
	return 0;
}



static void exemple_release(struct gendisk * disk, fmode_t mode)
{
	printk(KERN_INFO "%s - %s()\n", THIS_MODULE->name, __FUNCTION__);
}



static void exemple_request(struct request_queue * rqueue)
{
	unsigned long start;
	unsigned long length;
	struct request * rq;
	int err;

	/* Lire les requetes de la file. */
	rq = blk_fetch_request(rqueue);
	while (rq != NULL) {

		err = 0;
		if (rq->cmd_type != REQ_TYPE_FS) {
			err = -EIO;
			goto request_end;
		}

		start  = blk_rq_pos(rq);
		length = blk_rq_cur_sectors(rq);

		if (rq_data_dir(rq)) { /* write */
			memmove(& exemple_data[start * EXEMPLE_SECTOR_SIZE],
			        bio_data(rq->bio),
			        length * EXEMPLE_SECTOR_SIZE);
		} else /* read */ {
			memmove(bio_data(rq->bio),
			        & exemple_data[start * EXEMPLE_SECTOR_SIZE],
			        length * EXEMPLE_SECTOR_SIZE);
		}
request_end:
		if (__blk_end_request_cur(rq, err) == 0)
			rq = blk_fetch_request(rqueue);
	}
}



static int __init exemple_init (void)
{
	int ret;

	if (exemple_sectors <= 0)
		return -EINVAL;

	exemple_data = vmalloc(exemple_sectors * EXEMPLE_SECTOR_SIZE);
	if (exemple_data == NULL)
		return -ENOMEM;
	memset(exemple_data, 0, exemple_sectors * EXEMPLE_SECTOR_SIZE);

	ret = register_blkdev(exemple_major, THIS_MODULE->name);
	if (ret < 0) {
		vfree(exemple_data);
		return ret;
	}

	if (exemple_major == 0)
		exemple_major = ret;

	spin_lock_init(& exemple_spinlock);

	exemple_request_queue = blk_init_queue(exemple_request,
	                                     & exemple_spinlock);
	if (exemple_request_queue == NULL) {
		unregister_blkdev(exemple_major, THIS_MODULE->name);
		vfree(exemple_data);
		return -ENOMEM;
	}

	exemple_gendisk = alloc_disk(EXEMPLE_MINORS);
	if (exemple_gendisk == NULL) {
		blk_cleanup_queue(exemple_request_queue);
		unregister_blkdev(exemple_major, THIS_MODULE->name);
		vfree(exemple_data);
		return -ENOMEM;
	}
	exemple_gendisk->major       = exemple_major;
	exemple_gendisk->first_minor = 0;
	exemple_gendisk->fops        = &exemple_devops;
	exemple_gendisk->queue       = exemple_request_queue;
	snprintf(exemple_gendisk->disk_name, 32, THIS_MODULE->name);
	set_capacity(exemple_gendisk, exemple_sectors);

	add_disk(exemple_gendisk);

	return 0;
}



static void __exit exemple_exit (void)
{
	del_gendisk(exemple_gendisk);
	blk_cleanup_queue(exemple_request_queue);
	unregister_blkdev(exemple_major, THIS_MODULE->name);
	vfree(exemple_data);
}


	module_init(exemple_init);
	module_exit(exemple_exit);
	MODULE_LICENSE("GPL");
