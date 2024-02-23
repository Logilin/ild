// SPDX-License-Identifier: GPL-2.0
//
// Exemples de la formation
//  "Ecriture de drivers et programmation noyau Linux"
//
// (c) 2001-2024 Christophe Blaess
//
//    https://www.logilin.fr/
//

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void)
{
	char hostname[HOST_NAME_MAX + 1];

	gethostname(hostname, HOST_NAME_MAX);

	printf("Hello from %s\n", hostname);

	return EXIT_SUCCESS;
}
