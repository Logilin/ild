/************************************************************************\
  Exemples de la formation
    "Ecriture de drivers et programmation noyau Linux"
  Chapitre "Acces au materiel"

  (c) 2005-2025 Christophe Blaess
  http://www.blaess.fr/christophe/

\************************************************************************/

#ifndef _GPIO_EXAMPLES_H_
#define _GPIO_EXAMPLES_H_

#include <linux/version.h>

#define EXAMPLE_RASPBERRY_PI       1
#define EXAMPLE_BEAGLEBONE_BLACK   2
#define EXAMPLE_TERASIC_DE1_SOC    3


// Uncomment one of the following lines depending on you plateform:

//#define EXAMPLE_GPIO_MODE   EXAMPLE_RASPBERRY_PI
#define EXAMPLE_GPIO_MODE   EXAMPLE_BEAGLEBONE_BLACK
// #define EXAMPLE_GPIO_MODE   EXAMPLE_TERASIC_DE1_SOC


#ifndef EXAMPLE_GPIO_MODE
	#error "Please select your platform in gpio_examples.h"
#else

	#if EXAMPLE_GPIO_MODE == EXAMPLE_RASPBERRY_PI
		#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 6, 0)
			// Input on pin 16
			#define EXAMPLE_GPIO_IN  594
			// Output on pin 18
			#define EXAMPLE_GPIO_OUT 595
		#else
			// Input on pin 16
			#define EXAMPLE_GPIO_IN  23
			// Output on pin 18
			#define EXAMPLE_GPIO_OUT 24
		#endif
	#elif EXAMPLE_GPIO_MODE == EXAMPLE_BEAGLEBONE_BLACK
		// Input on pin P8-07 (GPIO 66)
		#define EXAMPLE_GPIO_IN  66
		// Output on LED 4 (GPIO 56)
		#define EXAMPLE_GPIO_OUT 56
	#elif EXAMPLE_GPIO_MODE == EXAMPLE_TERASIC_DE1_SOC
		// Input on the push button
		#define EXAMPLE_GPIO_IN  223
		// Output on the led.
		#define EXAMPLE_GPIO_OUT 222
	#else
		#error "Unknown platform definition in gpio_examples.h"
	#endif
#endif

#endif

