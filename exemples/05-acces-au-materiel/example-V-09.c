/************************************************************************\
  Exemples de la formation
    "Ecriture de drivers et programmation noyau Linux"
  Chapitre "Acces au materiel"

  (c) 2005-2019 Christophe Blaess
  http://www.blaess.fr/christophe/

\************************************************************************/

	#include <linux/device.h>
	#include <linux/fs.h>
	#include <linux/miscdevice.h>
	#include <linux/module.h>
	#include <linux/gpio.h>
	#include <linux/interrupt.h>
	#include <linux/sched.h>
	#include <linux/uaccess.h>
	#include <asm/uaccess.h>

	#include "gpio-examples.h"


	#define EXAMPLE_BUFFER_SIZE 1024
	static unsigned long example_buffer[EXAMPLE_BUFFER_SIZE];
	static int           example_buffer_end = 0;
	static spinlock_t    example_buffer_spl;
	static DECLARE_WAIT_QUEUE_HEAD(example_buffer_wq);


	static irqreturn_t example_handler(int irq, void * ident);

	static ssize_t example_read  (struct file * filp, char * buffer,
	                              size_t length, loff_t * offset);

	static struct file_operations example_fops = {
		.owner   =  THIS_MODULE,
		.read    =  example_read,
	};

	static struct miscdevice example_misc_driver = {
		    .minor          = MISC_DYNAMIC_MINOR,
		    .name           = THIS_MODULE->name,
		    .fops           = & example_fops,
	};


static int __init example_init (void)
{
	int err;

	if ((err = gpio_request(EXAMPLE_GPIO_IN,THIS_MODULE->name)) != 0)
		return err;

	if ((err = gpio_direction_input(EXAMPLE_GPIO_IN)) != 0) {
		gpio_free(EXAMPLE_GPIO_IN);
		return err;
	}

	spin_lock_init(& example_buffer_spl);

	if ((err = request_irq(gpio_to_irq(EXAMPLE_GPIO_IN), example_handler,
	                       IRQF_SHARED | IRQF_TRIGGER_RISING,
	                       THIS_MODULE->name, THIS_MODULE->name)) != 0) {
		gpio_free(EXAMPLE_GPIO_IN);
		return err;
	}

	if ((err = misc_register(& example_misc_driver)) != 0) {
		free_irq(gpio_to_irq(EXAMPLE_GPIO_IN), THIS_MODULE->name);
		gpio_free(EXAMPLE_GPIO_IN);
		return err;
	}

	return 0;
}


static void __exit example_exit (void)
{
	misc_deregister(& example_misc_driver);
	free_irq(gpio_to_irq(EXAMPLE_GPIO_IN), THIS_MODULE->name);
	gpio_free(EXAMPLE_GPIO_IN);
}


static ssize_t example_read(struct file * filp, char * buffer,
                            size_t length, loff_t * offset)
{
	char k_buffer[80];
	unsigned long irqs;

	spin_lock_irqsave(& example_buffer_spl, irqs);

	while (example_buffer_end == 0) {
		spin_unlock_irqrestore(& example_buffer_spl, irqs);
		if (filp->f_flags & O_NONBLOCK)
			return -EAGAIN;
		if (wait_event_interruptible(example_buffer_wq,
		                    (example_buffer_end != 0)) != 0)
			return -ERESTARTSYS;
		spin_lock_irqsave(& example_buffer_spl, irqs);
	}

	snprintf(k_buffer, 80, "%ld\n", example_buffer[0]);
	if (length < (strlen(k_buffer)+1)) {
		spin_unlock_irqrestore(& example_buffer_spl, irqs);
		return -ENOMEM;
	}

	example_buffer_end --;
	if (example_buffer_end > 0)
		memmove(example_buffer, & (example_buffer[1]), example_buffer_end * sizeof(long int));

	spin_unlock_irqrestore(& example_buffer_spl, irqs);

	if (copy_to_user(buffer, k_buffer, strlen(k_buffer)+1) != 0)
		return -EFAULT;

	return strlen(k_buffer)+1;
}


static irqreturn_t example_handler(int irq, void * ident)
{
	spin_lock(& example_buffer_spl);

	if (example_buffer_end < EXAMPLE_BUFFER_SIZE) {
		example_buffer[example_buffer_end] = jiffies;
		example_buffer_end ++;
	}
	spin_unlock(& example_buffer_spl);
	wake_up_interruptible(& example_buffer_wq);

	return IRQ_HANDLED;
}


	module_init(example_init);
	module_exit(example_exit);

	MODULE_DESCRIPTION("Blocking and non-blocking read() system call.");
	MODULE_AUTHOR("Christophe Blaess <Christophe.Blaess@Logilin.fr>");
	MODULE_LICENSE("GPL");
