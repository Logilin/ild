/****************************************************************************\
   exemple_fpga.c - Driver pour carte Altera - Projet #####
   
   2012 Logilin pour #####
\****************************************************************************/

#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/interrupt.h>
#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/pagemap.h>
#include <linux/pci.h>
#include <linux/proc_fs.h>
#include <linux/slab.h>
#include <linux/spinlock.h>
#include <linux/string.h>
#include <linux/version.h>

#include "exemple-fpga.h"

// Constantes du document exemple "PCI Express" §1.01 p.14
#define EXEMPLE_FPGA_MODULE_MEM_START         0x00000000
#define EXEMPLE_FPGA_MODULE_MEM_LENGTH        0x00001000
#define EXEMPLE_FPGA_MODULE_LEDS_START        0x00001000
#define EXEMPLE_FPGA_MODULE_LEDS_LENGTH       0x00000020
#define EXEMPLE_FPGA_MODULE_SWITCHES_START    0x00002000
#define EXEMPLE_FPGA_MODULE_SWITCHES_LENGTH   0x00000020
#define EXEMPLE_FPGA_MODULE_CONTROLS_START    0x00004000
#define EXEMPLE_FPGA_MODULE_CONTROLS_LENGTH   0x00004000



// ---------------------------------------------------------------------------
// Gestion de l'interface avec l'espace utilisateur
// ---------------------------------------------------------------------------


// Affectation des numeros mineurs de peripheriques

#define EXEMPLE_FPGA_MINOR_MEM        0
#define EXEMPLE_FPGA_MINOR_LEDS       1
#define EXEMPLE_FPGA_MINOR_SWITCHES   2

#define EXEMPLE_FPGA_FIRST_MINOR      0
#define EXEMPLE_FPGA_NB_MINORS        3


// Noms des peripheriques caracteres geres par le module
static char * module_names [] = {
	"exemple-fpga-mem",
	"exemple-fpga-leds",
	"exemple-fpga-switches",
	NULL
};



// Une structure specifique dans laquelle on stocke les donnees privées.
struct exemple_fpga_s {

	// Adresse de projection des modules 
	unsigned char * module_mem;
	unsigned char * module_leds;
	unsigned char * module_switches;
	// Protection contre l'ouverture multiple
	struct mutex    mtx_open;
	int             module_open[3];
	
	// Le module de controle PCIe non-accessible depuis l'espace user
	unsigned char * module_controls;
	
	// Adresse d'I/O du peripherique PCIe
	unsigned long   iobase;

	// NUmeros d'identification du driver (majeur & premier mineur)
	dev_t           dev;
	
	// Peripherique de type caractere
	struct cdev     cdev;
	
	// Utilisation des interruptions MSI
	int             msi_enabled;
	
	// Activation du handler d'interruption
	int             irq_handling;
};



// Un parametre pour desactiver totalement les interruptions
static int exemple_fpga_disable_irq = 0;
	module_param_named(disable_irq, exemple_fpga_disable_irq, int, 0644);
	MODULE_PARM_DESC(disable_irq, "disable all interrupt handling");


// Une classe de peripherique specifique pour notre driver.
static struct class * exemple_fpga_class = NULL;
// Un mutex pour proteger l'acces a cette structure.
DEFINE_MUTEX(exemple_fpga_class_mtx);



// Methodes definies ci-dessous (appels-systeme depuis espace utlisateur).
static int  exemple_fpga_open    (struct inode *,  struct file *);
static int  exemple_fpga_release (struct inode *,  struct file *);
static ssize_t exemple_fpga_read (struct file *,   char __user *, size_t, loff_t *);
static ssize_t exemple_fpga_write(struct file *,   const char __user *, size_t, loff_t *);
static int  exemple_fpga_mmap    (struct file *,   struct vm_area_struct *);
static long exemple_fpga_ioctl   (struct file *,   unsigned int, unsigned long int);



// Methodes du peripherique caractere.
static struct file_operations exemple_fpga_fops = {
	.owner   =  THIS_MODULE,
	.open    =  exemple_fpga_open,
	.release =  exemple_fpga_release,
	.read    =  exemple_fpga_read,
	.write   =  exemple_fpga_write,
	.mmap    =  exemple_fpga_mmap,
	.unlocked_ioctl = exemple_fpga_ioctl,
};



// Enregistrement d'un nouveau peripherique de type caractere
static int exemple_fpga_initialize_char_device(struct exemple_fpga_s * exemple_fpga)
{
	int i;
	int err;
	
	// Reserver un numero majeur dont le mineur 0 soit libre
	err = alloc_chrdev_region(& (exemple_fpga->dev), EXEMPLE_FPGA_FIRST_MINOR, EXEMPLE_FPGA_NB_MINORS, THIS_MODULE->name);
	if (err < 0) {
		kfree(exemple_fpga);
		return EINVAL;
	}
	
	// Inscrire un peripherique dans la classe specifique
	mutex_lock(&exemple_fpga_class_mtx);
	for (i = 0; i < EXEMPLE_FPGA_NB_MINORS; i ++) 
		device_create(exemple_fpga_class, NULL, MKDEV(MAJOR(exemple_fpga->dev), MINOR(exemple_fpga->dev) + i), NULL, module_names[i]);
	
	// Associer le peripherique caractere et ses methodes
	cdev_init(& (exemple_fpga->cdev), & exemple_fpga_fops);
	
	// Enregistrer le peripherique avec les numeros majeur/mineur obtenus
	if ((err = cdev_add(& (exemple_fpga->cdev), exemple_fpga->dev, EXEMPLE_FPGA_NB_MINORS)) != 0) {
		device_destroy(exemple_fpga_class, exemple_fpga->dev);
		mutex_unlock(&exemple_fpga_class_mtx);
		unregister_chrdev_region(exemple_fpga->dev, EXEMPLE_FPGA_NB_MINORS);
		return err;
	}
	mutex_unlock(&exemple_fpga_class_mtx);
	return 0;
}



// Liberation d'un peripherique de type caractere.
static void exemple_fpga_release_char_device(struct exemple_fpga_s * exemple_fpga)
{
	int i;
	mutex_lock(&exemple_fpga_class_mtx);
	cdev_del (& (exemple_fpga->cdev));
	for (i = 0; i < EXEMPLE_FPGA_NB_MINORS; i ++) 
		device_destroy(exemple_fpga_class, MKDEV(MAJOR(exemple_fpga->dev), MINOR(exemple_fpga->dev) + i));
	mutex_unlock(&exemple_fpga_class_mtx);
	unregister_chrdev_region(exemple_fpga->dev, EXEMPLE_FPGA_NB_MINORS);
}



// Une structure pour regrouper la configuration et le numero mineur.
struct exemple_fpga_file_private_s {
	struct exemple_fpga_s * exemple_fpga;
	int                 minor;
};



// Methode d'ouverture du peripherique caractere
static int exemple_fpga_open(struct inode * indp,  struct file * filp)
{
	struct exemple_fpga_s * exemple_fpga;
	struct exemple_fpga_file_private_s * private_data;
	
	// Le mode d'ouverture pourrait etre consulte avec
	//     filp->f_mode & FMODE_READ
	// et  filp->f_mode & FMODE_WRITE
	
	
	// Rechercher la structure de configuration correspondant.
	exemple_fpga = container_of(indp->i_cdev, struct exemple_fpga_s, cdev);
	
	// Assurer l'unicite d'ouverture du module
	mutex_lock(& (exemple_fpga->mtx_open));
	if (exemple_fpga->module_open[iminor(indp)] != 0) {
		mutex_unlock(& (exemple_fpga->mtx_open));
		return -EBUSY;
	}
	
	// Allouer une structure privee
	if ((private_data = kmalloc(sizeof(struct exemple_fpga_file_private_s), GFP_KERNEL)) == NULL) {
		mutex_unlock(& (exemple_fpga->mtx_open));
		return -ENOMEM;
	}
	
	// Y sauver la configuration et le mineur (pour les autres methodes).
	private_data->exemple_fpga = exemple_fpga;
	private_data->minor = iminor(indp);
	filp->private_data = private_data;
	
	// Ouverture Ok.
	exemple_fpga->module_open[iminor(indp)] ++;
	mutex_unlock(& (exemple_fpga->mtx_open));
	return 0;
}



// Methode de fermeture du peripherique caractere
static int exemple_fpga_release(struct inode * indp,  struct file * filp)
{
	struct exemple_fpga_file_private_s * private_data = filp->private_data;
	struct exemple_fpga_s * exemple_fpga = private_data->exemple_fpga;
	
	mutex_lock(& (exemple_fpga->mtx_open));
	exemple_fpga->module_open[iminor(indp)]--;
	mutex_unlock(& (exemple_fpga->mtx_open));
	
	// Fermeture Ok.
	return 0;
}



// Methode de lecture sur un module
static ssize_t exemple_fpga_read  (struct file * filp, char __user * buffer, size_t size, loff_t * offset)
{
	int i;
	int err;
	int length;
	unsigned char * kbuffer;
	unsigned char * start;

	struct exemple_fpga_file_private_s * private_data = filp->private_data;
	struct exemple_fpga_s * exemple_fpga = private_data->exemple_fpga;
	int minor = private_data->minor;
	
	switch (minor) {
		case EXEMPLE_FPGA_MINOR_MEM:
			length = EXEMPLE_FPGA_MODULE_MEM_LENGTH - (* offset);
			start  = exemple_fpga->module_mem + (* offset);
			break;
		case EXEMPLE_FPGA_MINOR_LEDS:
			length = 4;
			start  = exemple_fpga->module_leds;
			break;
		case EXEMPLE_FPGA_MINOR_SWITCHES:
			length = 4;
			start  = exemple_fpga->module_switches;
			break;
		default:
			return  -EINVAL;
	}
	if (size < length)
		length = size;
	if (length <= 0)
		return 0; // End of file
	
	// Allouer un buffer noyau et y lire les donnees I/O.
	if ((kbuffer = kmalloc(length, GFP_KERNEL)) == NULL)
		return -ENOMEM;
	for (i = 0; i < length; i ++)
		kbuffer[i] = ioread8(start+i);
	
	// Copier le buffer vers celui de l'espace utilisateur.
	err = copy_to_user(buffer, kbuffer, length);
	kfree(kbuffer);
	if (err != 0)
		return -EFAULT;
	
	// Incrementer l'offset pour la prochaine lecture.
	(* offset) += length;
	
	// Renvoyer la longueur copiee.
	return length;
}



// Methode d'ecriture vers un module
static ssize_t exemple_fpga_write (struct file * filp, const char __user * buffer, size_t size, loff_t * offset)
{
	int i;
	int length;
	unsigned char * kbuffer;
	unsigned char * start;
	
	struct exemple_fpga_file_private_s * private_data = filp->private_data;
	struct exemple_fpga_s * exemple_fpga = private_data->exemple_fpga;
	int minor = private_data->minor;
	
	switch (minor) {
		case EXEMPLE_FPGA_MINOR_MEM:
			length = EXEMPLE_FPGA_MODULE_MEM_LENGTH - (* offset);
			start  = exemple_fpga->module_mem + (* offset);
			break;
		case EXEMPLE_FPGA_MINOR_LEDS:
			length = 4;
			start  = exemple_fpga->module_leds;
			break;
		case EXEMPLE_FPGA_MINOR_SWITCHES:
		default:
			return  -EINVAL;
	}
	if (size < length)
		length = size;
	if (length <= 0)
		return 0; // End of file
	
	// Allouer un buffer en memoire noyau.
	kbuffer = kmalloc(length, GFP_KERNEL);
	if (kbuffer == NULL)
		return -ENOMEM;
	
	// Y copier les donnees envoyees par l'espace utilisateur.
	if (copy_from_user(kbuffer, buffer, length) != 0) {
		kfree(kbuffer);
		return -EFAULT;
	}
	
	// Transferer les donnees en I/O
	for (i = 0; i < length; i ++)
		iowrite8(kbuffer[i], start+i);
	
	kfree(kbuffer);
	
	// Mettre a jour l'offset pour la prochaine ecriture
	(* offset) += length;
	// Renvoyer toute la longueur du buffer
	return size;
}



// Fonction de projection en memoire virtuelle (pas sur PXA168)
static int exemple_fpga_mmap(struct file * filp, struct vm_area_struct * vma)
{
	int length;
	unsigned long start;
	
	struct exemple_fpga_file_private_s * private_data = filp->private_data;
	struct exemple_fpga_s * exemple_fpga = private_data->exemple_fpga;
	int minor = private_data->minor;
	
	switch (minor) {
		case EXEMPLE_FPGA_MINOR_MEM:
			length = EXEMPLE_FPGA_MODULE_MEM_LENGTH;
			start  = exemple_fpga->iobase + EXEMPLE_FPGA_MODULE_MEM_START;
			break;
		case EXEMPLE_FPGA_MINOR_LEDS:
			length = 4;
			start  = exemple_fpga->iobase + EXEMPLE_FPGA_MODULE_LEDS_START;
			break;
		case EXEMPLE_FPGA_MINOR_SWITCHES:
			length = 4;
			start  = exemple_fpga->iobase + EXEMPLE_FPGA_MODULE_SWITCHES_START;;
			break;
		default:
			return  -EINVAL;
	}
	vma->vm_flags |= VM_IO | VM_DONTCOPY;
	vma->vm_page_prot = pgprot_noncached(vma->vm_page_prot);
	
	if (io_remap_pfn_range (vma, vma->vm_start,
	                        start >> PAGE_SHIFT,
				vma->vm_end - vma->vm_start, vma->vm_page_prot) != 0)
		return -EAGAIN;
	
	return 0;
}



// Methode d'ioctl sur un module
static long exemple_fpga_ioctl (struct file * filp, unsigned int cmd, unsigned long int arg)
{
	struct exemple_fpga_file_private_s * private_data = filp->private_data;
	struct exemple_fpga_s * exemple_fpga = private_data->exemple_fpga;
	
	// Verifier si la commande est bien destinee a notre driver
	if (_IOC_TYPE(cmd) != EXEMPLE_FPGA_IOCTL_MAGIC_NUMBER)
		return -ENOTTY; // L'erreur "no TTY" est une convention Unix/Linux historique
	
	// Si la commande est une lecture depuis l'espace utilisateur
	if (_IOC_DIR(cmd) & _IOC_READ)
		// Verifier si nous pouvons acceder en ecriture sur le pointeur transmis
		if (access_ok(VERIFY_WRITE, (void __user *) arg, _IOC_SIZE(cmd)) == 0)
			return -EFAULT;
	
	// Si la commande est une ecriture depuis l'espace utilisateur
	if (_IOC_DIR(cmd) & _IOC_WRITE)
		// Verifier si nous pouvons acceder en lecture sur le pointeur transmis
		if (access_ok(VERIFY_READ, (void __user *) arg, _IOC_SIZE(cmd)) == 0)
			return -EFAULT;
	
	// Extraire le numero de commande
	switch(_IOC_NR(cmd)) {
		// Lecture de l'etat d'activation de l'IRQ
		case EXEMPLE_FPGA_IOCTL_COMMAND_GET_ENABLE_IRQ:
			if (copy_to_user((void __user *) arg, & (exemple_fpga->irq_handling), sizeof(int)) != 0)
				return -EFAULT;
			break;
		// Ecriture d'un nouvel etat d'activation de l'IRQ
		case EXEMPLE_FPGA_IOCTL_COMMAND_SET_ENABLE_IRQ:
			if (copy_from_user(& (exemple_fpga->irq_handling), (void __user *) arg, sizeof(int)) != 0)
				return -EFAULT;
			break;
		default :
			return -ENOTTY;
	}
	return 0;
}




// ---------------------------------------------------------------------------
// Gestion de l'interface avec le materiel
// ---------------------------------------------------------------------------



// Handler d'interruption defini plus bas.
static irqreturn_t exemple_fpga_interrupt_handler(int, void *);



// Initialisation de la communication avec le FPGA
static int exemple_fpga_initialize_pci_device (struct pci_dev * dev, struct exemple_fpga_s * exemple_fpga)
{
	int err;
	
	// Activer le peripherique PCI
	if ((err = pci_enable_device(dev)) != 0)
		return err;
	
	// Verifier si on supporte les interruptions MSI
	if (pci_enable_msi(dev) == 0) {
		printk (KERN_DEBUG "EXEMPLE-FPGA uses MSI\n");
		exemple_fpga->msi_enabled = 1;
	} else {
		exemple_fpga->msi_enabled = 0;
	}
	
	// Adresse de depart de la BAR0
	exemple_fpga->iobase = pci_resource_start(dev, 0); // 0 -> BAR0
	
	// Acces a la memoire du port PCIe
	err = pci_request_region(dev, 0, THIS_MODULE->name);
	if (err) {
		printk(KERN_ERR "Error on pci_request_region() -> %d\n", err);
		pci_disable_device(dev);
		return err;
	}
	
	// Projection en memoire du module RAM
	exemple_fpga->module_mem = ioremap_nocache(exemple_fpga->iobase + EXEMPLE_FPGA_MODULE_MEM_START, EXEMPLE_FPGA_MODULE_MEM_LENGTH);
	if (exemple_fpga->module_mem == NULL) {
		printk(KERN_DEBUG "Error on ioremap for PIO MEM");
		pci_release_region(dev, 0);
		pci_disable_device(dev);
		return EINVAL;
	}
	
	// Projection en memoire du module LED
	exemple_fpga->module_leds = ioremap_nocache(exemple_fpga->iobase + EXEMPLE_FPGA_MODULE_LEDS_START, EXEMPLE_FPGA_MODULE_LEDS_LENGTH);
	if (exemple_fpga->module_leds == NULL) {
		printk(KERN_DEBUG "Error on ioremap for PIO LED");
		iounmap(exemple_fpga->module_mem);
		pci_release_region(dev, 0);
		pci_disable_device(dev);
		return EINVAL;
	}
	
	// Projection en memoire du module Switch
	exemple_fpga->module_switches = ioremap_nocache(exemple_fpga->iobase + EXEMPLE_FPGA_MODULE_SWITCHES_START, EXEMPLE_FPGA_MODULE_SWITCHES_LENGTH);
	if (exemple_fpga->module_switches == NULL) {
		printk(KERN_DEBUG "Error on ioremap for PIO SWITCH");
		iounmap(exemple_fpga->module_leds);
		iounmap(exemple_fpga->module_mem);
		pci_release_region(dev, 0);
		pci_disable_device(dev);
		return EINVAL;
	}
	
	// Projection en memoire du module Controls
	exemple_fpga->module_controls = ioremap_nocache(exemple_fpga->iobase + EXEMPLE_FPGA_MODULE_CONTROLS_START, EXEMPLE_FPGA_MODULE_CONTROLS_LENGTH);
	if (exemple_fpga->module_controls == NULL) {
		printk(KERN_DEBUG "Error on ioremap for PIO CONTROL");
		iounmap(exemple_fpga->module_switches);
		iounmap(exemple_fpga->module_leds);
		iounmap(exemple_fpga->module_mem);
		pci_release_region(dev, 0);
		pci_disable_device(dev);
		return EINVAL;
	}
	
	// Le parametre exemple_fpga_disable_irq peut etre fourni sur la ligne
	// de commande de insmod pour interdire tout traitement d'interruption.
	if (! exemple_fpga_disable_irq) {
		
		// Inscription du handler d'interruption
		if (dev->irq != 0) {
			if (request_irq(dev->irq, exemple_fpga_interrupt_handler, IRQF_SHARED, THIS_MODULE->name, exemple_fpga) != 0) {
				printk(KERN_DEBUG "Error on request_irq");
				iounmap(exemple_fpga->module_controls);
				iounmap(exemple_fpga->module_switches);
				iounmap(exemple_fpga->module_leds);
				iounmap(exemple_fpga->module_mem);
				pci_release_region(dev, 0);
				pci_disable_device(dev);
				return EINVAL;
			}
		}
		
		// Edge Capture 
		iowrite32(0x00000003, exemple_fpga->module_switches + 0x0C);
		// Enable PIO IRQ
		iowrite32(0x00000003, exemple_fpga->module_switches + 0x08);
		// Enable PCIe IRQ
		iowrite32(0x00000001, exemple_fpga->module_controls + 0x50);
	}
	
	return 0;
}



// Liberation de la partie basse du driver
static void exemple_fpga_release_pci_device (struct pci_dev * dev, struct exemple_fpga_s * exemple_fpga)
{
	if (! exemple_fpga_disable_irq) {
		
		// Arret des interruptions
		iowrite32(0x00000000, exemple_fpga->module_controls + 0x50);
		iowrite32(0x00000000, exemple_fpga->module_switches + 8);
		
		// Supprimer le handler d'interruption
		if (dev->irq) {
			free_irq(dev->irq, exemple_fpga);
		}
	}
	
	// retirer la projection des PIO
	iounmap(exemple_fpga->module_controls);
	iounmap(exemple_fpga->module_switches);
	iounmap(exemple_fpga->module_leds);
	iounmap(exemple_fpga->module_mem);
	
	// Liberer l'occupation de la memoire PCI
	pci_release_region(dev, 0);
	
	// Desactiver eventuellement le MSI
	if (exemple_fpga->msi_enabled == 1)
		pci_disable_msi(dev);
		
	// Desactiver le peripherique PCI
	pci_disable_device(dev);
}



// Handler d'interruption (commutation des switches)
static irqreturn_t exemple_fpga_interrupt_handler(int num, void * arg)
{
	struct exemple_fpga_s * exemple_fpga = (struct exemple_fpga_s *) arg;
	static int value = 0x00;
	int n;
	
	// Le PCIe a-t-il declenche une interruption ?
	n = ioread32(exemple_fpga->module_controls + 0x40);
	
	printk(KERN_DEBUG "controls+0x40=%d\n", n);
	
	// Sinon indiquer au kernel que l'interruption n'etait pas pour nous.
	if (n == 0) {
		// Clear the PIO Interrupt Mask (to ack the IRQ)
		// iowrite32(0x00000000, exemple_fpga->module_switches + 0x08);
		return IRQ_NONE;
	}
	
	// Quel switch ?
	n = ioread32(exemple_fpga->module_switches + 0x0C);
	
	if (exemple_fpga->irq_handling) {
		// Toggle the bit corresponding to the switch
		value = value ^ n;
		// Output the value to the leds
		iowrite8(value, exemple_fpga->module_leds);
		
	}
	
	// Clear the PIO Interrupt Mask (to ack the IRQ)
	iowrite32(0x00000000, exemple_fpga->module_switches + 0x08);
	// Reset edge trigerring register
	iowrite32(0x00000003, exemple_fpga->module_switches + 0x0C);
	// Then re-enable all PIO interrupts 
	iowrite32(0x00000003, exemple_fpga->module_switches + 0x08);
	
	// Clear the interrupt bit in the Avalon-MM Status Register
	//	iowrite32(0x00000000, exemple_fpga->module_controls + 0x40);
	
	// Re-enable interrupt bit in the Avalon-MM Enable Register
	iowrite32(0x00000001, exemple_fpga->module_controls + 0x50);
	
	return IRQ_HANDLED;
}




// ---------------------------------------------------------------------------
// Methodes du driver PCIe 
// ---------------------------------------------------------------------------



// Methode invoquee à la detection du peripherique (ou au chargement module).
static int exemple_fpga_probe (struct pci_dev *dev, const struct pci_device_id *id)
{
	int err;
	struct exemple_fpga_s * exemple_fpga = NULL;

	// Donnees privees encore invalides.
	pci_set_drvdata(dev, NULL);

	// Il y a un premier appel (?) a ignorer. Son champ irq est nul.
	if (dev->irq == 0)
		return 0;
	
	// Allouer la memoire pour nos donnees privees.
	if ((exemple_fpga = kzalloc(sizeof(struct exemple_fpga_s), GFP_KERNEL)) == NULL)
		return -ENOMEM;
	mutex_init(&(exemple_fpga->mtx_open));
	exemple_fpga->irq_handling = 1;
	
	// Initialiser la partie basse du driver (materiel).
	if ((err = exemple_fpga_initialize_pci_device(dev, exemple_fpga)) != 0) {
		kfree(exemple_fpga);
		return err;
	}
	
	// Inscrire les donnees privees.
	pci_set_drvdata(dev, exemple_fpga);

	// Initialiser la partie haute du driver (interface espace user).
	if ((err = exemple_fpga_initialize_char_device(exemple_fpga)) != 0) {
		exemple_fpga_release_pci_device(dev, exemple_fpga);
		kfree(exemple_fpga);
		return err;
	}
	return 0;
}



// Methode invoquee au retrait du driver (ou du peripherique si hotplug)
static void exemple_fpga_remove (struct pci_dev *dev)
{
	struct exemple_fpga_s * exemple_fpga = NULL;
	
	// Recuperer les donnees privees du driver.
	if ((exemple_fpga = pci_get_drvdata(dev)) == NULL)
		return;
	
	// Terminer la partie haute du driver.
	exemple_fpga_release_char_device(exemple_fpga);
	
	// Terminer la partie basse du driver.
	exemple_fpga_release_pci_device (dev, exemple_fpga);
	
	// Liberer la memoire privee du driver.
	kfree(exemple_fpga);
}




// ---------------------------------------------------------------------------
// Gestion du driver PCIe 
// ---------------------------------------------------------------------------



// Le numero de vendeur PCI d'Altera. 
#ifndef PCI_VENDOR_ID_ALTERA
        #define PCI_VENDOR_ID_ALTERA   0x00001172
#endif



// Numero de peripherique, fourni dans documentation EXEMPLE §3.2.2.
#define PCI_DEVICE_ID_EXEMPLE_FPGA   0x00000004



// Liste des peripheriques PCI geres par le driver defini ci-dessous.
static struct pci_device_id   exemple_id_table [] = {

	{ PCI_DEVICE(PCI_VENDOR_ID_ALTERA, PCI_DEVICE_ID_EXEMPLE_FPGA) },
	{ } // Entree vide pour terminer la table.
};



// Liste des peripheriques PCI geres par le module (si chargemt automatique).
MODULE_DEVICE_TABLE(pci, exemple_id_table);



// La structure decrivant le module PCI
static struct pci_driver exemple_pci_driver = {
	.name       = "EXEMPLE FPGA DRIVER",
	.id_table   = exemple_id_table,
	.probe      = exemple_fpga_probe,
	.remove     = exemple_fpga_remove,
};



// Fonction appelee automatiquement au chargement du module.
static int __init exemple_fpga_init(void)
{
	int err;
	
	// Creer la classe specifique pour notre peripherique.
	exemple_fpga_class = class_create(THIS_MODULE, THIS_MODULE->name);
	if (IS_ERR(exemple_fpga_class)) {
		exemple_fpga_class = NULL;
		return -EINVAL;
	}
	
	// L'enregistrement du driver provoque l'appel de la methode probe
	// (si le materiel est present).
	if ((err = pci_register_driver(& exemple_pci_driver)) != 0) {
		printk(KERN_ERR "%s: Erreur dans pci_register(): %d\n",
		       THIS_MODULE->name, err);
		class_destroy(exemple_fpga_class);
		exemple_fpga_class = NULL;
		return err;
	}
	return 0; // Chargement Ok
}



// Fonction invoquee au retrait du module.
static void __exit exemple_fpga_exit(void)
{
	// Le des-enregistrement du driver va appeler sa methode "remove".
	pci_unregister_driver(& exemple_pci_driver);
	
	// Suppression de la classe specifique.
	if (exemple_fpga_class != NULL)
		class_destroy(exemple_fpga_class);
}



// Points d'entree du module.
module_init (exemple_fpga_init);
module_exit (exemple_fpga_exit);



// Information sur le module (pour outils comme modinfo).
MODULE_DESCRIPTION("FPGA design driver on Altera card for #####");
MODULE_AUTHOR("christophe.blaess@logilin.fr");
MODULE_LICENSE("GPL");

/****************************************************************************/
