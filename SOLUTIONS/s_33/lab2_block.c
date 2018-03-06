/* **************** LFD331:4.2 s_33/lab2_block.c **************** */
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
 * Read/Write Block Driver
 *
 * Extend the previous exercise in order to put an ext4 file system
 * (or another type) on your device.  You can place a filesystem on
 * the device with
 *
    $ mkfs.ext4 /dev/mybdrv
    $ mount /dev/mybdrv mnt
 *
 * where you give the appropriate name of the device node and mount
 * point.
 *
 * For an additional enhancement, try partitioning the device with
 * fdisk. For this you may need an additional ioctl() for HDIO_GETGEO,
 * and you'll have to include: linux/hdreg.h. This ioctl returns a
 * pointer to the following structure:
 *
    struct hd_geometry {
	unsigned char heads;
	unsigned char sectors;
	unsigned short cylinders;
	unsigned long start;
    };
 *
 * Remember the total capacity is (sector size ) x (sectors/track) x
 * (cylinders) x (heads). You also want to use a value of 4 for the
 * starting sector.
 *
 * If you are using a recent kernel and version of udev, the partition
 * nodes should be made automatically when you load the driver;
 * otherwise you will have to actually add them manually.
 *
 * After you make the partitions you should be able to format them and
 * mount and use them with commands like:
 *
   $ mkfs.ext4 /dev/mybdrv1
   $ mkfs.ext3 /dev/mybdrv2
   $ mkfs.vfat /dev/mybdrv3
   $ mkdir 1 2 3
   $ mount /dev/mybdrv1 1
   $ mount /dev/mybdrv2 2
   $ mount /dev/mybdrv3 3
  *
 @*/

#include <linux/fs.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/vmalloc.h>
#include <linux/blkdev.h>
#include <linux/genhd.h>
#include <linux/errno.h>
#include <linux/hdreg.h>
#include <linux/version.h>

#define MY_DEVICE_NAME "mybdrv"

static int mybdrv_ma_no, diskmb = 256, disk_size;
static char *ramdisk;
static struct gendisk *my_gd;
static spinlock_t lock;
static unsigned short sector_size = 512;
static struct request_queue *my_request_queue;

module_param_named(size, diskmb, int, 0);
static void my_request(struct request_queue *q)
{
	struct request *rq;
	int size, res = 0;
	char *ptr;
	unsigned nr_sectors, sector;
	pr_info("entering request routine\n");

	rq = blk_fetch_request(q);
	while (rq) {
		if (rq->cmd_type != REQ_TYPE_FS) {
			pr_warn
			    ("This was not a normal fs request, skipping\n");
			goto done;
		}
		nr_sectors = blk_rq_cur_sectors(rq);
		sector = blk_rq_pos(rq);

		ptr = ramdisk + sector * sector_size;
		size = nr_sectors * sector_size;

		if ((ptr + size) > (ramdisk + disk_size)) {
			pr_warn(" tried to go past end of device\n");
			goto done;
		}

		if (rq_data_dir(rq)) {
			pr_info("writing at sector %d, %u sectors\n",
				sector, nr_sectors);
			memcpy(ptr, bio_data(rq->bio), size);
		} else {
			pr_info("reading at sector %d, %u sectors\n",
				sector, nr_sectors);
			memcpy(bio_data(rq->bio), ptr, size);
		}
done:
		if (!__blk_end_request_cur(rq, res))
			rq = blk_fetch_request(q);
	}
	pr_info("leaving request\n");
}

static int my_ioctl(struct block_device *bdev, fmode_t mode,
		    unsigned int cmd, unsigned long arg)
{
	long size;
	struct hd_geometry geo;

	pr_info("cmd=%d\n", cmd);

	switch (cmd) {
	case HDIO_GETGEO:
		pr_info("HIT HDIO_GETGEO\n");
		/*
		 * get geometry: we have to fake one...  trim the size to a
		 * multiple of 64 (32k): tell we have 16 sectors, 4 heads,
		 * whatever cylinders. Tell also that data starts at sector. 4.
		 */

		size = disk_size;
		size &= ~0x3f;	/* multiple of 64 */
		geo.cylinders = size >> 6;
		geo.heads = 4;
		geo.sectors = 16;
		geo.start = 4;

		if (copy_to_user((void __user *)arg, &geo, sizeof(geo)))
			return -EFAULT;

		return 0;
	}
	pr_warn("return -ENOTTY\n");

	return -ENOTTY;		/* unknown command */
}

static const struct block_device_operations mybdrv_fops = {
	.owner = THIS_MODULE,
	.ioctl = my_ioctl,
};

static int __init my_init(void)
{
	disk_size = diskmb * 1024 * 1024;
	spin_lock_init(&lock);

	ramdisk = vmalloc(disk_size);
	if (!ramdisk)
		return -ENOMEM;

	my_request_queue = blk_init_queue(my_request, &lock);
	if (!my_request_queue) {
		vfree(ramdisk);
		return -ENOMEM;
	}
	blk_queue_logical_block_size(my_request_queue, sector_size);

	mybdrv_ma_no = register_blkdev(0, MY_DEVICE_NAME);
	if (mybdrv_ma_no < 0) {
		pr_err("Failed registering mybdrv, returned %d\n",
		       mybdrv_ma_no);
		vfree(ramdisk);
		return mybdrv_ma_no;
	}

	my_gd = alloc_disk(16);
	if (!my_gd) {
		unregister_blkdev(mybdrv_ma_no, MY_DEVICE_NAME);
		vfree(ramdisk);
		return -ENOMEM;
	}

	my_gd->major = mybdrv_ma_no;
	my_gd->first_minor = 0;
	my_gd->fops = &mybdrv_fops;
	strcpy(my_gd->disk_name, MY_DEVICE_NAME);
	my_gd->queue = my_request_queue;
	set_capacity(my_gd, disk_size / sector_size);
	add_disk(my_gd);

	pr_info("device successfully   registered, Major No. = %d\n",
		mybdrv_ma_no);
	pr_info("Capacity of ram disk is: %d MB\n", diskmb);

	return 0;
}

static void __exit my_exit(void)
{
	del_gendisk(my_gd);
	put_disk(my_gd);
	unregister_blkdev(mybdrv_ma_no, MY_DEVICE_NAME);
	pr_info("module successfully unloaded, Major No. = %d\n", mybdrv_ma_no);
	blk_cleanup_queue(my_request_queue);
	vfree(ramdisk);
}

module_init(my_init);
module_exit(my_exit);

MODULE_AUTHOR("Jerry Cooperstein");
MODULE_DESCRIPTION("LFD331:4.2 s_33/lab2_block.c");
MODULE_LICENSE("GPL v2");
