// ***********************************************************************
// Chapitre "Ecriture de driver - peripherique caractere"
// exemple_ioctl
//
// Utilisation d'un ioctl dans l'espace utilisateur pour ouvrir
// le lecteur de CD
//
// Exemples de la formation "Programmation Noyau sous Linux"
// (c) 2005-2010 Christophe Blaess - http://www.blaess.fr/christophe/
//
// *********************************************************************

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
