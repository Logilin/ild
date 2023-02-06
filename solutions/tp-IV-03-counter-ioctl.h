/* SPDX-License-Identifier: GPL-2.0 */
//
// Exemples de la formation
//  "Ecriture de drivers et programmation noyau Linux"
//
// (c) 2001-2023 Christophe Blaess
//
//    https://www.logilin.fr/
//

#ifndef TP_IV_COUNTER_H
#define TP_IV_COUNTER_H

	#include <asm/ioctl.h>

	#define COUNTER_IOCTL_MAGIC     'c'

	#define SET_STEP_COMMAND  1

	#define COUNTER_SET_STEP _IOW(COUNTER_IOCTL_MAGIC, SET_STEP_COMMAND, int)

#endif

