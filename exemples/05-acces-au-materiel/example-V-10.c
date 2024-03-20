// SPDX-License-Identifier: GPL-2.0
//
// Exemples de la formation
//  "Ecriture de drivers et programmation noyau Linux"
// Chapitre "Driver en mode caracteres"
//
// (c) 2001-2024 Christophe Blaess
//
//    https://www.logilin.fr/
//


#include <linux/device.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/module.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/version.h>
#include <linux/wait.h>
#include <asm/uaccess.h>

#include "gpio-examples.h"


#define EXAMPLE_ARRAY_SIZE 1024

struct example_data {

	unsigned long          array[EXAMPLE_ARRAY_SIZE];
	int                    array_end;
	spinlock_t             array_spl;
	wait_queue_head_t      array_wq;

};



static irqreturn_t example_handler(int irq, void *ident)
{
	struct example_data *data = ident;

	spin_lock(&data->array_spl);

	if (data->array_end < EXAMPLE_ARRAY_SIZE) {
		data->array[data->array_end] = jiffies;
		data->array_end++;
	}
	spin_unlock(&data->array_spl);
	wake_up_interruptible(&data->array_wq);

	return IRQ_HANDLED;
}



static int example_open(struct inode *ind, struct file *filp)
{
	int err;
	struct example_data *data;

	err = -ENOMEM;

	data = kmalloc(sizeof(struct example_data), GFP_KERNEL);
	if (data == NULL)
		goto out;

	data->array_end = 0;
	spin_lock_init(&data->array_spl);
	init_waitqueue_head(&data->array_wq);

	filp->private_data = data;

	err = gpio_request(EXAMPLE_GPIO_IN, THIS_MODULE->name);
	if (err != 0)
		goto free_data;

	err = gpio_direction_input(EXAMPLE_GPIO_IN);
	if (err != 0)
		goto free_gpio;

	err = request_irq(gpio_to_irq(EXAMPLE_GPIO_IN), example_handler,
		IRQF_SHARED | IRQF_TRIGGER_RISING,
		THIS_MODULE->name, data);
	if (err != 0)
		goto free_gpio;

	return 0;

free_gpio:
		gpio_free(EXAMPLE_GPIO_IN);

free_data:
	kfree(data);

out:
	return err;
}



static int example_release(struct inode *ind, struct file *filp)
{
	struct example_data *data = filp->private_data;

	free_irq(gpio_to_irq(EXAMPLE_GPIO_IN), data);
	gpio_free(EXAMPLE_GPIO_IN);
	kfree(data);

	return 0;
}



static ssize_t example_read(struct file *filp, char *u_buffer, size_t length, loff_t *offset)
{
	unsigned long irqs;
	char k_buffer[80];
	struct example_data *data = filp->private_data;
	unsigned long value;

	spin_lock_irqsave(&data->array_spl, irqs);

	while (data->array_end == 0) {
		spin_unlock_irqrestore(&data->array_spl, irqs);
		if (filp->f_flags & O_NONBLOCK)
			return -EAGAIN;
		if (wait_event_interruptible(data->array_wq, (data->array_end != 0)) != 0)
			return -ERESTARTSYS;
		spin_lock_irqsave(&data->array_spl, irqs);
	}

	value = data->array[0];

	data->array_end--;
	if (data->array_end > 0)
		memmove(data->array, &(data->array[1]), data->array_end * sizeof(long));

	spin_unlock_irqrestore(&data->array_spl, irqs);

	snprintf(k_buffer, 80, "%ld\n", value);
	if (length < (strlen(k_buffer)+1))
		return -ENOMEM;

	if (copy_to_user(u_buffer, k_buffer, strlen(k_buffer)+1) != 0)
		return -EFAULT;

	return strlen(k_buffer)+1;
}


static const struct file_operations example_fops = {
	.owner   =  THIS_MODULE,
	.open    =  example_open,
	.release =  example_release,
	.read    =  example_read,
};


static struct miscdevice example_misc_driver = {
	.minor   = MISC_DYNAMIC_MINOR,
	.name    = THIS_MODULE->name,
	.fops    = &example_fops,
	.mode    = 0666,
};



#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 9, 0)
	module_misc_device(example_misc_driver);
#else
	module_driver(example_misc_driver, misc_register, misc_deregister)
#endif


MODULE_DESCRIPTION("Blocking and non-blocking read() system call.");
MODULE_AUTHOR("Christophe Blaess <Christophe.Blaess@Logilin.fr>");
MODULE_LICENSE("GPL v2");
