/************************************************************************\
  Exemples de la formation
    "Ecriture de drivers et programmation noyau Linux"
  Chapitre "Driver en mode caracteres"

  (c) 2005-2019 Christophe Blaess
  http://www.blaess.fr/christophe/

\************************************************************************/

#ifndef EXAMPLE_06_H
#define EXAMPLE_06_H

	#include <asm/ioctl.h>

	#define EXAMPLE_IOCTL_MAGIC     'x'

	#define EXAMPLE_GET_PPID_FLAG  1
	#define EXAMPLE_SET_PPID_FLAG  2

	#define EXIOCGPPIDF _IOR(EXAMPLE_IOCTL_MAGIC, EXAMPLE_GET_PPID_FLAG, int)
	#define EXIOCSPPIDF _IOW(EXAMPLE_IOCTL_MAGIC, EXAMPLE_SET_PPID_FLAG, int)

#endif

