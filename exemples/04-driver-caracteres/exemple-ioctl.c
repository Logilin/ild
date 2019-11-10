/************************************************************************\
  Exemples de la formation
    "Ecriture de drivers et programmation noyau Linux"
  Chapitre "Driver en mode caracteres"

  (c) 2005-2019 Christophe Blaess
  http://www.blaess.fr/christophe/

\************************************************************************/

#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <linux/cdrom.h>


int main(int argc, char * argv[])
{
	int fd;

	fd = open("/dev/cdrom", O_RDONLY|O_NONBLOCK);
	if (fd < 0) {
		perror("/dev/cdrom");
		exit(1);
	}

	if (ioctl(fd, CDROMEJECT, 0) < 0) {
		perror("ioctl(CDROMEJECT)");
		exit(1);
	}

	return 0;
}

