/************************************************************************\
  TP de la formation
    "Ecriture de drivers et programmation noyau Linux"
  Chapitre "Programmer pour le noyau Linux"

  (c) 2005-2015 Christophe Blaess
  http://www.blaess.fr/christophe/

\************************************************************************/

	#include <stdio.h>
	#include <sys/time.h>

#define NB_SAMPLES 50

int main(void)
{
	int i;
	static struct timeval tv [NB_SAMPLES];

	for (i = 0; i < NB_SAMPLES; i ++)
		gettimeofday(&(tv[i]), NULL);

	for (i = 0; i < NB_SAMPLES; i ++)
		fprintf(stderr, "%2d : %ld.%06ld\n", i, tv[i].tv_sec, tv[i].tv_usec);
	return 0;
}
