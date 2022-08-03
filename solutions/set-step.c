// SPDX-License-Identifier: GPL-2.0
//
// Exemples de la formation
//  "Ecriture de drivers et programmation noyau Linux"
//
// (c) 2001-2022 Christophe Blaess
//
//    https://www.logilin.fr/
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include "tp-IV-03-counter-ioctl.h"


int main(int argc, char *argv[])
{
	int fd;
	int step;

	if (argc < 3) {
		fprintf(stderr, "usage: %s <filename> <step>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	fd = open(argv[1], O_RDONLY, 0);
	if (fd < 0) {
		perror(argv[1]);
		exit(EXIT_FAILURE);
	}

	if (sscanf(argv[2], "%d", &step) != 1) {
		fprintf(stderr, "invalid value for step\n");
		exit(EXIT_FAILURE);
	}

	if (ioctl(fd, COUNTER_SET_STEP, &step) != 0)
				perror("ioctl(COUNTER_SET_STEP)");

	return EXIT_SUCCESS;
}