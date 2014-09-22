/************************************************************************\
  Chapitre "Peripheriques en mode bloc"
  exemple_02

  Appels-systeme d'acces au disque

  Exemples de la formation "Programmation Noyau sous Linux"
 
  (c) 2005-2014 Christophe Blaess
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
	#define NB_MINORS 16

	#define LG_SECTEUR 512
	static int exemple_nb_sect = 4096;
	module_param_named(nb_sect, exemple_nb_sect, int, 0444);

	static char * exemple_data = NULL;

	/* La structure request_queue stocke les requetes en attente */
	static struct request_queue * exemple_request_queue;
	
	/* La structure gendisk represente un disque generique.*/
	static struct gendisk * exemple_gendisk;

	/* Le spinlock est utilise en interne par le sous-systeme block
	 * pour limiter les acces concurrents.*/	
	static spinlock_t exemple_spinlock;

	static int  exemple_open    (struct block_device *, fmode_t);
	static void exemple_release (struct gendisk *, fmode_t);

	static struct block_device_operations exemple_devops = {
		.owner   = THIS_MODULE,
		.open    = exemple_open,
		.release = exemple_release,
	};


static int exemple_open(struct block_device * blkdev, fmode_t mode)
{
	/* Cette fonction est appelee a chaque ouverture du peripherique.
	 * Elle n'a pas d'utilite ici. */
	printk(KERN_INFO "%s: exemple_02_open()\n", THIS_MODULE->name);
	return 0;
}


static void exemple_release(struct gendisk * disk, fmode_t mode)
{
	printk(KERN_INFO "%s: exemple_02_release()\n", THIS_MODULE->name);
}


static void exemple_request(struct request_queue * rqueue)
{
	unsigned long secteur_debut;
	unsigned long nb_secteurs;
	struct request * rq;
	int erreur;

	/* Lire les requetes de la file. */
	rq = blk_fetch_request(rqueue);
	while (rq != NULL) {
		erreur = 0;
		/* Ignorer les requetes autres que celles du
		 * systeme de fichier. */
		if (rq->cmd_type != REQ_TYPE_FS) {
			erreur = -EIO;
			goto fin_requete;
		}

		/* Rechercher la position des donnees a transferer. */
		secteur_debut = blk_rq_pos(rq);
		nb_secteurs   = blk_rq_cur_sectors(rq);

		if (secteur_debut + nb_secteurs > exemple_nb_sect) {
			erreur = -EIO;
			goto fin_requete;
		}

		if (rq_data_dir(rq)) { /* write */
			memmove(& exemple_data[secteur_debut*LG_SECTEUR],
			        rq->buffer,
			        nb_secteurs*LG_SECTEUR);
		} else /* read */ {
			memmove(rq->buffer,
			        & exemple_data[secteur_debut*LG_SECTEUR],
			        nb_secteurs*LG_SECTEUR);
		}
		
fin_requete:
		/* Valider le traitement de la requete */
		if (__blk_end_request_cur(rq, erreur) == 0)
			/* Il n'y a plus de donnees dans la requete */
			rq = blk_fetch_request(rqueue);
	}
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

	/* Initialiser le spinlock et la file des requetes. */
	spin_lock_init(& exemple_spinlock);

	exemple_request_queue = blk_init_queue(exemple_request,
	                                     & exemple_spinlock);
	if (exemple_request_queue == NULL) {
		unregister_blkdev(exemple_major, THIS_MODULE->name);
		vfree(exemple_data);
		return -ENOMEM;
	}

	/* Initialiser le driver gendisk */
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
	set_capacity(exemple_gendisk, exemple_nb_sect);
	
	/* Enregistrer le driver gendisk */
	add_disk(exemple_gendisk);
		
	return 0; 
}


static void __exit exemple_exit (void)
{
	/* Supprimer le driver gendisk. */
	del_gendisk(exemple_gendisk);

	/* Vider la file des requetes.*/
	blk_cleanup_queue(exemple_request_queue);

	unregister_blkdev(exemple_major, THIS_MODULE->name);
	vfree(exemple_data);
}


module_init(exemple_init);
module_exit(exemple_exit);
MODULE_LICENSE("GPL");

