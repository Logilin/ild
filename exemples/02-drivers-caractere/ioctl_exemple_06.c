/************************************************************************\
  Chapitre "Ecriture de driver - peripherique caractere"
  test_exemple_06
  
  Fonction d'ioctl

  Exemples de la formation "Programmation Noyau sous Linux"
  (c) 2005-2008 Christophe Blaess - http://www.blaess.fr/christophe/

\************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include "exemple_06.h"

int main(int argc, char * argv[])
{
	int fd;
	int affiche_ppid;

	if (argc < 2) {
		fprintf(stderr, "usage: %s fichier [affiche_ppid]\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	fd = open(argv[1], O_RDONLY, 0);
	if (fd < 0) {
		perror(argv[1]);
		exit(EXIT_FAILURE);
	}
	if (argc > 2)
		if (sscanf(argv[2], "%d", & affiche_ppid) == 1)
			if (ioctl(fd, EX_IOCSAFFPPID, & affiche_ppid) != 0)
				perror("ioctl(IOCSAFFPPID)");
	
	if (ioctl(fd, EX_IOCGAFFPPID, & affiche_ppid) != 0) {
		perror("ioctl(IOCGAFFPPID)");
		exit(EXIT_FAILURE);
	}
	fprintf(stdout, "%d\n", affiche_ppid);
	return 0;
}

