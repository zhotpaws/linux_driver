/* **************** LFD331:4.2 s_14/lab1_firmware_chardev.c **************** */
/*
 * The code herein is: Copyright the Linux Foundation, 2015
 *
 * This Copyright is retained for the purpose of protecting free
 * redistribution of source.
 *
 *     URL:    http://training.linuxfoundation.org
 *     email:  trainingquestions@linuxfoundation.org
 *
 * This code is distributed under Version 2 of the GNU General Public
 * License, which you should have received with the source.
 *
 */
/*
 * Loading Firmware (char/misc device solution)

 * Write a module that loads some firmware from the filesystem. It
 * should print out the contents.
 *
 * In order to do this you'll need to place a firmware file under
 * /lib/firmware. You can use just a text file for the purpose of this
 * demonstration.

 * Since this is a pseudo device, you will have to declare and
 * initialize a device structure. Minimally you must set the void
 * (*release)(struct device *dev) field in this structure, and call
 *
 *    int dev_set_name (struct device *dev, const char *fmt, ...);
 *
 * to set the device name, which can be read out with:
 *
 *    const char *dev_name(const struct device *dev);
 *
 * (You may want to see what happens if you neglect setting one of
 * these quantities.)
 *
 * Make sure you call
 *
 *    device_register  (struct device *dev);
 *    device_unregister(struct device *dev);
 *
 * before requesting and after releasing the firmware.
 *
 * As an alternative (and easier solution) just adapt your previous
 * miscellaneous or character driver to do this.
 @*/

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt
#include <linux/module.h>
#include "lab_miscdev.h"
/* #include "lab_char.h" */

#include <linux/firmware.h>
#define FWFILE "my_fwfile"
static const struct firmware *fw;

static const struct file_operations mycdrv_fops = {
	.owner = THIS_MODULE,
	.read = mycdrv_generic_read,
	.write = mycdrv_generic_write,
	.open = mycdrv_generic_open,
	.release = mycdrv_generic_release,
	.llseek = mycdrv_generic_lseek,
};

static int __init my_init(void)
{
	if (my_generic_init() < 0)
		return -1;

	dev_info(my_dev, "firmware_example: my device inserted\n");

	if (request_firmware(&fw, FWFILE, my_dev)) {
		pr_err("requesting firmware failed\n");
		return -1;
	}
	dev_info(my_dev, "firmware contents=\n%s\n", fw->data);
	return 0;
}

static void __exit my_exit(void)
{
	release_firmware(fw);
	dev_info(my_dev, "release firwmare and unregistered device\n");
	my_generic_exit();
}

module_init(my_init);
module_exit(my_exit);

MODULE_AUTHOR("Jerry Cooperstein");
MODULE_DESCRIPTION("LFD331:4.2 s_14/lab1_firmware_chardev.c");
MODULE_LICENSE("GPL v2");
