// SPDX-License-Identifier: GPL-2.0
//
// Exemples de la formation
//  "Ecriture de drivers et programmation noyau Linux"
// Chapitre "Driver en mode caracteres"
//
// (c) 2001-2025 Christophe Blaess
//
//    https://www.logilin.fr/
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include "example-IV-06.h"


int main(int argc, char *argv[])
{
	int fd;
	int ppid_flag;

	if (argc < 2) {
		fprintf(stderr, "usage: %s filename [ppid_flag]\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	fd = open(argv[1], O_RDONLY, 0);
	if (fd < 0) {
		perror(argv[1]);
		exit(EXIT_FAILURE);
	}

	if (argc > 2)
		if (sscanf(argv[2], "%d", &ppid_flag) == 1)
			if (ioctl(fd, EXAMPLE_SET_PPID_FLAG, &ppid_flag) != 0)
				perror("ioctl(EXAMPLE_SET_PPID_FLAG)");

	if (ioctl(fd, EXAMPLE_GET_PPID_FLAG, &ppid_flag) != 0) {
		perror("ioctl(EXAMPLE_GET_PPID_FLAG)");
		exit(EXIT_FAILURE);
	}
	fprintf(stdout, "%d\n", ppid_flag);
	return 0;
}

