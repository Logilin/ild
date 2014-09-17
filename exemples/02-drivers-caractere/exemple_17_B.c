/************************************************************************\
  exemple_17 - Chapitre "Ecriture de driver - peripherique caractere"

  Appel-systeme read() bloquant en attente d'interruption

  Exemples de la formation "Programmation Noyau sous Linux"

  (c) 2005-2014 Christophe Blaess
  http://www.blaess.fr/christophe/

\************************************************************************/

#include <linux/device.h>
#include <linux/fs.h>
#include <linux/interrupt.h>
#include <linux/ioport.h>
#include <linux/miscdevice.h>
#include <linux/module.h>
#include <linux/sched.h>

#include <asm/uaccess.h>
#include <asm/io.h>


	#define EXEMPLE_LG_BUFFER 1024
	static unsigned long exemple_buffer[EXEMPLE_LG_BUFFER];
	static int           exemple_fin_buffer = 0;
	static spinlock_t    exemple_spinlock_buffer;
	static DECLARE_WAIT_QUEUE_HEAD(exemple_waitqueue_buffer);


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

	if (request_region(0x378, 3, THIS_MODULE->name) == NULL)
		return -EBUSY;

	if ((err = request_irq(7, exemple_handler,
	                       IRQF_SHARED,
	                       THIS_MODULE->name, THIS_MODULE->name)) != 0) {
		release_region(0x378, 3);
		return err;
	}

	spin_lock_init(& exemple_spinlock_buffer);

	if ((err = misc_register(& exemple_misc_driver)) != 0) {
		free_irq(7, THIS_MODULE->name);
		release_region(0x378, 3);
		return err;
	}
	outb(0x10, 0x37A);
	return 0;
}


static void __exit exemple_exit (void)
{
	outb(0x00, 0x37A);
	misc_deregister(& exemple_misc_driver);
	free_irq(7, THIS_MODULE->name);
	release_region(0x378, 3);
}


static ssize_t exemple_read(struct file * filp, char * buffer,
                        size_t length, loff_t * offset)
{
	char k_buffer[4];
	unsigned long irqs;


	if (length < 4)
		return 0;

	spin_lock_irqsave(& exemple_spinlock_buffer, irqs);
	while (exemple_fin_buffer == 0) {
		spin_unlock_irqrestore(& exemple_spinlock_buffer, irqs);
		if (wait_event_interruptible(exemple_waitqueue_buffer,
		                    (exemple_fin_buffer != 0)) != 0)
			return -ERESTARTSYS;
		spin_lock_irqsave(& exemple_spinlock_buffer, irqs);
		
	}

	sprintf(k_buffer, "%02X ", (int) (exemple_buffer[0]));
	exemple_fin_buffer --;
	if (exemple_fin_buffer > 0)
		memmove(exemple_buffer, & (exemple_buffer[1]), exemple_fin_buffer);

	spin_unlock_irqrestore(& exemple_spinlock_buffer, irqs);
	if (copy_to_user(buffer, k_buffer, strlen(k_buffer)+1) != 0)
		return -EFAULT;
	return strlen(k_buffer)+1;
}


static irqreturn_t exemple_handler(int irq, void * ident)
{
	int val = inb(0x379);

	if ((val & 0x40) == 0)
		return IRQ_NONE;

	val = val & 0xBF;
	spin_lock(& exemple_spinlock_buffer);
	if (exemple_fin_buffer < EXEMPLE_LG_BUFFER) {
		exemple_buffer[exemple_fin_buffer] = val;
		exemple_fin_buffer ++;
	}
	spin_unlock(& exemple_spinlock_buffer);
	wake_up_interruptible(& exemple_waitqueue_buffer);
	return IRQ_HANDLED;
}


module_init(exemple_init);
module_exit(exemple_exit);
MODULE_LICENSE("GPL");

