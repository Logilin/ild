// SPDX-License-Identifier: GPL-2.0
//
// Exemples de la formation
//  "Ecriture de drivers et programmation noyau Linux"
//
// (c) 2001-2022 Christophe Blaess
//
//    https://www.logilin.fr/
//

#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/version.h>
#include <asm/uaccess.h>


static int counter;


static ssize_t counter_read(struct file *filp, char *buffer, size_t max_lg, loff_t *offset)
{
	char kbuffer[128];
	int lg;

	snprintf(kbuffer, 128, "Counter = %d\n", counter);
	lg = strlen(kbuffer) - (*offset);
	if (lg <= 0)
		return 0;
	if (lg > max_lg)
		lg = max_lg;

	if (copy_to_user(buffer, &kbuffer[*offset], lg) != 0)
		return -EFAULT;
	counter++;
	*offset += lg;
	return lg;
}


static ssize_t counter_write(struct file *filp, const char *ubuffer, size_t lg, loff_t *offset)
{
	int nb;
	char *kbuffer;

	kbuffer = kmalloc(lg + 1, GFP_KERNEL);
	if (kbuffer == NULL)
		return -ENOMEM;

	copy_from_user(kbuffer, ubuffer, lg);
	kbuffer[lg] = '\0';

	nb = kstrtoint(kbuffer, 10, &counter);
	kfree(kbuffer);
	if (nb != 0)
		return -EINVAL;

	return lg;
}


static const struct file_operations counter_fops = {
	.owner   =  THIS_MODULE,
	.read    =  counter_read,
	.write   =  counter_write,
};


static struct miscdevice counter_misc_driver = {
	.minor          = MISC_DYNAMIC_MINOR,
	.name           = "counter",
	.fops           = &counter_fops,
	.mode           = 0666,
};


static int __init counter_init(void)
{
	return misc_register(&counter_misc_driver);
}


static void __exit counter_exit(void)
{
	misc_deregister(&counter_misc_driver);
}


module_init(counter_init);
module_exit(counter_exit);

MODULE_DESCRIPTION("chapter IV exercise solution.");
MODULE_AUTHOR("Christophe Blaess <christophe.blaess@logilin.fr>");
MODULE_LICENSE("GPL v2");

