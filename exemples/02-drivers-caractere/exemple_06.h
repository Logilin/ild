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

	#define EXEMPLE_IOCTL_MAGIC     'x'

	#define EXEMPLE_GET_PPID_FLAG  1
	#define EXEMPLE_SET_PPID_FLAG  2

	#define EXIOCGPPIDF _IOR(EXEMPLE_IOCTL_MAGIC, EXEMPLE_GET_PPID_FLAG, int)
	#define EXIOCSPPIDF _IOW(EXEMPLE_IOCTL_MAGIC, EXEMPLE_SET_PPID_FLAG, int)

#endif
