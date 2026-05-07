// SPDX-License-Identifier: GPL-2.0
//
// Exemples de la formation
//  "Ecriture de drivers et programmation noyau Linux"
// Chapitre "Driver en mode caracteres"
//
// (c) 2001-2026 Christophe Blaess
//
//    https://www.logilin.fr/
//

#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/gpio.h>
#include <linux/gpio/consumer.h>
#include <linux/miscdevice.h>
#include <linux/module.h>
#include <linux/of_platform.h>
#include <linux/uaccess.h>
#include <asm/uaccess.h>


struct gpio_desc *gpio_in = NULL;
struct gpio_desc *gpio_out = NULL;


static ssize_t example_read(struct file *filp, char *u_buffer, size_t length, loff_t *offset)
{
	char k_buffer[8];

	if (length < 2)
		return 0;
	sprintf(k_buffer, "%d\n", gpiod_get_value(gpio_in));
	if (copy_to_user(u_buffer, k_buffer, 2) != 0)
		return -EFAULT;

	return 2;
}


static ssize_t example_write(struct file *filp, const char *buffer, size_t length, loff_t *offset)
{
	char k_buffer[80];
	int val;

	if (length > 79)
		return -ENOMEM;

	if (copy_from_user(k_buffer, buffer, length) != 0)
		return -EFAULT;
	k_buffer[length] = '\0';

	if (kstrtoint(k_buffer, 10, &val) != 0)
		return -EINVAL;

	gpiod_set_value(gpio_out, val);

	return length;
}


static const struct file_operations example_fops = {
	.owner   =  THIS_MODULE,
	.read    =  example_read,
	.write   =  example_write,
};


static struct miscdevice example_misc_driver = {
	.minor          = MISC_DYNAMIC_MINOR,
	.name           = THIS_MODULE->name,
	.fops           = &example_fops,
	.mode           = 0666,
};


static int example_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	int err;

	gpio_in = devm_gpiod_get(dev, "in", GPIOD_IN);
	if (IS_ERR(gpio_in)) {
		pr_err("%s: Unable to get gpio `in` \n", THIS_MODULE->name);
		return -EBUSY;
	}

	gpio_out = devm_gpiod_get(dev, "out", GPIOD_OUT_LOW);
	if (IS_ERR(gpio_out)) {
		gpiod_put(gpio_in);
		pr_err("%s: Unable to get gpio `out` \n", THIS_MODULE->name);
		return -EBUSY;
	}

	err = misc_register(&example_misc_driver);
	if (err != 0) {
		gpiod_put(gpio_out);
		gpiod_put(gpio_in);
		return err;
	}


	return 0;
}


static int example_remove(struct platform_device *pdev)
{
	misc_deregister(&example_misc_driver);

	gpiod_put(gpio_out);
	gpiod_put(gpio_in);

	return 0;
}

static const struct of_device_id example_of_match[] = {
	{ .compatible = "logilin,example" },
	{ }
};
MODULE_DEVICE_TABLE(of, example_of_match);

static struct platform_driver example_driver = {
	.probe  = example_probe,
	.remove = example_remove,
	.driver = {
		.name = "example",
		.of_match_table = example_of_match,
	},
};

module_platform_driver(example_driver);

MODULE_DESCRIPTION("Read and write system call on GPIO pins.");
MODULE_AUTHOR("Christophe Blaess <Christophe.Blaess@Logilin.fr>");
MODULE_LICENSE("GPL v2");
