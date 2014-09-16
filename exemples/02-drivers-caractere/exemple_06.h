/************************************************************************\
  exemple_06 - Chapitre "Ecriture de driver - peripherique caractere"

  Fonction d'ioctl permettant d'afficher ou non le PPID

  Exemples de la formation "Programmation Noyau sous Linux"

  (c) 2005-2014 Christophe Blaess
  http://www.blaess.fr/christophe/

\************************************************************************/

#ifndef EXEMPLE_06_H
#define EXEMPLE_06_H

	#include <asm/ioctl.h>
	
	#define TYPE_IOCTL_EXEMPLE	'x'
	
	#define EX_GET_AFFICHE_PPID  1
	#define EX_SET_AFFICHE_PPID  2
	
	#define EX_IOCGAFFPPID _IOR(TYPE_IOCTL_EXEMPLE, EX_GET_AFFICHE_PPID, int)
	#define EX_IOCSAFFPPID _IOW(TYPE_IOCTL_EXEMPLE, EX_SET_AFFICHE_PPID, int)
	
#endif
