// SPDX-License-Identifier: GPL-2.0
//
// Exemples de la formation
//  "Ecriture de drivers et programmation noyau Linux"
// Chapitre "Driver en mode caracteres"
//
// (c) 2001-2025 Christophe Blaess
//
//    https://www.logilin.fr/
//

#include <linux/device.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/module.h>
#include <linux/poll.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>
#include <linux/sched.h>

#include <asm/uaccess.h>

#include "gpio-examples.h"


#define EXAMPLE_ARRAY_SIZE 1024
static unsigned long example_array[EXAMPLE_ARRAY_SIZE];
static int           example_array_end = 0;
static spinlock_t    example_spinlock;
static DECLARE_WAIT_QUEUE_HEAD(example_waitqueue);


static ssize_t example_read(struct file * filp, char * buffer,
                            size_t length, loff_t * offset)
{
	char k_buffer[80];
	unsigned long irqs;

	spin_lock_irqsave(& example_spinlock, irqs);

	while (example_array_end == 0) {
		spin_unlock_irqrestore(& example_spinlock, irqs);
		if (filp->f_flags & O_NONBLOCK)
			return -EAGAIN;
		if (wait_event_interruptible(example_waitqueue,
		                    (example_array_end != 0)) != 0)
			return -ERESTARTSYS;
		spin_lock_irqsave(& example_spinlock, irqs);

	}

	snprintf(k_buffer, 80, "%ld\n", example_array[0]);
	if (length < (strlen(k_buffer)+1)) {
		spin_unlock_irqrestore(& example_spinlock, irqs);
		return -ENOMEM;
	}

	example_array_end --;
	if (example_array_end > 0)
		memmove(example_array, & (example_array[1]), example_array_end * sizeof(long int));

	spin_unlock_irqrestore(& example_spinlock, irqs);

	if (copy_to_user(buffer, k_buffer, strlen(k_buffer)+1) != 0)
		return -EFAULT;
	return strlen(k_buffer)+1;
}


static unsigned int example_poll  (struct file * filp, poll_table * table)
{
	int ret = 0;
	unsigned long irqs;

	poll_wait(filp, & example_waitqueue, table);
	spin_lock_irqsave(& example_spinlock, irqs);
	if (example_array_end > 0)
		ret = POLLIN | POLLRDNORM;
	spin_unlock_irqrestore(& example_spinlock, irqs);

	return ret;
}


static irqreturn_t example_handler(int irq, void * ident)
{
	spin_lock(& example_spinlock);

	if (example_array_end < EXAMPLE_ARRAY_SIZE) {
		example_array[example_array_end] = jiffies;
		example_array_end ++;
	}
	spin_unlock(& example_spinlock);
	wake_up_interruptible(& example_waitqueue);
	return IRQ_HANDLED;
}


static struct file_operations example_fops = {
	.owner   =  THIS_MODULE,
	.read    =  example_read,
	.poll    =  example_poll,
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

	spin_lock_init(& example_spinlock);

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


module_init(example_init);
module_exit(example_exit);
MODULE_LICENSE("GPL");

