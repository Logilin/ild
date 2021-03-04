// SPDX-License-Identifier: GPL-2.0
//
// Exemples de la formation
//  "Ecriture de drivers et programmation noyau Linux"
// Chapitre "Acces au matriel"
//
// (c) 2001-2021 Christophe Blaess
//
//    https://www.logilin.fr/
//

// A small non-blocking cat implementation

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>


void nonblocking_cat(int fd)
{
	char buffer[4096];
	int n;
	char *prompt = "|/-\\";
	int i = 0;

	fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);

	while ((n = read(fd, buffer, 4096)) != 0) {
		if (n > 0) {
			write(STDOUT_FILENO, buffer, n);
			continue;
		}

		write(STDOUT_FILENO, &(prompt[i]), 1);
		write(STDOUT_FILENO, "\b", 1);
		i++;
		if (i == 4)
			i = 0;
		usleep(100000);
	}
}


int main(int argc, char *argv[])
{
	int i;
	int fd;

	for (i = 1; i < argc; i++) {
		fd = open(argv[i], O_RDONLY, 0);
		if (fd < 0)
			continue;
		nonblocking_cat(fd);
		close(fd);
	}
	if (argc == 1)
		nonblocking_cat(STDIN_FILENO);

	return 0;
}

