/************************************************************************\
  Chapitre "Peripheriques en mode bloc"
  exemple_04

  Secteurs de taille configurable au chargement

  Exemples de la formation "Programmation Noyau sous Linux"

  (c) 2005-2015 Christophe Blaess
  http://www.blaess.fr/christophe/

\************************************************************************/

#include <linux/fs.h>
#include <linux/genhd.h>
#include <linux/hdreg.h>
#include <linux/blkdev.h>
#include <linux/module.h>
#include <linux/vmalloc.h>
#include <linux/spinlock.h>

	static int exemple_major = 0;
	module_param_named(major, exemple_major, int, 0444);

	#define NB_MINORS 16

	/* La taille des secteurs devient un parametre configurable,
	   par puissance de deux.
	   La valeur par defaut est 2^9 = 512 octets */

	#define PUISSANCE_SECTEUR_DEFAUT 9
	static int exemple_puissance_sect = PUISSANCE_SECTEUR_DEFAUT;
	module_param_named(puissance_sect, exemple_puissance_sect, int, 0444);

	#define LG_SECTEUR 512
	static int exemple_nb_sect = 4096;
	module_param_named(nb_sect, exemple_nb_sect, int, 0444);

	static char * exemple_data = NULL;

	static struct request_queue * exemple_request_queue;
	
	static struct gendisk * exemple_gendisk;

	static spinlock_t exemple_spinlock;

	static int exemple_getgeo (struct block_device *, struct hd_geometry *);

	static struct block_device_operations exemple_devops = {
		.owner   = THIS_MODULE,
		.getgeo  = exemple_getgeo,
	};


static void exemple_request(struct request_queue * rqueue)
{
	unsigned long secteur_debut;
	unsigned long nb_secteurs;
	struct request * rq;
	int erreur;

	rq = blk_fetch_request(rqueue);
	while (rq != NULL) {
		erreur = 0;
		if (rq->cmd_type != REQ_TYPE_FS) {
			erreur = -EIO;
			goto fin_requete;
		}

		/*
		 * Les numeros de secteurs pour le transfert correspondent 
		 * a des secteurs de 512 octets... -> convertir.
		 */
		secteur_debut = (blk_rq_pos(rq) * 512) >> exemple_puissance_sect;
		nb_secteurs   = (blk_rq_cur_sectors(rq) * 512) >> exemple_puissance_sect;

		if (secteur_debut + nb_secteurs > exemple_nb_sect) {
			erreur = -EIO;
			goto fin_requete;
		}

		if (rq_data_dir(rq)) { /* write */
			memmove(& exemple_data[secteur_debut << exemple_puissance_sect],
			        bio_data(rq->bio),
			        nb_secteurs << exemple_puissance_sect);
		} else /* read */ {
			memmove(bio_data(rq->bio),
			        & exemple_data[secteur_debut << exemple_puissance_sect],
			        nb_secteurs << exemple_puissance_sect);
		}
		
fin_requete:
		if (__blk_end_request_cur(rq, erreur) == 0)
			rq = blk_fetch_request(rqueue);
	}
}


/*
 * Encore une conversion puisque l'on veut des secteurs de 512 octets
 */
static int exemple_getgeo(struct block_device *bdev, struct hd_geometry *geo)
{
	geo->heads = 4;
	geo->sectors = 8;
	geo->cylinders = (exemple_nb_sect << exemple_puissance_sect) / 512/ geo->heads / geo->sectors;
	geo->start = 0;
	return 0;
}


static int __init exemple_init (void)
{
	int ret;

	if (exemple_nb_sect <= 0)
		return -EINVAL;

	exemple_data = vmalloc(exemple_nb_sect * LG_SECTEUR);
	if (exemple_data == NULL)
		return -ENOMEM;
	memset(exemple_data, 0, exemple_nb_sect * LG_SECTEUR);

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

	exemple_gendisk = alloc_disk(NB_MINORS);
	if (exemple_gendisk == NULL) {
		blk_cleanup_queue(exemple_request_queue);
		unregister_blkdev(exemple_major, THIS_MODULE->name);
		vfree(exemple_data);
		return -ENOMEM;
	}
	exemple_gendisk->major       = exemple_major;
	exemple_gendisk->first_minor = 0;
	exemple_gendisk->fops        = & exemple_devops;
	exemple_gendisk->queue       = exemple_request_queue;
	snprintf(exemple_gendisk->disk_name, 32, THIS_MODULE->name);
	set_capacity(exemple_gendisk, (exemple_nb_sect << exemple_puissance_sect) / 512);
	
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

