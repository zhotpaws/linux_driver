/* **************** LFD331:4.2 s_05/lab5_with_header.c **************** */
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
 * Dynamical Node Creation (II)
 *
 * Adapt the previous dynamic registration driver to use udev to
 * create the device node on the fly.
 *
 * A second solution is given which includes a header file
 * (lab_header.h) which will be used to simplify the coding in
 * subsequent character drivers we will write.
 *
 @*/

#include <linux/module.h>
#include "lab_char.h"

static const struct file_operations mycdrv_fops = {
	.owner = THIS_MODULE,
	.read = mycdrv_generic_read,
	.write = mycdrv_generic_write,
	.open = mycdrv_generic_open,
	.release = mycdrv_generic_release,
	.llseek = mycdrv_generic_lseek,
};

module_init(my_generic_init);
module_exit(my_generic_exit);

MODULE_AUTHOR("Jerry Cooperstein");
MODULE_DESCRIPTION("LFD331:4.2 s_05/lab5_with_header.c");
MODULE_LICENSE("GPL v2");
