/* **************** LFD331:4.2 s_16/anon_mmap.c **************** */
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
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char **argv)
{
        int fd = -1, size = 4096, status;
        char *area;
        pid_t pid;

        area =
            mmap(NULL, size, PROT_READ | PROT_WRITE,
                 MAP_SHARED | MAP_ANONYMOUS, fd, 0);

        pid = fork();
        if (pid == 0) {         /* child */
                strcpy(area, "This is a message from the child");
                printf("Child has written: %s\n", area);
                exit(EXIT_SUCCESS);
        }
        if (pid > 0) {          /* parent */
                wait(&status);
                printf("Parent has read:   %s\n", area);
                exit(EXIT_SUCCESS);
        }
        exit(EXIT_FAILURE);
}
