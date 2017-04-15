/************************************************************************\
  Exemples de la formation
    "Ecriture de drivers et programmation noyau Linux"
  Chapitre "Acces au materiel"

  (c) 2005-2017 Christophe Blaess
  http://www.blaess.fr/christophe/

\************************************************************************/

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>


int main(int argc, char * argv[])
{
	int fd;
	char * ptr;

	if ((argc != 2)
	 || ((fd = open(argv[1], O_RDWR, 0)) < 0)) {
		fprintf(stderr, "usage: %s <device>\n", argv[0]);
		exit(1);
	}

	ptr = mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (ptr == MAP_FAILED) {
		perror("mmap");
		exit(1);
	}

	while (1) {
		fprintf(stderr, "%s", ptr);
		usleep(100000); /* 1/10e s. */
	}

	munmap(ptr, 4096);
	close(fd);

	return 0;
}

