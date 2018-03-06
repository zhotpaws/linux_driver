/* **************** LFD331:4.2 s_16/lab4_relay_read.c **************** */
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
 * Using Relay Channels. (read()reading program)
 @*/
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>

int main(int argc, char **argv)
{
	int fd, j, rc;
	char buf[64];
	char *fname = "/sys/kernel/debug/my_rc_file0";
	if (argc > 1)
		fname = argv[1];
	fd = open(fname, O_RDONLY);
	printf("opening %s, fd=%d\n", fname, fd);

	for (j = 1; j < 20; j++) {
		rc = read(fd, buf, 64);
		printf("rc=%d    %s::", rc, buf);
	}
	exit(EXIT_SUCCESS);
}
