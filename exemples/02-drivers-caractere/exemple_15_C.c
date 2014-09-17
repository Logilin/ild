/************************************************************************\
  exemple_15 - Chapitre "Ecriture de driver - peripherique caractere"

  Synchronisation par spinlock entre appel-systeme et interruption

  Exemples de la formation "Programmation Noyau sous Linux"

  (c) 2005-2014 Christophe Blaess
  http://www.blaess.fr/christophe/

\************************************************************************/

#include <linux/device.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/module.h>
#include <linux/interrupt.h>

#include <asm/uaccess.h>


	static int irq_number = 1;
	module_param(irq_number, int, 0444);

	#define EXEMPLE_LG_BUFFER 1024
	static unsigned long exemple_buffer[EXEMPLE_LG_BUFFER];
	static int           exemple_fin_buffer = 0;
	static spinlock_t    exemple_spinlock_buffer;


	static irqreturn_t exemple_handler(int irq, void * ident);

	static ssize_t exemple_read  (struct file * filp, char * buffer,
	                              size_t length, loff_t * offset);


	static struct file_operations fops_exemple = {
		.owner   =  THIS_MODULE,
		.read    =  exemple_read,
	};


	static struct miscdevice exemple_misc_driver = {
		    .minor          = MISC_DYNAMIC_MINOR,
		    .name           = THIS_MODULE->name,
		    .fops           = & fops_exemple,
	};


static int __init exemple_init (void)
{
	int err;

	if ((err = request_irq(irq_number, exemple_handler,
	                       IRQF_SHARED,
	                       THIS_MODULE->name, THIS_MODULE->name)) != 0) {
		return err;
	}

	spin_lock_init(& exemple_spinlock_buffer);

	if ((err = misc_register(& exemple_misc_driver)) != 0) {
		free_irq(irq_number, THIS_MODULE->name);
		return err;
	}
	return 0;
}

static void __exit exemple_exit (void)
{
	misc_deregister(& exemple_misc_driver);
	free_irq(irq_number, THIS_MODULE->name);
}


static ssize_t exemple_read(struct file * filp, char * buffer,
                            size_t length, loff_t * offset)
{
	unsigned long irqs;
	char k_buffer[80];

	spin_lock_irqsave(& exemple_spinlock_buffer, irqs);

	if (exemple_fin_buffer == 0) {
		spin_unlock_irqrestore(& exemple_spinlock_buffer, irqs);
		return 0;
	}
	
	snprintf(k_buffer, 80, "%ld\n", exemple_buffer[0]);
	exemple_fin_buffer --;
	if (exemple_fin_buffer > 0)
		memmove(exemple_buffer, & (exemple_buffer[1]), exemple_fin_buffer * sizeof(unsigned long));
	
	spin_unlock_irqrestore(& exemple_spinlock_buffer, irqs);

	if (length < (strlen(k_buffer) +1))
		return -ENOMEM;

	if (copy_to_user(buffer, k_buffer, strlen(k_buffer) + 1) != 0)
		return -EFAULT;
	return strlen(k_buffer) + 1;
}


static irqreturn_t exemple_handler(int irq, void * ident)
{
	spin_lock(& exemple_spinlock_buffer);
	
	if (exemple_fin_buffer < EXEMPLE_LG_BUFFER) {
		exemple_buffer[exemple_fin_buffer] = jiffies;
		exemple_fin_buffer ++;
	}
	spin_unlock(& exemple_spinlock_buffer);
	return IRQ_HANDLED;
}

module_init(exemple_init);
module_exit(exemple_exit);
MODULE_LICENSE("GPL");

