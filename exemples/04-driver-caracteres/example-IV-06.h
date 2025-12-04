/* SPDX-License-Identifier: GPL-2.0 */
//
// Exemples de la formation
//  "Ecriture de drivers et programmation noyau Linux"
// Chapitre "Driver en mode caracteres"
//
// (c) 2001-2025 Christophe Blaess
//
//    https://www.logilin.fr/
//

#ifndef EXAMPLE_06_H
#define EXAMPLE_06_H

	#include <asm/ioctl.h>

	#define EXAMPLE_IOCTL_MAGIC     'x'

	#define EXAMPLE_INTERNAL_GET_PPID_FLAG  1
	#define EXAMPLE_INTERNAL_SET_PPID_FLAG  2

	#define EXAMPLE_GET_PPID_FLAG  _IOR(EXAMPLE_IOCTL_MAGIC, EXAMPLE_INTERNAL_GET_PPID_FLAG, int)
	#define EXAMPLE_SET_PPID_FLAG  _IOW(EXAMPLE_IOCTL_MAGIC, EXAMPLE_INTERNAL_SET_PPID_FLAG, int)

#endif

