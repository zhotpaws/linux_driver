/* **************** LFD331:4.2 s_12/lab2_ioctl_data_test.c **************** */
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
 * Using ioctl's to pass data (User-space application)
 @*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <string.h>

struct my_data {
	int i;
	long x;
	char s[256];
} my_data;

#define MYIOC_TYPE 'k'

int main(int argc, char *argv[])
{
	int fd, rc;
	int MY_IOCTL;
	char *nodename = "/dev/mycdrv";

	/* open the device node */

	if (argc > 1)
		nodename = argv[1];
	fd = open(nodename, O_RDWR);
	printf(" I opened the device node, file descriptor = %d\n", fd);

	/* retrieve the original values; */

	MY_IOCTL = (int)_IOR(MYIOC_TYPE, 1, struct my_data);
	rc = ioctl(fd, MY_IOCTL, &my_data);
	printf("\n rc from ioctl reading = %d \n\n", rc);
	printf
	    ("    my_data.i = %d\n    my_data.x = %ld\n    my_data.s = %s\n",
	     my_data.i, my_data.x, my_data.s);

	/* fill up the data structure */

	strcpy(my_data.s, "a string");
	my_data.i = 10;
	my_data.x = -700;
	printf("\n Sending from user-space:\n");
	printf
	    ("    my_data.i = %d\n    my_data.x = %ld\n    my_data.s = %s\n",
	     my_data.i, my_data.x, my_data.s);

	/* send to the device */

	MY_IOCTL = (int)_IOW(MYIOC_TYPE, 1, struct my_data);
	rc = ioctl(fd, MY_IOCTL, &my_data);
	printf("\n rc from ioctl = %d \n\n", rc);

	/* reread device and see if it got through */

	MY_IOCTL = (int)_IOR(MYIOC_TYPE, 1, struct my_data);
	rc = ioctl(fd, MY_IOCTL, &my_data);
	printf("\n rc from ioctl reading = %d \n\n", rc);
	printf
	    ("    my_data.i = %d\n    my_data.x = %ld\n    my_data.s = %s\n",
	     my_data.i, my_data.x, my_data.s);

	close(fd);
	exit(EXIT_SUCCESS);

}
