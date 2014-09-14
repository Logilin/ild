/************************************************************************\
  exemple_05 - Chapitre "Programmer pour le noyau Linux"

  Manipulation de liste doublement chainee.

  Exemples de la formation "Programmation Noyau sous Linux"

  (c) 2005-2014 Christophe Blaess
  http://www.blaess.fr/christophe/

\************************************************************************/

#include <linux/list.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/string.h>

#define TAILLE_NOM_JOUR  16

typedef struct s_jour {
	struct list_head chainage;
	char nom [TAILLE_NOM_JOUR];
} jour_t ;


LIST_HEAD(semaine);

static int __init exemple_05_init (void)
{
	jour_t * nouveau;

	nouveau = kmalloc(sizeof(jour_t), GFP_KERNEL);
	if (nouveau != NULL) {
		strncpy(nouveau->nom, "Lundi", TAILLE_NOM_JOUR);
		list_add(& nouveau->chainage, & semaine);
	}

	nouveau = kmalloc(sizeof(jour_t), GFP_KERNEL);
	if (nouveau != NULL) {
		strncpy(nouveau->nom, "Dimanche", TAILLE_NOM_JOUR);
		list_add(& nouveau->chainage, & semaine);
	}

	nouveau = kmalloc(sizeof(jour_t), GFP_KERNEL);
	if (nouveau != NULL) {
		strncpy(nouveau->nom, "Mardi", TAILLE_NOM_JOUR);
		list_add_tail(& nouveau->chainage, & semaine);
	}

	return 0;
}


static void __exit exemple_05_exit (void)
{
	jour_t * jour;
	jour_t * n;

	list_for_each_entry(jour, & semaine, chainage) {
		printk(KERN_INFO "%s\n", jour->nom);
	}
	
	list_for_each_entry_safe(jour, n, & semaine, chainage) {
		kfree(jour);
	}
}

module_init(exemple_05_init);
module_exit(exemple_05_exit);

MODULE_LICENSE("GPL");

