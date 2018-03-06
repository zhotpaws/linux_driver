/* **************** LFD331:4.2 s_23/lab1_dma.c **************** */
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
 * DMA Memory Allocation
 *
 * Extend your character or miscellaneous driver module to allocate
 * and map a suitable DMA buffer, and obtain the bus address handle.
 *
 * Do this in three ways:
 *
 *    Using dma_alloc_coherent().
 *    Using dma_map_single()
 *    Using a DMA Pool.
 *
 * Compare the resulting kernel and bus addresses; how do they differ?
 * Compare with the value of PAGE_OFFSET.
 *
 * In each case copy a string into the buffer and make sure it can be
 * read back properly.
 *
 * In the case of dma_map_single(), you may want to compare the use of
 * different direction arguments.
@*/

#include <linux/module.h>
#include "lab_miscdev.h"
/* #include "lab_char.h" */
#include <linux/pci.h>
#include <linux/dma-mapping.h>
#include <linux/dmapool.h>

static char *kbuf;
static dma_addr_t handle;
static size_t size = (10 * PAGE_SIZE), pool_size = 1024, pool_align = 8;
static struct dma_pool *mypool;

/* int direction = PCI_DMA_TODEVICE ; */
/*/ int direction = PCI_DMA_FROMDEVICE ; */
static int direction = PCI_DMA_BIDIRECTIONAL;
/* int direction = PCI_DMA_NONE; */

/* data and code to set my_dev for dma_map_single */
static u64 *prev_dma_mask_ptr;
static u64 my_dma_mask;

static int patch_struct_dev_for_dma_map_single(void)
{
	prev_dma_mask_ptr = my_dev->dma_mask;
	if (prev_dma_mask_ptr == NULL)
		pr_warn("previous dma_mask pointer was NULL\n");
	my_dev->dma_mask = &my_dma_mask;
	if (dma_set_mask(my_dev, DMA_BIT_MASK(32)) != 0) {
		pr_info("FAILED:  dma_set_mask()\n");
		my_dev->dma_mask = prev_dma_mask_ptr;	/* unpatch */
		return -1;
	}
	return 0;
}

static void unpatch_struct_dev_for_dma_map_single(void)
{
	my_dev->dma_mask = prev_dma_mask_ptr;
}

static void output(char *kbuf, dma_addr_t handle, size_t size, char *string)
{
	unsigned long diff;
	diff = (unsigned long)kbuf - handle;
	dev_info(my_dev, "kbuf=%12p, handle=%12p, size = %d\n", kbuf,
		 (unsigned long *)handle, (int)size);
	dev_info(my_dev,
		 "(kbuf-handle)= %12p, %12lu, PAGE_OFFSET=%12lu, compare=%lu\n",
		 (void *)diff, diff, PAGE_OFFSET, diff - PAGE_OFFSET);
	strcpy(kbuf, string);
	dev_info(my_dev, "string written was, %s\n", kbuf);
}

static const struct file_operations mycdrv_fops = {
	.owner = THIS_MODULE,
};

static int __init my_init(void)
{
	int rc;
	rc = my_generic_init();
	if (rc < 0)
		return rc;
	dev_info(my_dev, "Loading DMA allocation test module\n");

	/* dma_alloc_coherent method */

	dev_info(my_dev, "\nTesting dma_alloc_coherent()..........\n\n");
	kbuf = dma_alloc_coherent(NULL, size, &handle, GFP_KERNEL);
	output(kbuf, handle, size, "This is the dma_alloc_coherent() string");
	dma_free_coherent(NULL, size, kbuf, handle);

	/* dma_map/unmap_single */

	dev_info(my_dev, "\nTesting dma_map_single()................\n\n");
	if (patch_struct_dev_for_dma_map_single() != 0) {
		pr_warn("skipping dma_map_single test\n");
	} else {
		kbuf = kmalloc(size, GFP_KERNEL);
		handle = dma_map_single(my_dev, kbuf, size, direction);
		output(kbuf, handle, size,
		       "This is the dma_map_single() string");
		dma_unmap_single(my_dev, handle, size, direction);
		kfree(kbuf);

		/* dma_pool method */

		dev_info(my_dev, "\nTesting dma_pool_alloc()..........\n\n");
		mypool =
		    dma_pool_create("mypool", my_dev, pool_size, pool_align, 0);
		kbuf = dma_pool_alloc(mypool, GFP_KERNEL, &handle);
		output(kbuf, handle, size,
		       "This is the dma_pool_alloc() string");
		dma_pool_free(mypool, kbuf, handle);
		dma_pool_destroy(mypool);

		unpatch_struct_dev_for_dma_map_single();
	}

	return rc;
}

module_init(my_init);
module_exit(my_generic_exit);

MODULE_AUTHOR("Jerry Cooperstein");
MODULE_AUTHOR("Bill Kerr");
MODULE_AUTHOR("John Bonesio");
MODULE_DESCRIPTION("LFD331:4.2 s_23/lab1_dma.c");
MODULE_LICENSE("GPL v2");
