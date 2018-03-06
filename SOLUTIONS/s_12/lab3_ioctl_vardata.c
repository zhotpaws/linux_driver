/* **************** LFD331:4.2 s_12/lab3_ioctl_vardata.c **************** */
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
 * Using ioctl's to pass data of variable length.
 *
 * Extend the previous exercise to send a buffer whose length is
 * determined at run time.  You will probably need to use the _IOC
 * macro directly in the user-space program.  (See linux/ioctl.h.)
 @*/

#include <linux/module.h>

/* either of these (but not both) will work */
/* #include "lab_char.h" */
#include "lab_miscdev.h"

#define MYIOC_TYPE 'k'

static inline long
mycdrv_unlocked_ioctl(struct file *fp, unsigned int cmd, unsigned long arg)
{
	int i, rc, direction;
	int size;
	char *buffer;
	void __user *ioargp = (void __user *)arg;

	/* make sure it is a valid command */

	if (_IOC_TYPE(cmd) != MYIOC_TYPE) {
		dev_err(my_dev, " got invalid case, CMD=%d\n", cmd);
		return -EINVAL;
	}

	/* get the size of the buffer and kmalloc it */

	size = _IOC_SIZE(cmd);
	buffer = kmalloc((size_t) size, GFP_KERNEL);
	if (!buffer) {
		dev_err(my_dev, "Kmalloc failed for buffer\n");
		return -ENOMEM;
	}

	/* fill it with X */

	memset(buffer, 'X', size);

	direction = _IOC_DIR(cmd);

	switch (direction) {

	case _IOC_WRITE:
		dev_info(my_dev,
			 " reading = %d bytes from user-space and writing to device\n",
			 size);
		rc = copy_from_user(buffer, ioargp, size);
		dev_info(my_dev, "rc from copy_from_user = %d\n", rc);
		break;

	case _IOC_READ:
		dev_info(my_dev,
			 " reading device and writing = %d bytes to user-space\n",
			 size);
		rc = copy_to_user(ioargp, buffer, size);
		dev_info(my_dev, "rc from copy_to_user = %d\n", rc);
		break;

	default:
		dev_err(my_dev, " got invalid case, CMD=%d\n", cmd);
		return -EINVAL;
	}
	for (i = 0; i < size; i++)
		dev_info(my_dev, "%c", buffer[i]);
	dev_info(my_dev, "\n");

	kfree(buffer);
	return rc;
}

static const struct file_operations mycdrv_fops = {
	.owner = THIS_MODULE,
	.unlocked_ioctl = mycdrv_unlocked_ioctl,
	.open = mycdrv_generic_open,
	.release = mycdrv_generic_release
};

module_init(my_generic_init);
module_exit(my_generic_exit);

MODULE_AUTHOR("Jerry Cooperstein");
MODULE_DESCRIPTION("LFD331:4.2 s_12/lab3_ioctl_vardata.c");
MODULE_LICENSE("GPL v2");
